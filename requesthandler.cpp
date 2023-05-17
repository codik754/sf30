//Файл requesthandler.cpp
#include "requesthandler.h"

//Конструктор по умолчанию
RequestHandler::RequestHandler() : n_(1){
   //Стартуем пул
   thpool_.start();
}

//Деструктор
RequestHandler::~RequestHandler(){
   //Останавливаем пул
   thpool_.stop();
}

//Пробрасываем запрос в пул
resFuture RequestHandler::pushTask(FuncType f, std::shared_ptr<int[]> array, long arg1, long arg2){
   //Залочиваем мьютекс
   std::lock_guard<std::mutex> l(mutex_);
   if(n_ == 1){
      ++n_;//увеличиваем счетчик
      //добавляем в пул задачу и возврщаем фьючерс
      return thpool_.pushTask(f, array, arg1, arg2);
   }else{
      //Добавляем в пул
      fv_.push_back(thpool_.pushTask(f, array, arg1, arg2));
      //Возвращаем пустой фьючерс
      resFuture res;
      return res;
   }
}

//Ожидать завершения всех фьючерсов
void RequestHandler::waitFutures(){
   //Проходим по всему вектору фьючерсов
   for(size_t i = 0; i < fv_.size(); ++i){
      fv_[i].wait();
   }
   //Очищаем вектор
   fv_.clear();
}
