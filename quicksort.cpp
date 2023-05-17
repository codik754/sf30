//Файл quicksort.cpp
#include "quicksort.h"

RequestHandler pool;

//Функция, реализующая алгоритм быстрой сортировки(без многопоточности)
void quickSortWithout(std::shared_ptr<int[]> array, long left, long right){
   if(left >= right){
      return;
   }
   long leftBound = left;
   long rightBound = right;

   long middle = array[(leftBound + rightBound) / 2];
   
   do {
       while(array[leftBound] < middle) {
           leftBound++;
       }
       while(array[rightBound] > middle) {
           rightBound--;
       }

       //Меняем элементы местами
       if (leftBound <= rightBound) {
           std::swap(array[leftBound], array[rightBound]);
           leftBound++;
           rightBound--;
       }
   } while (leftBound <= rightBound);

   //запускаем обе части синхронно
   quickSortWithout(array, left, rightBound);
   quickSortWithout(array, leftBound, right);
}

//Функция, реализующая алгоритм быстрой сортировки(c многопоточностью)
resFuture quickSortWith(std::shared_ptr<int[]> array, long left, long right){
   if(left >= right){
      resFuture res;
      return res;
   }
   long leftBound = left;
   long rightBound = right;

   long middle = array[(leftBound + rightBound) / 2];
   
   do {
      while(array[leftBound] < middle) {
           leftBound++;
       }
       while(array[rightBound] > middle) {
           rightBound--;
       }

       //Меняем элементы местами
       if (leftBound <= rightBound) {
           std::swap(array[leftBound], array[rightBound]);
           leftBound++;
           rightBound--;
       }
   } while (leftBound <= rightBound);
   
   resFuture res;

   if(rightBound - left > 100'000){
       //Добавляем в пул
       res = pool.pushTask(quickSortWith, array, left, rightBound);
       quickSortWith(array, leftBound, right);
   } else {
       // запускаем обе части без многопоточности
       quickSortWithout(array, left, rightBound);
       quickSortWithout(array, leftBound, right);
   }

   return res;
}
