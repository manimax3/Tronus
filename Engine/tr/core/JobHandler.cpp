#include "JobHandler.h"
#include "Engine.h"

using namespace tr;

JobHandler::JobHandler()
    : mRunning(false), mThreadPool(std::thread::hardware_concurrency()), mActiveThreads(0)
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
    
    return true;
}

bool JobHandler::Shutdown()
{
	if(!mRunning)
		return true;

    mRunning = false;
	for(auto& thread : mThreadPool)
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