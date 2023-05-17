//Файл requesthandler.h
#pragma once

#include "threadspool.h"
#include <vector>
#include <mutex>
#include <memory>

//Класс для объектов, которые будут принимать запрос и
//пробрабрасывать его на обработку в пул
class RequestHandler{
   ThreadsPool thpool_;//пул потоков
   std::vector<resFuture> fv_;//вектор фьючерсо
   std::mutex mutex_;//мьютекс
   int n_;//переменная, которая нужна для отделения самого первого возвращаемого фьючерса от остальнных
public:
   //Конструктор по умолчанию
   RequestHandler();
   //Деструктор
   ~RequestHandler();

   //Пробрасываем запрос в пул
   resFuture pushTask(FuncType f, std::shared_ptr<int[]> array, long arg1, long arg2);

   //Ожидать завершения всех фьючерсов
   void waitFutures();
};
