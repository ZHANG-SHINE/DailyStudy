/**
 * @file MyThreadPool.h
 * @author zhang shine
 * @brief a simple thread pool.
 * @version 0.1
 * @date 2022-06-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef MY_THREAD_POOL_H
#define MY_THREAD_POOL_H

#include "FunctionTraits.h"
#include <future>
#include <functional>
#include <memory>
#include <thread>
#include <vector>
#include <deque>

namespace MySpace {
enum PoolStatus {
    NOT_INIT = 0,
    RUNNING,
    STOPPED
};


/**
 * @brief 一个简单的线程池，包括的功能有：设置线程数量，添加任务，关闭线程池
 * 
 */
class MyThreadPool {
public:
    /**
     * @brief Get the Instance object
     * 
     * @return MyThreadPool& 
     */
    static MyThreadPool& GetInstance();

    /**
     * @brief init thread pool
     * 
     * @param poolSize set the thread numbers, default is 4.
     */
    void Init(int poolSize = 4);

    /**
     * @brief put a task into thread pool.
     * 
     * @param func lambda expression
     * @param arg args
     * @return std::shared_ptr<std::future<R>> a future type pointer
     */
    template <class F, class... Args, typename R = typename std::decay<typename function_traits<F>::return_type >::type>
    std::shared_ptr<std::future<R>> AddTask(F&& func, Args&&... arg);

    void ShutDown();

    void ShutDownNow();

    ~MyThreadPool();

    MyThreadPool(const MyThreadPool&) = delete;

    MyThreadPool& operator=(const MyThreadPool&) = delete;

private:
    MyThreadPool();

private:
    PoolStatus status_;
    std::mutex lock_;
    std::mutex taskLock_;
    std::vector<std::thread> threadArray_;
    std::deque<std::function<void()>> taskQueue_;
    std::condition_variable taskCv_;
};

// 实现添加任务的函数，泛型需要实现在头文件里
template <class F, class... Args, typename R>
std::shared_ptr<std::future<R>> MyThreadPool::AddTask(F&& func, Args&&... args)
{
    if (status_ != PoolStatus::RUNNING) {
        return nullptr;
    }
    auto task = std::make_shared<std::packaged_task<R()>>(
        std::bind(std::forward<F>(func), std::ref(std::forward<Args>(args))...)
        );
    auto result = std::make_shared<std::future<R>>(task->get_future());
    {
        // 新任务入队，并唤醒一个线程来干活
        std::lock_guard<std::mutex> lock(taskLock_);
        taskQueue_.emplace_back([task] {
            (*task)();
        });
        taskCv_.notify_one();
    }
    return result;
}
} // namespace MySpace
#endif