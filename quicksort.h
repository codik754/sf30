//Файл quicksort.h
#pragma once

#include <memory>
#include "requesthandler.h"

extern RequestHandler pool;

//Функция, реализующая алгоритм быстрой сортировки(без многопоточности)
void quickSortWithout(std::shared_ptr<int[]> array, long left, long right);

//Функция, реализующая алгоритм быстрой сортировки(c многопоточностью)
resFuture quickSortWith(std::shared_ptr<int[]> array, long left, long right);
