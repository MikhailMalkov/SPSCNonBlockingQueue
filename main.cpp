#include "NonBlockQueue.h"
#include <thread>
#include <iostream>
#include <memory>
int main()
{
    const size_t sizeQueue = 5000;

    TNonBlockQueue<int> queue(sizeQueue);

   std::shared_ptr<std::thread>(
       new std::thread ([&]()
       { 
           for (size_t i = 0; i < sizeQueue; ++i)
              queue.Enqueue(i);
       }), 
       [](std::thread* pthread) 
       {
           if (pthread->joinable()) pthread->join();
       }
   );
    
   std::shared_ptr<std::thread>(
       new std::thread([&]()
           {
               int item{-1};
               for (size_t i = 0; i < sizeQueue; ++i)
               {
                   if(queue.Dequeue(item))
                       std::cout << item << std::endl;
               }
           }),
       [](std::thread* pthread)
       {
            if (pthread->joinable()) pthread->join();
       }
   );
    
   
    return 0;
}
