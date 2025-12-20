#pragma once

#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>


// 异步写日志队列
template<typename T>
class LockQueue {
    public:
        // 多线程写
        void push(const T &data) {
            std::lock_guard<std::mutex> lck(mtx_);
            que_.push(data);
            cond_.notify_one();
        }

        // 单线程读日志
        T& pop() {
            std::unique_lock<std::mutex> lck(mtx_);
            while(que_.empty()) {
                // 日志队列为空，等待数据
                cond_.wait(lck);
            }
            T data = que_.front();
            que_.pop();
            return data;
        }
    private:
        std::queue<T> que_;
        std::mutex mtx_;
        std::condition_variable cond_;
};