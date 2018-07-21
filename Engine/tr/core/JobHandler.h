#pragma once

#include "tr.h"
#include <atomic>
#include <functional>
#include <memory>
#include <thread>
#include <tuple>
#include <vector>

#include "Subsystem.h"
#include "blockingconcurrentqueue.h"

namespace tr {
class JobHandler : public Subsystem<JobHandler> {
public:
    class Job {
    private:
        std::function<void()> mTask;
        // std::atomic_bool mStatus;

    public:
        template<typename Func>
        Job(Func &&func);

        Job() = default;

        Job(Job &&)  = default;
        Job &operator=(Job &&) = default;

        Job(const Job &) = delete;
        Job &operator=(const Job &) = delete;

        // operator bool() const;
        inline void operator()() const { mTask(); }

        // inline void SetStatus(const bool status) { mStatus = status; }
    };

private:
    using ThreadPtr = std::unique_ptr<std::thread>;

    std::vector<ThreadPtr>                   mThreadPool;
    moodycamel::BlockingConcurrentQueue<Job> mQueue;

    std::atomic_bool   mRunning;
    std::atomic_int8_t mActiveThreads;

public:
    JobHandler();
    ~JobHandler();

    template<typename Func>
    bool AddJob(Func &&function, bool bypass = false);

    bool               Initialize(Engine *engine) override;
    bool               Shutdown() override;
    bool               Tick() override;
    inline std::string GetName() const override { return "JobHandler"; }
};

template<typename Func>
bool JobHandler::AddJob(Func &&function, bool bypass)
{

    if (!mRunning && !bypass)
        return false;

    Job  job(std::forward<Func>(function));
    bool valid = mQueue.enqueue(std::move(job));

    return valid;
}

template<typename Func>
JobHandler::Job::Job(Func &&func)
    : mTask(std::forward<Func>(func)) /*, mStatus(false)*/
{
}
}
