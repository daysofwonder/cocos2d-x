//
//  CCBAnimationManagerDelegateHandler.cpp
//
//  Created by Gérald Guyomard on 20/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCBAnimationManagerDelegateHandler.h"

NS_CC_EXT_BEGIN

    CCBAnimationManagerDelegateHandler::CCBAnimationManagerDelegateHandler()
    {}
    
    CCBAnimationManagerDelegateHandler::~CCBAnimationManagerDelegateHandler()
    {
        uninstall();
    }
    
    void
    CCBAnimationManagerDelegateHandler::install(CCBAnimationManager* iManager, const TCompletion& iCompletion)
    {
        fCompletion = iCompletion;
        fManager = iManager;
        
        assert(fManager != nullptr);
        assert(fCompletion != nullptr);
        
        fManager->setDelegate(this);
    }
    
    void
    CCBAnimationManagerDelegateHandler::uninstall()
    {
        if (fManager != nullptr)
        {
            fManager->setDelegate(nullptr);
            fManager = nullptr;
            fCompletion = nullptr;
        }
    }
    
    void
    CCBAnimationManagerDelegateHandler::completedAnimationSequenceNamed(const char* iAnimationName)
    {
        assert(fManager != nullptr);
        assert(fCompletion != nullptr);
            
        fCompletion(iAnimationName);
    }

NS_CC_EXT_END

