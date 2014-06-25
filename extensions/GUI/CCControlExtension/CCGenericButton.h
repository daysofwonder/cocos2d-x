//
//  CCGenericButton.h
//
//  Created by Gérald Guyomard on 3/6/14.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCGenericButton__
#define __CCGenericButton__

#include "GUI/CCControlExtension/CCControl.h"
#include "cocoa/CCObjectPtr.h"
#include "CCBReader/CCNodeLoaderListener.h"
#include "CCBReader/CCControlLoader.h"

NS_CC_EXT_BEGIN

    class CCGenericButton : public CCControl, public CCNodeLoaderListener
    {
    public:
        DOW_DECLARE_HIERARCHY(CCGenericButton, CCControl);
        
        CCGenericButton();
        virtual ~CCGenericButton();
        
        static CCGenericButton* create();
        
        virtual void registerWithTouchDispatcher(void) override;
        
        virtual void needsLayout() override;
        virtual void onNodeLoaded(CCNode * pNode, CCNodeLoader * pNodeLoader) override;
        
        virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent) override;
        virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent) override;
        virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) override;
        virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent) override;
        
        virtual void setHighlighted(bool bHighlighted) override;

        bool isPushed() const { return fIsPushed; }
        
        bool isZoomedOnTouch() const { return fIsZoomedOnTouch; }
        void setIsZoomedOnTouch(bool iZoom) { fIsZoomedOnTouch = iZoom; }
        
    protected:
        
        bool init();
        
        enum EState
        {
            kDisabledState = 0,
            kEnabledState,
            kSelectedState,
            kHighlightedState,
            
            kStateCount
        };
        
        bool _updateState(EState iState);
        
        CCObjectPtr<CCNode> fStateNode[kStateCount];
        uint16_t fIsPushed = false;
        uint16_t fIsZoomedOnTouch = false;
        float fSavedScale = 0;
    };
    
    class CCGenericButtonLoader : public CCControlLoader
    {
    public:
        DOW_DECLARE_HIERARCHY(CCGenericButtonLoader, CCControlLoader);
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCGenericButtonLoader, loader);
        
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CCGenericButton);
        
        virtual void onHandlePropTypeCheck(CCNode * pNode, CCNode * pParent, const char * pPropertyName, bool pCheck, CCBReader * pCCBReader) override;

    };

NS_CC_EXT_END

#endif /* defined(__CCGenericButton__) */
