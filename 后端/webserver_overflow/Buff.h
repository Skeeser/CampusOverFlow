#pragma once

#include <iostream>
#include <vector>
#include <atomic>
#include <unistd.h>
#include <sys/uio.h>

// 缓冲区实现
class Buff
{
public:
    BUff(int init_buff_size = 1024);
    ~Buff() = default;

    void append()

        // io读写接口
        ssize_t readFd(int fd, int *Errno);
    ssize_t writeFd(int fd, int *Errno);

private:
    std::vector<char> buff_;
    std::atomic<size_t> *read_pos_;
    std::atomic<size_t> *write_pos_;
};