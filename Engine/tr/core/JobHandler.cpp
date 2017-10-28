#include "JobHandler.h"

using namespace tr;

JobHandler::JobHandler()
    : mRunning(false), mThreadPool(std::thread::hardware_concurrency()), mActiveThreads(0)
{
}

JobHandler::~JobHandler()
{
    Stop();
}

void JobHandler::Start()
{
    mRunning = true;

    for(auto &ptr : mThreadPool)
    {
        ptr = std::make_unique<std::thread>([&](){

			mActiveThreads++;
            Job func;

            while(mRunning)
                if(mQueue.try_dequeue(func))
					func();

			mActiveThreads--;
        });
    }
}

void JobHandler::Stop()
{
	if(!mRunning)
		return;

    mRunning = false;
	for(auto& thread : mThreadPool)
		thread->join();
}

//JobHandler::Job::operator bool() const
//{
//	return mStatus;
//}