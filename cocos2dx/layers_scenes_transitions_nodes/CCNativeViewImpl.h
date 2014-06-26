//
//  CCNativeViewImpl.h
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCNativeViewImpl__
#define __CCNativeViewImpl__

#include "platform/CCPlatformMacros.h"

NS_CC_BEGIN

    class CCNode;

    class CCNativeViewImpl
    {
    public:
        virtual ~CCNativeViewImpl() {}

        virtual bool init(cocos2d::CCNode* iNode) = 0;
        virtual cocos2d::CCNode* owner() const = 0;

        virtual void onEnter() = 0;
        virtual void onEnterTransitionDidFinish() = 0;

        virtual void onExit(void) = 0;
        virtual void onExitTransitionDidStart() = 0;
    };

NS_CC_END

#endif /* defined(__CCNativeViewImpl__) */
