#pragma once

#include <iostream>
#include <pthead.h>
#include <stdlib.h>
#include <queue>
#include <memory>
#include "Locker.hpp"

// 阻塞队列
// 运用生产者和消费者模型
template <class T>
class BlockQueue
{
public:
    // 初始化阻塞队列
    BlockQueue()
    {
        queue_ptr_ = std::make_unique<std::queue<T>>();
    }
    ~BlockQueue() = default;

    void clear()
    {
        mutex_.lock();
        queue_ptr


        mutex_.unlock();
    }

private:
    Locker mutex_;
    Cond cond_;

    // 维护一个队列
    std::unique_ptr<std::queue<T>> queue_ptr_;
};