#pragma once

#include <memory>
#include <vector>
#include <string>
#include <cstring>
#include "common.h"

static const int kPrePendIndex = 8; // prependindex长度
static const int kInitalSize = 1024; // 初始化开辟空间长度

class Buffer{
    public:
        DISALLOW_COPY_AND_MOVE(Buffer);

        Buffer();
        ~Buffer();
        // 获得缓冲区起始位置
        char *begin();
        const char *begin() const;
        // 获得可读数据的起始位置
        char *beginread();
        const char *beginread() const;
        // 获得可写数据的起始位置
        char *beginwrite();
        const char *beginwrite() const;

        // 写入数据
        void append(const char *message);
        void append(const char *message, int len);
        void append(const std::string &message);

        // 获得环形缓冲区各区域大小
        int readablebytes() const;
        int writablebytes() const;
        int prependablebytes() const;

        // 可重复读取数据，read_index不变
        char *Peek();
        const char *Peek() const;
        std::string PeekAsString(int len);
        std::string PeekAllAsString();

        // 不可重复读取数据，read_index向后移动
        void Retrieve(int len);
        std::string RetrieveAsString(int len);
        void RetrieveAll();
        std::string RetrieveAllAsString();
        // 不可重复读取数据，直到某个索引之前
        void RetrieveUtil(const char *end);
        std::string RetrieveUtilAsString(const char *end);
        //查看空间
        void EnsureWritableBytes(int len);
    private:
        std::vector<char> buffer_;
        int read_index_;
        int write_index_;
};