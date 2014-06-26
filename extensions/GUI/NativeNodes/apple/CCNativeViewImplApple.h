//
//  CCNativeViewImplApple.h
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCNativeViewImplApple__
#define __CCNativeViewImplApple__

#include "layers_scenes_transitions_nodes/CCNativeViewImpl.h"
#include "cocoa/CCObjectPtr.h"
#include "ExtensionMacros.h"

#ifdef CC_TARGET_OS_MAC
@class NSView;
typedef  NSView AppleView;
//typedef NSTextView AppleTextView;
//typedef WebView AppleWebView;
#else
@class UIView;
typedef  UIView AppleView;
//typedef UITextView AppleTextView;
//typedef UIWebView AppleWebView;
#endif

NS_CC_EXT_BEGIN

    class CCNativeViewImplApple : public CCNativeViewImpl
    {
    public:
        CCNativeViewImplApple();
        virtual ~CCNativeViewImplApple();
        
        virtual bool init(CCNode* iNode) override;
        
        virtual CCNode* owner() const override { return fOwner; }

        virtual void onEnter() override;
        virtual void onEnterTransitionDidFinish() override;
        
        virtual void onExit(void) override;
        virtual void onExitTransitionDidStart() override;

        AppleView* uiView() const { return fUIView; }
        
        float frameBufferScale() const;
        
    protected:
        virtual AppleView* _createUIView() = 0;
        virtual void _updateNativeUIView();

        virtual void _onEnter();
        virtual void _onExit();

    private:
        CCNode*             fOwner;
        AppleView*             fUIView;
        bool                fIsDiscarded;
        
        class Updater;
        CCObjectPtr<Updater> fUpdater;
    };

NS_CC_EXT_END

#endif 
