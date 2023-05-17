//Файл blockedqueue.h
#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

//Потокобезопасная очередь
template<class T>
class BlockedQueue{
   std::mutex mutex_;//мьютекс
   std::queue<T> taskQueue_;//очередь
   std::condition_variable notifier_;//условная переменная
public:
   //Конструктор по умолчанию
   BlockedQueue() : mutex_(), taskQueue_(), notifier_() {}

   //Вставить элемент в очередь
   void push(T &item);

   //Достать элемент из очереди. Метод является блокирующим
   void pop(T &item);

   //Быстрый метод получения элемента. Неблокирующий метод.
   bool fastPop(T &item);
};


//Вставить элемент в очередь
template<class T>
void BlockedQueue<T>::push(T &item){
   //Залочиваем мьютекс
   std::lock_guard<std::mutex> l(mutex_);
   //Обычный push
   taskQueue_.push(item);
   //Делаем оповещение, чтобы поток проснулся
   notifier_.notify_one();
}

//Достать элемент из очереди. Метод является блокирующим
template<class T>
void BlockedQueue<T>::pop(T &item){
   //Залочиваем мьютекс
   std::unique_lock<std::mutex> l(mutex_);
   //Если очередь пуста
   if(taskQueue_.empty()){
      //Ждем, когда вызовут push
      notifier_.wait(l, [this](){ return !taskQueue_.empty(); });
   }
   //Берем элемент из очереди
   item = std::move(taskQueue_.front());
   //Удаляем элемент из очереди
   taskQueue_.pop();
}

//Быстрый метод получения элемента. Неблокирующий метод.
template<class T>
bool BlockedQueue<T>::fastPop(T &item){
   //Залочиваем мьютекс
   std::lock_guard<std::mutex> l(mutex_);
   //Если очередь пуста
   if(taskQueue_.empty()){
      //Выходим
      return false;
   }
   //Забираем элемент
   item = std::move(taskQueue_.front());
   //Убираем его из очереди
   taskQueue_.pop();
   //Возвращаем true
   return true;
}
