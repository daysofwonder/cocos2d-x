//
//  NodeController.cpp
//
//  Created by Gérald Guyomard on 09/10/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "NodeControllers/NodeController.h"
#include "DOWFoundation/NotificationCenter.h"
#include "CCOperationPool.h"

NS_CC_EXT_BEGIN

using namespace DOW;

NodeControllerPtr
NodeController::createFromResource(const std::string& iResourceName)
{
    NodeControllerPtr controller;
    controller << new NodeController();
    
    controller->_load(iResourceName);
    
    return controller;
}
    
NodeController::NodeController()
{
}
    
NodeController::~NodeController()
{
    NotificationCenter::defaultNotificationCenter().removeObserver(this);
}
    
void
NodeController::release(void)
{
    // When the app is dying: in CCDirector::purgeDirector the CCOperationPool is released (in CCScheduler::unscheduleAll) before the onExit of the scene
    CCOperationPool* pool = CCOperationPool::instance();
    if (pool != nullptr)
    {
        CCOperationPool::instance()->releaseInMainThread(this);
    }
    else
    {
        SuperClass::release();
    }
}

CCObject*
NodeController::owner()
{
    return this;
}


void
NodeController::_postLoad(CCNode* iRootNode)
{
    fRootNode = iRootNode;
    
    _didLoad();
}

void
NodeController::_didLoad()
{        
}
    
void
NodeController::preLoad(Resolvers& ioResolvers)
{
    _preLoad();
    _fillResolvers(ioResolvers);    
}
    
void
NodeController::postLoad(CCNode* iRootNode)
{
    _postLoad(iRootNode);
}

void
NodeController::postLoad()
{
    _postLoad(fRootNode);
}
    
CCScheduler*
NodeController::scheduler() const
{
    return CCDirector::sharedDirector()->getScheduler();
}

void
NodeController::schedule(SEL_SCHEDULE selector, float interval, unsigned int repeat, float delay)
{
    CCAssert( selector, "Argument must be non-nil");
    CCAssert( interval >=0, "Argument must be positive");
    
    // void scheduleSelector(SEL_SCHEDULE pfnSelector, CCObject *pTarget, float fInterval, unsigned int repeat, float delay, bool bPaused);
    scheduler()->scheduleSelector(selector, this, interval, repeat, delay, !fRootNode->isRunning());

}
    
void
NodeController::scheduleOnce(SEL_SCHEDULE selector, float delay)
{
      this->schedule(selector, 0.0f, 0, delay);   
}
    
void
NodeController::unschedule(SEL_SCHEDULE selector)
{
    // explicit nil handling
    if (selector == 0)
        return;
    
    scheduler()->unscheduleSelector(selector, this);
}

NS_CC_EXT_END
