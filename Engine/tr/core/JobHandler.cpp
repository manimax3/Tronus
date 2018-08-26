#include "JobHandler.h"
#include "Engine.h"
#include "profile/Profiler.h"

#include "concurrentqueue.h"
#include "blockingconcurrentqueue.h"

using namespace tr;

JobHandler::JobHandler()
    : mRunning(false)
    , mThreadPool(std::max((int)std::thread::hardware_concurrency() - 2, 2))
    , mActiveThreads(0)
{
    mQueue = std::make_shared<moodycamel::BlockingConcurrentQueue<TaskPtr>>(10);
    mSyncQueue = std::make_shared<moodycamel::ConcurrentQueue<TaskPtr>>();
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
            TaskPtr task;

            while (mRunning) {
                std::static_pointer_cast<
                    moodycamel::BlockingConcurrentQueue<TaskPtr>>(mQueue)
                    ->wait_dequeue(task);
                if (task) {
                    EASY_BLOCK("Task");
                    mActiveExecutingThreads++;
                    (*task)();
                    task.reset();
                    mActiveExecutingThreads--;
                }
            }
            mActiveThreads--;
        });
    }

    return true;
}

bool JobHandler::Tick()
{
    if (mRunning) {
        TaskPtr task;

        auto sq
            = std::static_pointer_cast<moodycamel::ConcurrentQueue<TaskPtr>>(
                mQueue);

        sq->try_dequeue(task);
        while (task) {
            (*task)();
            task.reset();
            sq->try_dequeue(task);
        }

    }
    return true;
}

void tr::JobHandler::QueueTaskInternal(TaskPtr task)
{
    std::static_pointer_cast<moodycamel::BlockingConcurrentQueue<TaskPtr>>(
        mQueue)
        ->enqueue(std::move(task));
}

void tr::JobHandler::QueueSyncTaskInternal(TaskPtr task)
{
    std::static_pointer_cast<moodycamel::ConcurrentQueue<TaskPtr>>(mQueue)
        ->enqueue(std::move(task));
}

bool JobHandler::Shutdown()
{
    if (!mRunning)
        return true;

    mRunning = false;

    // Make sure that each thread leaves the while loop
    while (mActiveThreads > 0) {
        QueueTask([]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    for (auto &thread : mThreadPool) {
        thread->join();
    }

    return true;
}

