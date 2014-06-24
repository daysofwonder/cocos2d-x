//
//  CCOperationPool.cpp
//
//  Created by Gérald Guyomard on 13/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCOperationPool.h"
#include "cocoa/CCObjectPtr.h"
#include <thread>
#include "CCDirector.h"
#include "CCScheduler.h"

NS_CC_BEGIN

	CCOperationPool* CCOperationPool::s_Instance = nullptr;

    CCOperationPool::CCOperationPool()
    {
    }

    CCOperationPool::~CCOperationPool()
    {
        assert(s_Instance == this);
        s_Instance = nullptr;
    }

    static std::thread::id s_MainThreadID;

    void
    CCOperationPool::installInstance()
    {
        assert(s_Instance == nullptr);

        s_MainThreadID = std::this_thread::get_id();

        CCObjectPtr<CCOperationPool> instance;
        instance << new CCOperationPool;

        CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(instance, 0, false);

        assert(instance->retainCount() > 1); // Should be retain by CCObjectPtr + scheduler

        s_Instance = instance;
    }

    bool
    CCOperationPool::isInMainThread()
    {
        return std::this_thread::get_id() == s_MainThreadID;
    }

    void
    CCOperationPool::executeInMainThread(const TOperation& iOperation)
    {
        if (iOperation != nullptr)
        {
            if (isInMainThread())
            {
                iOperation();
            }
            else
            {
                std::lock_guard<std::mutex> lock(fPendingOperationsMutex);

                fPendingOperations.push(iOperation);
            }
        }
    }

	void
	CCOperationPool::releaseInMainThread(CCObject* iObject)
	{
		if (iObject != nullptr)
		{
			if ((iObject->retainCount() > 1) || isInMainThread())
			{
				// Not near to death, safe to release here
				iObject->release();
			}
			else
			{
				std::lock_guard<std::mutex> lock(fPendingOperationsMutex);
				fPendingObjectsToRelease.push(iObject);
			}
		}
	}

    void
    CCOperationPool::executeInMainThreadAtNextFrame(const TOperation& iOperation)
    {
        if (iOperation != nullptr)
        {
            std::lock_guard<std::mutex> lock(fPendingOperationsMutex);
            fPendingOperations.push(iOperation);
        }
    }
    void
    CCOperationPool::update(float)
    {
        assert(isInMainThread());

        // Make sure there is at a scene running
        // may be null at launch of app
        if (CCDirector::sharedDirector()->getRunningScene() != nullptr)
        {
            // Make a copy before executing all operations
            std::vector<TOperation> operations;
			std::vector<CCObject*> objects;
            {
                std::lock_guard<std::mutex> lock(fPendingOperationsMutex);
                while (!fPendingOperations.empty())
                {
                    const TOperation& op = fPendingOperations.front();
                    operations.push_back(op);

                    fPendingOperations.pop();
                }

				while (!fPendingObjectsToRelease.empty())
				{
					CCObject* object = fPendingObjectsToRelease.front();
					objects.push_back(object);

					fPendingObjectsToRelease.pop();
				}
            }

            for (const auto& op : operations)
            {
                op();
            }

			for (auto* object : objects)
			{
				object->release();
			}
        }
    }

NS_CC_END

