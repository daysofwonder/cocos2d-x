//
//  CCOperationPool.h
//
//  Created by Gérald Guyomard on 13/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCOperationPool__
#define __CCOperationPool__

#include "cocos2d.h"

#include <functional>
#include <queue>
#include <mutex>

NS_CC_BEGIN

    class CCOperationPool : public CCObject
    {
    public:
        
        static void installInstance(); // Should be called in main thread!
        
		static CCOperationPool* instance() { return s_Instance;  }
        
        static bool isInMainThread();
        
        typedef std::function<void()> TOperation;
        void executeInMainThread(const TOperation& iOperation);
        void executeInMainThreadAtNextFrame(const TOperation& iOperation);
        
		void releaseInMainThread(CCObject* iObject);

    private:
        CCOperationPool();
        virtual ~CCOperationPool();
        
        virtual void update(float) override;
        
        std::mutex fPendingOperationsMutex;
        std::queue<TOperation> fPendingOperations;
		std::queue<CCObject*> fPendingObjectsToRelease;

		static CCOperationPool* s_Instance;
    };

NS_CC_END

#endif /* defined(__CCOperationPool__) */
