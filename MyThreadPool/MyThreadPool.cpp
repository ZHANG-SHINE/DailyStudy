#include "MyThreadPool.h"
#include <algorithm>
#include <iostream>

constexpr int MIN_POOL_SIZE = 1;

namespace MySpace {
MyThreadPool& MyThreadPool::GetInstance()
{
    static MyThreadPool instance;
    return instance;
}

MyThreadPool::MyThreadPool() {
    status_ = PoolStatus::NOT_INIT;
}

void MyThreadPool::Init(int poolSize) 
{
    std::lock_guard<std::mutex> lock(lock_);
    if (status_ != PoolStatus::NOT_INIT) {
        std::cout << "thread pool is already inited" << std::endl;
        return;
    }
    status_ = PoolStatus::RUNNING;
    poolSize = std::max(poolSize, MIN_POOL_SIZE);
    for (int i = 0; i < poolSize; i++) {
        std::thread th([&] {
            while (status_ == PoolStatus::RUNNING) {
                std::function<void()> task;
                bool hasTask = false;
                {
                    // 加锁对队列进行读写，读写完后释放锁再执行任务
                    std::unique_lock<std::mutex> taskLock(taskLock_);
                    if (taskQueue_.empty()) {
                        taskCv_.wait(taskLock);
                    } else {
                        task = taskQueue_.front();
                        taskQueue_.pop_front();
                        hasTask = true;
                    }
                }
                if (hasTask) {
                    task();
                }
            }
        });
        threadArray_.emplace_back(std::move(th));
    }
}

void MyThreadPool::ShutDown()
{
    std::lock_guard<std::mutex> lock(lock_);
    if (status_ != PoolStatus::RUNNING) {
        std::cout << "thread pool is not running, cannot stop" << std::endl;
        return;
    }
    status_ = PoolStatus::STOPPED;
    std::deque<std::function<void()>> emptyQueue;
    {
        std::unique_lock<std::mutex> taskLock(taskLock_);
        taskQueue_.swap(emptyQueue);
        taskCv_.notify_all();
    }
    for (size_t i = 0; i < threadArray_.size(); i++) {
        threadArray_[i].join();
    }
}

void MyThreadPool::ShutDownNow()
{
    std::lock_guard<std::mutex> lock(lock_);
    if (status_ != PoolStatus::RUNNING) {
        std::cout << "thread pool is not running, cannot stop" << std::endl;
        return;
    }
    status_ = PoolStatus::STOPPED;
    std::deque<std::function<void()>> emptyQueue;
    {
        std::unique_lock<std::mutex> taskLock(taskLock_);
        taskQueue_.swap(emptyQueue);
        taskCv_.notify_all();
    }
    for (size_t i = 0; i < threadArray_.size(); i++) {
        threadArray_[i].detach();
    }
}

MyThreadPool::~MyThreadPool() {
    ShutDown();
}
} // namespace MySpace