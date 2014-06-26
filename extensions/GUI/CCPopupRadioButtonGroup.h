//
//  CCPopupRadioButtonGroup.h
//
//  Created by Gérald Guyomard on 3/4/14.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCPopupRadioButtonGroup__
#define __CCPopupRadioButtonGroup__

#include "GUI/CCRadioButtonGroup.h"
#include "cocoa/CCObjectPtr.h"

NS_CC_BEGIN
    class CCSmartLayer;
NS_CC_END

NS_CC_EXT_BEGIN

    class CCPopupRadioButtonGroup : public CCRadioButtonGroup
    {
    public:
        DOW_DECLARE_HIERARCHY(CCPopupRadioButtonGroup, CCRadioButtonGroup);
        virtual ~CCPopupRadioButtonGroup();
        
        static CCPopupRadioButtonGroup* create();
        static CCPopupRadioButtonGroup* createFromResource(const std::string& iResourceName, CCControlButton* iTriggerButton, CCNode* iContainer = nullptr);
        
        virtual void addChild(CCNode* child, int zOrder, int tag) override;
        virtual void removeChild(CCNode* iChild, bool cleanup) override;
        
        virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent) override;
        
        virtual bool select(CCControl* iButton) override;
        
        CCControlButton* triggerButton() const { return fTriggerButton; }
        
        typedef std::function<void(CCPopupRadioButtonGroup*)> TTriggerButtonUpdater;
        void setTriggerButtonUpdater(const TTriggerButtonUpdater& iUpdater);
        
        void defaultUpdateTriggerButtonAppearance();
        
    protected:
        CCPopupRadioButtonGroup();
        
        void _setTriggerButton(CCControlButton* iButton);
        void _updateTriggerButtonAppearance();
        
        void _triggerDisplay(CCObject*, CCControlEvent);
        void _show();
        void _hide();
        
        CCObjectPtr<CCControlButton> fTriggerButton;
        CCObjectPtr<CCNode> fContainer;
        
        TTriggerButtonUpdater fTriggerButtonUpdater;
    };
    
    class CCPopupRadioButtonGroupLoader : public CCRadioButtonGroupLoader
    {
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCPopupRadioButtonGroupLoader, loader);
        
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CCPopupRadioButtonGroup);
    };

NS_CC_EXT_END

#endif /* defined(__CCPopupRadioButtonGroup__) */
