// NonBlockQueue.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#ifndef NONBLOCKQUEUE_H
#define NONBLOCKQUEUE_H

#include <atomic>


template<typename T>
class TNonBlockQueue
{
public:
    TNonBlockQueue(size_t size)
        : m_buffer( new Item[size]),
          m_bufferSize(size), 
          m_enqPos{ 0 },
          m_deqPos{ 0 }
    {
        for (size_t i = 0; i < m_bufferSize; ++i)
            m_buffer[i].isEmpty.store(1, std::memory_order_relaxed);
        
        m_isEnqBusy.store(0, std::memory_order_relaxed);
        m_isDeqBusy.store(0, std::memory_order_relaxed);
    }

    ~TNonBlockQueue()
    {
        delete[] m_buffer;
    }

    bool Enqueue(T const& data)
    {
        if (uint64_t isBusy = m_isEnqBusy.load(std::memory_order_acquire);  isBusy == 0) // "mutex" to prevent the code from handling more than one producer 
        {
            if (!m_isEnqBusy.compare_exchange_strong(isBusy, 1, std::memory_order_relaxed))
                return false; // another producer was faster
            
            if (m_enqPos >= m_bufferSize) // moving enqueue cursor to the start position
                m_enqPos = 0;

            Item* pItem = &m_buffer[m_enqPos];
            bool result{ false };
            
            if (pItem)
            {
                if (pItem->isEmpty.load(std::memory_order_acquire) == 1) // "mutex" for isEmpty item marker to synchronize producer and consumer thread
                {
                    pItem->data = data; // put data
                    ++m_enqPos;
                    pItem->isEmpty.store(0, std::memory_order_release); 
                    result = true;
                }
            }
            
            m_isEnqBusy.store(0, std::memory_order_release);
            return result;
        }
        
        return false;
    }

    bool Dequeue(T& data)
    {
        if (uint64_t isBusy = m_isDeqBusy.load(std::memory_order_acquire); isBusy == 0) // "mutex" to prevent the code from handling more than one consumer 
        {
            if (!m_isDeqBusy.compare_exchange_strong(isBusy, 1, std::memory_order_relaxed))
                return false; // another consumer was faster

            if (m_deqPos >= m_bufferSize) // moving dequeue cursor to the start position
                m_deqPos = 0;

            Item* pItem = &m_buffer[m_deqPos];
            bool result{ false };

            if (pItem)
            {
                if (pItem->isEmpty.load(std::memory_order_acquire) == 0) // "mutex" for isEmpty item marker to synchronize producer and consumer thread
                {
                    data = pItem->data; // get data
                    ++m_deqPos; 
                    pItem->isEmpty.store(1, std::memory_order_release); 
                    result = true;
                }
            }

            m_isDeqBusy.store(0, std::memory_order_release);
            return result;
        }
        
        return false;
    }
    
    TNonBlockQueue(TNonBlockQueue const&) = delete;
    void operator = (TNonBlockQueue const&) = delete;
   
private:
    
    struct alignas(64) Item
    {
        std::atomic<uint64_t> isEmpty;
        T data;
    };
    
    Item* const m_buffer;
    uint64_t const m_bufferSize;

    uint64_t m_enqPos;
    uint64_t m_deqPos;
    
    std::atomic<uint64_t> m_isEnqBusy; 
    std::atomic<uint64_t> m_isDeqBusy;
    
};

#endif

