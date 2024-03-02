#pragma once

#include <pthread.h>
#include <semaphore.h>
#include "MyException.hpp"

// 锁机制的实现
// 信号量管理,仅限同一进程
class Sem
{
public:
    // 初始化为0
    Sem()
    {
        sem_init(&sem_, 0, 0);
    }
    // 初始化为某个值
    Sem(int sem_init_num)
    {
        sem_init(&sem_, 0, sem_init_num);
    }
    ~Sem()
    {
        sem_destroy(&sem_);
    };

    // p操作
    semWait()
    {
        sem_wait(&sem_);
    }

    // v操作
    semPost()
    {
        sem_post(&sem_);
    }

private:
    // 维护类内的信号量
    sem_t sem_;
};

// 多线程锁
class Locker
{
public:
    Locker()
    {
        // 初始化
        if (pthread_mutex_init(&mutex_, NULL) != 0)
        {
            throw MyException("多线程锁初始化失败");
        }
    }
    ~Locker()
    {
        pthread_mutex_destroy(&mutex_);
    };

    bool lock()
    {
        return pthread_mutex_lock(&mutex_) == 0;
    }

    bool unlock()
    {
        return !pthread_mutex_unlock(&mutex_) == 0;
    }

    // 暴露出去，给条件变量等用
    pthread_mutex_t *get()
    {
        return &mutex_;
    }

private:
    pthread_mutex_t mutex_;
};

// 条件变量
class Cond
{
public:
    Cond()
    {
        if (pthread_cond_init(&cond, NULL))
        {
            throw MyException("条件变量初始化错误");
        }
    }

    ~Cond()
    {
        pthread_cond_destroy(&cond_);
    };

    // 等待条件变量
    bool condWait(pthread_mutex_t *mutex)
    {
        return pthread_cond_wait(&cond_, mutex) == 0;
    }

    // 定时等待
    bool condTimeWait(pthread_mutex_t *mutex, struct timespec time)
    {
        return pthread_cond_timedwait(&cond_, mutex, &time) == 0;
    }

    // 发送信号
    bool signal()
    {
        return pthread_cond_signal(&cond);
    }

    // 将信号广播
    bool signalBroadCast()
    {
        return pthread_cond_broadcast(&cond_);
    }

private:
    // 条件变量
    pthread_cond_t cond_;
};
