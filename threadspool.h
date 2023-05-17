//Файл threadspool.h
#pragma once

#include "taskwithprom.h"
#include "blockedqueue.h"
#include <functional>
#include <future>
#include <thread>
#include <vector>
#include <memory>
#include <mutex>

//Для future
typedef std::future<void> resFuture;

//Для типа функции
typedef resFuture (*FuncType)(std::shared_ptr<int[]>, long, long);

//Указатель, который будет хранить текущую задачу в каждом потоке
extern thread_local std::shared_ptr<TaskWithProm> currentTask;

//Класс для пула потоков
class ThreadsPool{
   int threadCount_;//количество потоков
   int index_;//для равномерного распределения задач
   int amount_;//счетчик количества задач
   std::mutex mutex_;//мьютекс
   std::vector<std::thread> vthreads_;//вектор потоков
   std::vector<BlockedQueue<std::shared_ptr<TaskWithProm>>> tasksQueue_;//очередь задач
public:
   //Конструктор по умолчанию
   ThreadsPool();
   
   //Запуск пула
   void start();
   //Остановка пула
   void stop();
   //Добавление в очередь задач
   resFuture pushTask(FuncType f, std::shared_ptr<int[]> array, long arg1, long arg2);
   //Функция выполнения для потоков
   void threadFunc(int qindex);
};
