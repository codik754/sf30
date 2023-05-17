//Файл main.cpp
#include <iostream>
#include "quicksort.h"
#include <chrono>
#include <memory>

int main(){
   //Длина массива
   long n = 10'000'000;

   //Создаем первый массив
   std::shared_ptr<int []> array1 = std::shared_ptr<int[]>(new int[n], std::default_delete<int[]>());
   for(long i = 0; i < n; ++i){
      array1[i] = rand() % 50000 + 1;   
   }
   
   //Создаем второй массив и копируем в него значения из первого
   std::shared_ptr<int []> array2 = std::shared_ptr<int[]>(new int[n], std::default_delete<int[]>());
   for(long i = 0; i < n; ++i){
      array2[i] = array1[i];
   }
   
   //Определяем начало выполнения
   auto start = std::chrono::system_clock::now();

   //Сортируем первый массив(без многопоточности)
   quickSortWithout(array1, 0, n - 1);

   //Определяем конец времени выполнения сортировки
   auto end = std::chrono::system_clock::now();


   //Высчитываем разницу
   std::chrono::duration<double> diff = end - start;

   //Выводим информацию
   std::cout << "Size: " << n << std::endl;
   
   //Проверяем правильность сортировки
   bool bit = true;
   for(long i = 0; i < n - 1; ++i){
      if(array1[i] > array1[i + 1]){
         bit = false;
         break;
      }
   }  
   
   if(bit){
      std::cout << "Сorrect sorting!" << std::endl;
   }
   else{
      std::cout << "Inсorrect sorting!" << std::endl;
   }

   std::cout << "Time: " << diff.count() << std::endl << std::endl;

   //Определяем начало выполнения
   start = std::chrono::system_clock::now();

   //Сортируем второй массив(с многопоточностью)
   resFuture res = quickSortWith(array2, 0, n - 1);
  
   res.wait();

   pool.waitFutures();
   

   //Определяем конец времени выполнения сортировки
   end = std::chrono::system_clock::now();
   
   //Высчитываем разницу
   diff = end - start;
   
   //Проверяем правильность сортировки
   bit = true;
   for(long i = 0; i < n - 1; ++i){
      if(array2[i] > array2[i + 1]){
         bit = false;
         break;
      }
   }  
   
   if(bit){
      std::cout << "Сorrect sorting!" << std::endl;
   }
   else{
      std::cout << "Inсorrect sorting!" << std::endl;
   }

   std::cout << "Time: " << diff.count() << std::endl;
   

   return 0;
}

