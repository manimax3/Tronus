#pragma once

#include "tr.h"
#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <tuple>
#include <vector>

#include "Subsystem.h"

namespace tr {
/**
 * Class used for scheduling tasks
 */
class JobHandler : public Subsystem<JobHandler> {
public:
    JobHandler();
    ~JobHandler();
    bool Initialize(Engine *engine) override;
    bool Shutdown() override;
    bool Tick() override;

    inline std::string GetName() const override { return "JobHandler"; }

    /**
     * Queues a task
     */
    template<typename T>
    std::future<void> QueueTask(T &&t)
    {
        auto task = std::make_shared<Task>(std::forward<T>(t));
        auto fut  = task->get_future();
        QueueTaskInternal(std::move(task));
        return std::move(fut);
    }

    template<typename T>
    void QueueSyncTask(T &&t)
    {
        auto task = std::make_shared<Task>(std::forward<T>(t));
        QueueSyncTaskInternal(std::move(task));
    }

private:
    using Task      = std::packaged_task<void()>;
    using TaskPtr   = std::shared_ptr<Task>;
    using ThreadPtr = std::unique_ptr<std::thread>;

    void QueueTaskInternal(TaskPtr task);
    void QueueSyncTaskInternal(TaskPtr task);

    std::vector<ThreadPtr> mThreadPool;
    std::shared_ptr<void>  mQueue;
    std::shared_ptr<void>  mSyncQueue;

    std::atomic_bool   mRunning;
    std::atomic_int8_t mActiveThreads;
    std::atomic_int8_t mActiveExecutingThreads;
};
}
