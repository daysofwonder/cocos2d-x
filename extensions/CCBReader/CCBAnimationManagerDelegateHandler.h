//
//  CCBAnimationManagerDelegateHandler.h
//
//  Created by Gérald Guyomard on 20/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef CCBAnimationManagerDelegateHandler_h
#define CCBAnimationManagerDelegateHandler_h

#include "cocoa/CCObjectPtr.h"
#include "CCBAnimationManager.h"
#include <functional>
#include <string>
#include <memory>

NS_CC_EXT_BEGIN

class CCBAnimationManagerDelegateHandler : protected CCBAnimationManagerDelegate
{
public:
    CCBAnimationManagerDelegateHandler();
    ~CCBAnimationManagerDelegateHandler();
    
    typedef std::function<void(const char* iAnimationName)> TCompletion;
    
    void install(CCBAnimationManager* iManager, const TCompletion& iCompletion);
    void uninstall();
    
private:    
    virtual void completedAnimationSequenceNamed(const char *name) override;
    
    CCObjectPtr<CCBAnimationManager>    fManager;
    TCompletion                         fCompletion;
};

NS_CC_EXT_END

#endif // CCBAnimationManagerDelegateHandler_h
