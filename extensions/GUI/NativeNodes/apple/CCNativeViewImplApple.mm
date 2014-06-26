//
//  CCNativeViewImplIOS.cpp
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCNativeViewImplApple.h"
#include "../DOWFoundationImpl/Apple/NSString+STLExtensions.h"

#import "EAGLView.h"
#include "DOWFoundation/NotificationCenter.h"
#include "Cocos2DNativeView.h"

#include "CCEGLView.h"

#ifdef CC_TARGET_OS_MAC
    #include <WebKit/WebKit.h>
    #include <AppKit/AppKit.h>
#else
    #include <UIKit/UIKit.h>
#endif

#include "CCDirector.h"
#include "CCScheduler.h"

NS_CC_EXT_BEGIN

using namespace DOW;

    class CCNativeViewImplApple::Updater : public CCObject
    {
    public:
        Updater(CCNativeViewImplApple* iImpl);
        virtual ~Updater();
        
        virtual void update(float iTime) override;
        
        void listen();
        void stopListening();
        
        void terminate();
        
    private:
        CCNativeViewImplApple* fImpl;
    };
    
    CCNativeViewImplApple::Updater::Updater(CCNativeViewImplApple* iImpl)
    : fImpl(iImpl)
    {}
    
    CCNativeViewImplApple::Updater::~Updater()
    {
        stopListening();
    }
    
    void
    CCNativeViewImplApple::Updater::update(float iTime)
    {
        if (fImpl != nullptr)
        {
            fImpl->_updateNativeUIView();            
        }
    }

    void
    CCNativeViewImplApple::Updater::listen()
    {
        CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(this, -10, false);
    }
    
    void
    CCNativeViewImplApple::Updater::stopListening()
    {
        CCDirector::sharedDirector()->getScheduler()->unscheduleUpdateForTarget(this);
    }

    void
    CCNativeViewImplApple::Updater::terminate()
    {
        stopListening();
        fImpl = nullptr;
    }

    CCNativeViewImplApple::CCNativeViewImplApple()
    : fUIView(nil), fIsDiscarded(false)
    {}
    
    CCNativeViewImplApple::~CCNativeViewImplApple()
    {
        NotificationCenter::defaultNotificationCenter().removeObserver(this);
                
        [fUIView removeFromSuperview];
        [fUIView release];
        
        if (fUpdater != nullptr)
        {
            fUpdater->terminate();
        }
    }
    
    bool
    CCNativeViewImplApple::init(CCNode* iNode)
    {
        fOwner = iNode;
        
        fUIView = _createUIView();
        if (fUIView == nil)
        {
            return false;
        }
        
        #ifdef CC_TARGET_OS_MAC
            [fUIView setWantsLayer: YES];
        #endif
        
        NotificationCenter& center = NotificationCenter::defaultNotificationCenter();
        
        center.addObserver(this, kShouldDiscardNonDescendantNativeNodesNotifID, [this](void* iObserver, const Notification& iNotification)
                           {
                               if (!fIsDiscarded)
                               {
                                   CCNode* node = (CCNode*) iNotification.object();
                                   if (!node->isAncestor(fOwner))
                                   {
                                       fIsDiscarded = true;
                                       _updateNativeUIView();
                                   }
                               }
                           });
        
        center.addObserver(this, kShouldRestoreNonDescendantNativeNodesNotifID, [this](void* iObserver, const Notification& iNotification)
                           {
                               if (fIsDiscarded)
                               {
                                   CCNode* node = (CCNode*) iNotification.object();
                                   if (!node->isAncestor(fOwner))
                                   {
                                       fIsDiscarded = false;
                                       _updateNativeUIView();
                                   }
                               }
                           });

        return true;
    }
    
    float
    CCNativeViewImplApple::frameBufferScale() const
    {
#if defined(CC_TARGET_OS_MAC)
        return [[EAGLView sharedEGLView] frameBufferScale];
#else
        return 1;
#endif
    }
    
    void
    CCNativeViewImplApple::_updateNativeUIView()
    {
        if (fOwner->isRunning() && !fIsDiscarded)
        {
            BOOL hidden = NO;
            
            float rotation = 0;
            
            CCNode* n = fOwner;
            while (n != nullptr)
            {
                if (!n->isVisible())
                {
                    // not visible on screen
                    hidden = YES;
                    break;
                }
                
                rotation += n->getRotation();
                
                n = n->getParent();
            }
            
            [fUIView setHidden:hidden];
            if (hidden)
            {
                return;
            }
            
            rotation = fmod(rotation, 360.f);
            
            CCNode* parent = fOwner->getParent();
            if (parent != nullptr)
            {
#if defined(CC_TARGET_OS_MAC)
                [fUIView setBoundsRotation:rotation];
#else
                fUIView.transform = CGAffineTransformMakeRotation(M_PI * (rotation / 180.f));
#endif
                
                CCRect r = fOwner->boundingBox();
                r = CCRectApplyAffineTransform(r, parent->nodeToWorldTransform());
                
                r.origin.x = roundf(r.origin.x);
                r.origin.y = roundf(r.origin.y);
                
                r.size.width = ceilf(r.size.width);
                r.size.height = ceilf(r.size.height);
                
                CGRect f;
                f.origin.x = r.origin.x;
                
#if defined(CC_TARGET_OS_MAC)
                f.origin.y = r.origin.y;
#else
                // Convert upside down coordinates of cocos2d to UIKit ref system
                const float kScreenHeight = [(AppleView*)[EAGLView sharedEGLView] bounds].size.height;

                f.origin.y = kScreenHeight - (r.origin.y + r.size.height);
#endif
                
                f.size.width = r.size.width;
                f.size.height = r.size.height;
                
#if defined(CC_TARGET_OS_MAC)
                const float backingScale = frameBufferScale();
                
                CCEGLViewProtocol* eglView = CCEGLView::sharedOpenGLView();
                const float scaleX = eglView->getScaleX() / backingScale;
                const float scaleY = eglView->getScaleY() / backingScale;
                
                f.origin.x *= scaleX;
                f.origin.y *= scaleY;
                f.size.width *= scaleX;
                f.size.height *= scaleY;
#endif
                
#if defined(CC_TARGET_OS_MAC)
                if ([fUIView isKindOfClass:[NSTextView class]]) {
                    //[[(NSTextView*)fUIView enclosingScrollView] setFrame:f];
                    [[(NSTextView*)fUIView textContainer] setContainerSize:f.size];
                    //[(NSTextView*)fUIView setConstrainedFrameSize:f.size];
                }
 #endif
                
                [fUIView setFrame: f];
            }
        }
        else
        {
            [fUIView setHidden: YES];
        }
    }
    
    void
    CCNativeViewImplApple::_onEnter()
    {
        [[EAGLView sharedEGLView] addSubview:fUIView];
        
        _updateNativeUIView();
        
        if (fUpdater == nullptr)
        {
            fUpdater << new Updater(this);
            fUpdater->listen();
        }
    }
    
    void
    CCNativeViewImplApple::_onExit()
    {
        if (fUpdater != nullptr)
        {
            fUpdater->stopListening();
            fUpdater = nullptr;
        }
        
        [fUIView removeFromSuperview];
    }

    void
    CCNativeViewImplApple::onEnter()
    {
        _onEnter();
    }
    
    void
    CCNativeViewImplApple::onEnterTransitionDidFinish()
    {
        _onEnter();        
    }
    
    void
    CCNativeViewImplApple::onExit(void)
    {
        _onExit();
    }
    
    void
    CCNativeViewImplApple::onExitTransitionDidStart()
    {
        _onExit();
    }


NS_CC_EXT_END

NS_CC_BEGIN

#ifndef CC_TARGET_OS_MAC

    static UIView*
    _findFirstResponder(UIView* iRoot)
    {
        if (iRoot.isFirstResponder)
        {
            return iRoot;
        }
        
        for (UIView* child in iRoot.subviews)
        {
            UIView* r = _findFirstResponder(child);
            if (r != NULL)
            {
                return r;
            }
        }
        return NULL;
    }
#endif

    cocos2d::CCNode*
    nodeWithKeyboardFocus()
    {
    #ifdef CC_TARGET_OS_MAC
        NSWindow* rootWindow = [NSApplication sharedApplication].keyWindow;
        NSResponder *responder = [rootWindow firstResponder];
    #else
        UIView* root = [UIApplication sharedApplication].keyWindow;
        AppleView* responder = _findFirstResponder(root);
    #endif
        if ([responder conformsToProtocol:@protocol(Cocos2DNativeView)])
        {
            id<Cocos2DNativeView> view = (id<Cocos2DNativeView>) responder;
            return [view cocos2dNode];
        }
        return NULL;
    }

NS_CC_END

