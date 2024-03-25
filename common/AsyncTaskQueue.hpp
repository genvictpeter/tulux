/*
 *  Copyright (c) 2018, 2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       AsyncTaskQueue.hpp
 * @brief      Implements a queue that will hold on futures for async tasks. This allows async
 *             tasks to be created from within class methods and prevents the task from
 *             blocking at the end of a function a class  method. This is necessary if the future
 *             is not returned. For instance, if in a function, in the following function foo:
 *
 *
 *                 void foo() {
 *                     std::async(std::launch::async, [](){ bar(); }
 *                 }
 *
 *
 *             ... bar will be executed in a background thread. However, since future is not
 *             returned to the caller of foo, the destructor for the async task will be invoked
 *             at the end of foo. This means that foo will block until bar completes. This is
 *             not what we want.
 *
 *             Instead, we want bar to run in a background thread and foo to return immediately.
 *             One way we can do this is by returning a future to the caller of foo:
 *
 *
 *                 std::future<void> foo() {
 *                     return std::async(std::launch::async, [](){ bar(); }
 *                 }
 *
 *
 *             However, in some cases, the caller doesn't care about the result of bar. Hence,
 *             it is clunky to return the future to the caller. In such a case, we need to hold
 *             a reference to the future somewhere else. This is the purpose of this
 *             AsyncTaskQueue.
 *
 *             Using AsyncTaskQueue, we can achieve the functionality like so:
 *
 *
 *                 AsyncTaskQueue taskQ;
 *
 *                  ...
 *
 *                 void foo() {
 *                     auto f = std::async(std::launch::async, [](){ bar(); }
 *                     taskQ.add(f);
 *                 }
 *
 */

#ifndef ASYNCTASKQUEUE_HPP
#define ASYNCTASKQUEUE_HPP

#include <condition_variable>
#include <deque>
#include <future>
#include <memory>
#include <mutex>

#include "common/Logger.hpp"

namespace telux {
namespace common {

template <typename T>
class AsyncTaskQueue {
 public:
    AsyncTaskQueue()
       : orderedTaskThread_(nullptr)
       , shuttingDown_(false) {
    }

    ~AsyncTaskQueue() {
        // Indicate we are shutting down, notify to release the wait and join before winding up
        shuttingDown_ = true;
        orderedTasksCv_.notify_all();
        if (orderedTaskThread_ != nullptr) {
            orderedTaskThread_->join();
        }
    }
    /**
     * This function performs two functions - it first purges completed tasks from the front
     * of the queue, then it adds the new task to the end of the queue. We do this to simplify
     * its use. Clients are not required to call purgeCompleted themselves.
     *
     * @param [in] f - future associated with an async task
     */
    void add(std::shared_future<void> &f) {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

        // Identify if the future is a deferred future
        if (f.wait_until(now) == std::future_status::deferred) {

            // If it is a deferred task, we add it to the ordered task queue and notify the executor
            // thread for it to handle the task
            std::lock_guard<std::mutex> lock(orderedTasksMutex_);
            orderedTasksQueue_.push_back(f);
            // If the thread does not exit, create one.
            if (orderedTaskThread_ == nullptr) {
                orderedTaskThread_
                    = std::make_shared<std::thread>(&AsyncTaskQueue::executeTask, this);
            }
            orderedTasksCv_.notify_one();
        } else {
            // Handling for an async task. Just add the future for persistence until it is purged
            // at a later point in time
            std::lock_guard<std::mutex> lock(tasksMutex_);
            purgeCompleted();
            tasksQueue_.push_back(f);
        }
    }

    void executeTask() {
        std::shared_future<T> f;
        while (true) {
            {
                std::unique_lock<std::mutex> lock(orderedTasksMutex_);
                orderedTasksCv_.wait(
                    lock, [this] { return (orderedTasksQueue_.size() > 0) || shuttingDown_; });
                if (shuttingDown_) {
                    break;
                }
                f = orderedTasksQueue_.front();
                orderedTasksQueue_.pop_front();
            }
            f.get();
        }
    }

 protected:
    /**
     * Removes completed tasks from the front of the task queue. For performance reasons, it
     * will not remove any completed tasks if there exits an uncompleted task ahead of it in
     * the task queue
     */
    void purgeCompleted() {

        LOG(DEBUG, "AsyncTask::", __FUNCTION__, " queue len is ", tasksQueue_.size());

        // Set timeout time to now so that we timeout immediately. Unfortunately,
        // futures don't have any methods to immediately find out if it's ready.
        // We always have to supply some timeout.
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        auto itr = std::begin(tasksQueue_);

        // Iterate from head of queue and remove if the task is complete
        while (itr != std::end(tasksQueue_)) {
            bool doRemove = false;
            if (itr->valid()) {
                // If the task has already completed, we can remove it.
                if (std::future_status::ready == itr->wait_until(now)) {
                    LOG(DEBUG, "  task is ready to remove");
                    doRemove = true;
                } else {
                    LOG(DEBUG, "  task is not ready...");
                }
            } else {
                // If the task is invalid, we'll just assume it's also complete
                // and remove it as well.
                doRemove = true;
            }

            // Remove the task if complete. If not, we'll just exit the loop.
            // Of course, we can just keep on looping and find all tasks that have
            // completed, but then we would be deleting from the middle of a queue
            // and we don't want to do that because it's a more expensive operation
            // than removing from the beginning of the queue.
            if (doRemove) {
                // LOG(DEBUG, "  removing task.");
                itr = tasksQueue_.erase(itr);
            } else {
                break;
            }
        }
    }

    std::mutex tasksMutex_;                         // mutex protecting unordered, async queue
    std::deque<std::shared_future<T>> tasksQueue_;  // queue of futures for async tasks

    std::shared_ptr<std::thread> orderedTaskThread_;  // Thread to execute ordered task
    std::mutex orderedTasksMutex_;                    // mutex protecting ordered, deferred queue
    std::condition_variable orderedTasksCv_;  // Condition variable used for waking up the worker
                                              // thread
    std::deque<std::shared_future<T>> orderedTasksQueue_;  // queue of futures for deferred tasks
    bool shuttingDown_;  // Flag indicating we are about to shutdown
};

}  // namespace common
}  // namespace telux

#endif  // #ifndef ASYNCTASKQUEUE_HPP
