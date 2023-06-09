#pragma once

#include <iostream>
#include <pthead.h>
#include <stdlib.h>
#include <queue>
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
    }

private:
    Locker mutex_;
    Cond cond_;
};