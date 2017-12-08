#include "JobHandler.h"
#include "Engine.h"
#include "profile/Profiler.h"

using namespace tr;

JobHandler::JobHandler()
    : mRunning(false)
    , mThreadPool(std::thread::hardware_concurrency())
    , mActiveThreads(0)
{
}

JobHandler::~JobHandler()
{
    Shutdown();
}

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
                if (mQueue.wait_dequeue_timed(func, std::chrono::microseconds(5))) {
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
    for (int i = 0; i < std::thread::hardware_concurrency() + 4; i++)
        AddJob([]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        });

    for (auto &thread : mThreadPool)
        thread->join();

    return true;
}

bool JobHandler::Tick()
{
    return true;
}

//JobHandler::Job::operator bool() const
//{
//	return mStatus;
//}
