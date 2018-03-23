#include "JobHandler.h"
#include "Engine.h"
#include "profile/Profiler.h"

using namespace tr;

JobHandler::JobHandler()
    : mRunning(false)
    , mThreadPool(std::max((int)std::thread::hardware_concurrency() - 2, 2))
    , mActiveThreads(0)
{
}

JobHandler::~JobHandler() { Shutdown(); }

bool JobHandler::Initialize(Engine *engine)
{
    Subsystem::Initialize(engine);

    mRunning = true;

    for (auto &ptr : mThreadPool) {
        ptr = std::make_unique<std::thread>([&]() {

            EASY_THREAD_SCOPE("Worker");

            mActiveThreads++;
            Job func;

            while (mRunning) {
                mQueue.wait_dequeue(func);

                {
                    EASY_BLOCK("Task");
                    func();
                }
            }

            mActiveThreads--;
        });
    }

    return true;
}

bool JobHandler::Shutdown()
{
    if (!mRunning)
        return true;

    mRunning = false;

    // Make sure that each thread leaves the while loop
    while (mActiveThreads > 0) {
        for (int i = 0; i < mThreadPool.size(); i++)
            AddJob(
                []() {
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                },
                true);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    for (auto &thread : mThreadPool) {
        thread->join();
    }

    return true;
}

bool JobHandler::Tick() { return true; }

// JobHandler::Job::operator bool() const
//{
//	return mStatus;
//}
