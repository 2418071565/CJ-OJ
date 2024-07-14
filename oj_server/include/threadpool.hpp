#pragma once
#include "conf.hpp"
#include <vector>
#include <thread>
#include <memory>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <future>
#include <atomic>
#include <queue>

CJOJ_BEGIN

// c++ 17 线程池
class threadpool
{
    std::vector<std::thread> workers;
    std::deque<std::function<void()>> tasks;
    std::atomic<bool> is_running, is_over;
    int max_task;

    // 同步
    std::condition_variable p_cv // 生成者条件变量
        , c_cv; // 消费者条件变量
    std::mutex  p_mut  // 生产者锁
        , c_mut; // 消费者锁

public:

    threadpool(int max = 20,int threads = 5)
        : workers(threads)
        , is_running(false)
        , is_over(false)
        , max_task(max)
    {
        for (int i = 0; i < threads; ++i) {
            workers[i] = std::thread([this, threads]() {
                std::function<void()> task;
                for (;;)
                {
                    {
                    std::unique_lock<std::mutex> ul(c_mut);
                    this->c_cv.wait(ul,
                        [this]() {return this->is_over or !this->tasks.empty(); });
                    if (this->is_over and this->tasks.empty())    // 线程池结束退出线程
                        return;
                    while (!is_running)std::this_thread::yield();   // 等待暂停结束

                    task = std::move(this->tasks.back());
                    this->tasks.pop_back();
                    }
                    task();
                }
            });
        }
    }

    template<class Callable, class... Args>
    auto add_task(Callable&& F, Args&&... args)
        -> decltype(std::future<typename std::invoke_result<Callable, Args...>::type>())
    {
        using ret_type = typename std::invoke_result<Callable, Args...>::type;

        auto task = std::make_shared<std::packaged_task<ret_type()>>(
            std::bind(std::forward<Callable>(F),std::forward<Args>(args)...)
        );


        auto ret = task->get_future();
        {
            std::unique_lock<std::mutex> ul(p_mut);
            p_cv.wait(ul,
                [this]() {return this->is_over or !(this->max_task == this->tasks.size()); }
            );
            if (is_over)return std::future<int>();

            tasks.push_back([task]() {(*task)(); });
        }

        return ret;
    }

    void start() noexcept   // 启动线程池
    { is_running = true;}

    void stop() noexcept    // 暂停线程池
    { is_running = false; }

    ~threadpool()
    {
        is_over = true;
        is_running = true;
        c_cv.notify_all();
        p_cv.notify_all();
        for (auto& td : workers)
            td.join();
    }
};

CJOJ_END