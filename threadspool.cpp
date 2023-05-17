//Файл threadspool.cpp
#include "threadspool.h"

thread_local std::shared_ptr<TaskWithProm> currentTask = nullptr;

//Конструктор по умолчанию
ThreadsPool::ThreadsPool() : threadCount_(std::thread::hardware_concurrency() != 0 ? std::thread::hardware_concurrency() : 4), index_(0), amount_(0), mutex_(),  vthreads_(),  tasksQueue_(threadCount_){}

//Запуск пула
void ThreadsPool::start(){
   //Создаем потоки
   for(int i = 0; i < threadCount_; ++i){
      vthreads_.emplace_back(std::thread(&ThreadsPool::threadFunc, this, i));
   }
}

//Остановка пула
void ThreadsPool::stop(){
   //Отправляем в каждый поток пустую задачу
   for(int i = 0; i < threadCount_; ++i){
      std::shared_ptr<TaskWithProm> emptyTask = std::make_shared<TaskWithProm>();
      tasksQueue_[i].push(emptyTask);
   }

   //Дожидаемся задач
   for(auto &t : vthreads_){
      t.join();
   }
}

//Добавение в очередь задач
resFuture ThreadsPool::pushTask(FuncType f, std::shared_ptr<int[]> array,long arg1, long arg2){
   //Вычисляем индекс
   int queueToPush = index_++ % threadCount_;
   //Создаем новую задачу
   std::shared_ptr<TaskWithProm> newTask = std::make_shared<TaskWithProm>();

   //Записываем лямбда выражение в качестве функции
   newTask->task_ = [=](){ f(array, arg1, arg2); };
   
   //Связываме std::future с std::promise
   resFuture res = newTask->prom_.get_future();

   //Записываем родительскую задачу
   //Если текущий указатель в потоке nullptr, то записываем nullptr
   if(!currentTask){
      newTask->parent_ = currentTask;
   }
   else{
      //Иначе получаем указатель с помощью shared_from_this()
      newTask->parent_ = currentTask->getPtr();
   }
   //Добавляем в очередь
   tasksQueue_[queueToPush].push(newTask);
   
   //Залочиваем мьютекс
   mutex_.lock();
   //Если это самая первая задача
   if(amount_ == 0){
      //Устанавливаем бит показывающий первую задачу
      newTask->thisIsFirst_ = true;
   }
   //Увеличить счетчик количества задач
   ++amount_;
   //Разлочиваем мьютекс
   mutex_.unlock();

   //Возвращаем фьючерс
   return res;
}

//Функция выполнения для потоков
void ThreadsPool::threadFunc(int qindex){
   while(true){
      //Обработка очередной задачи
      std::shared_ptr<TaskWithProm> taskToDo;
      bool res = false;
      int i = 0;
      for(; i < threadCount_; ++i){
         //Попытка быстро забрать задачу
         res = tasksQueue_[(qindex + i) % threadCount_].fastPop(taskToDo);
         if(res){
            break;
         }
      }
      //Если не получилось забрать
      if(!res){
         //То делаем с помощью блокирующего получения
         tasksQueue_[qindex].pop(taskToDo);
      }
      else if(!taskToDo->task_){
         //Если при быстром получении было получено пустое задание
         //возвращаемого его назад 
         tasksQueue_[(qindex + i) % threadCount_].push(taskToDo);
      }
      //Если пустое задание
      if(!taskToDo->task_){
         //выходим из потока
         return;
      }
      
      //Обнуляем указатель на текущую задачу в потоке
      if(currentTask){
         currentTask.reset();
      }
      
      //Если задача еще не выполнялась, то выполним ее
      if(!taskToDo->isDone_){
         //Теперь текущая задача та, которую взяли из очереди
         currentTask = taskToDo->getPtr();
         //Выполняем задачу
         taskToDo->task_();
         //Устанавливаем флаг выполнения задачи
         taskToDo->isDone_ = true;
         //Обнуляем указатель на текущую задачу в потоке
         currentTask.reset();
      }
      
      //Делаем weak_ptr
      std::weak_ptr<TaskWithProm> wptr = taskToDo; 
      //Проверяем количество используемых ссылок на указатель
      if(wptr.use_count() > 1){
         //Если на данную задачу еще кто-то ссылается, то возвращаем ее в очередь
         tasksQueue_[(qindex + i) % threadCount_].push(taskToDo);
      }
      else{
         //Если это самая первая задача надо сделать так, чтобы ее готовность была установлена самой последней
         if(taskToDo->thisIsFirst_){
            //Залочиваем мьютес
            mutex_.lock();
            //Если количество задач больше 1. То есть есть еще задачи помимо этой
            if(amount_ > 1){
               //Возвращаем задачу в очередь
               tasksQueue_[(qindex + i) % threadCount_].push(taskToDo);
               //Ращлочиваем мьютекс
               mutex_.unlock();
               //Возвращаемся в начало цикла. Делаем еще одну итерацию
               continue;
            }
            //Разлочиваем мьютекс
            mutex_.unlock();
         }
         //Иначе устанавливаем промис
         taskToDo->prom_.set_value();

         //Очищаем указатель
         taskToDo.reset();
         
         //Залочиваем мьютекс и уменьшаем счетчик количества задач
         mutex_.lock();
         //Уменьшаем счетчик задач на 1
         --amount_;
         //Разлочиваем мьютекс
         mutex_.unlock();
      }
   }
}
