//Файл taskwithprom.h
#pragma once

#include <future>
#include <memory>

//Для задачи
typedef std::function<void()> taskType;

//Структура, описывающая задачу с promise
struct TaskWithProm : public std::enable_shared_from_this<TaskWithProm>{
   taskType task_;//сама задача
   bool isDone_;//для проверки выполнения задачи
   bool thisIsFirst_;//это самая первая задача
   std::promise<void> prom_;//промис
   std::shared_ptr<TaskWithProm> parent_;//умный указатель на родителя

   //Конструктор по умолчанию
   TaskWithProm() : isDone_(false), thisIsFirst_(false), prom_(), parent_(nullptr) {}

   //Получить указатель на объект
   std::shared_ptr<TaskWithProm> getPtr(){
      return shared_from_this();
   }
};
