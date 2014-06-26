//
//  CCPopupRadioButtonGroup.cpp
//
//  Created by Gérald Guyomard on 3/4/14.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "GUI/CCPopupRadioButtonGroup.h"

#include "misc_nodes/CCOverallScaleProtocol.h"
#include "CCBReader/CCNodeLoaderLibrary.h"

NS_CC_EXT_BEGIN

    CCPopupRadioButtonGroup::CCPopupRadioButtonGroup()
    {
    }
    
    CCPopupRadioButtonGroup::~CCPopupRadioButtonGroup()
    {
        
    }
    
    CCPopupRadioButtonGroup*
    CCPopupRadioButtonGroup::create()
    {
        CCPopupRadioButtonGroup* pRet = new CCPopupRadioButtonGroup();
        if (pRet && pRet->init())
        {
            pRet->autorelease();
        }
        else
        {
            CC_SAFE_DELETE(pRet);
        }
        return pRet;
    }
    
    CCPopupRadioButtonGroup*
    CCPopupRadioButtonGroup::createFromResource(const std::string& iResourceName, CCControlButton* iTriggerButton, CCNode* iContainer)
    {
        CCNodeLoaderLibrary* lib = CCNodeLoaderLibrary::sharedCCNodeLoaderLibrary();
        
        CCObjectPtr<CCBReader> reader;
        reader << new CCBReader(lib, nullptr, nullptr);
        
        CCSize size = CCDirector::sharedDirector()->getWinSize();
        
        std::string fileName = iResourceName + ".ccbi";
        CCPopupRadioButtonGroup* group = dynamic_cast<CCPopupRadioButtonGroup*>(reader->readNodeGraphFromFile(fileName.c_str(), nullptr, size));

        if (group != nullptr)
        {
            group->_setTriggerButton(iTriggerButton);
            group->fContainer = iContainer;
        }
        
        return group;
    }
    
    void
    CCPopupRadioButtonGroup::addChild(CCNode* child, int zOrder, int tag)
    {
        CCControl* control = _controlFromChild(child);
        if (control != nullptr)
        {
            zOrder = 1;
        }
        
        SuperClass::addChild(child, zOrder, tag);
        
        _updateTriggerButtonAppearance();
    }
    
    void
    CCPopupRadioButtonGroup::removeChild(CCNode* iChild, bool cleanup)
    {
        SuperClass::removeChild(iChild, cleanup);
        
        _updateTriggerButtonAppearance();
    }
    
    void
    CCPopupRadioButtonGroup::_setTriggerButton(CCControlButton* iButton)
    {
        fTriggerButton = iButton;
        assert(fTriggerButton != nullptr);
        
        fTriggerButton->addTargetWithActionForControlEvents(this, (SEL_CCControlHandler) &ThisClass::_triggerDisplay, CCControlEventTouchUpInside);
        _updateTriggerButtonAppearance();
    }
    
    void
    CCPopupRadioButtonGroup::defaultUpdateTriggerButtonAppearance()
    {
        if (fTriggerButton != nullptr)
        {
            auto* selected = selectedControl();
            if (selected != nullptr)
            {
                // Because cloning a button (then a CCScale9Sprite) is such a mess
                // Let's do the brute force way: render selectedButton in its current state
                // And apply this texture to the trigger button
                const CCSize& s = selected->getContentSize();
                RenderTexture* render = RenderTexture::create(s.width, s.height);
                
                render->renderNode(selected);
                
                auto* frame = render->getSprite()->displayFrame();
                fTriggerButton->setBackground(frame);
                
                fTriggerButton->setPreferredSize(s);
            }
        }
    }
    
    void
    CCPopupRadioButtonGroup::_updateTriggerButtonAppearance()
    {
        if (fTriggerButton != nullptr)
        {
            if (fTriggerButtonUpdater != nullptr)
            {
                fTriggerButtonUpdater(this);
            }
            else
            {
                defaultUpdateTriggerButtonAppearance();
            }
        }
    }
    
    void
    CCPopupRadioButtonGroup::setTriggerButtonUpdater(const TTriggerButtonUpdater& iUpdater)
    {
        fTriggerButtonUpdater = iUpdater;
        _updateTriggerButtonAppearance();
    }
    
    void
    CCPopupRadioButtonGroup::_triggerDisplay(CCObject*, CCControlEvent)
    {
        _show();
    }
    
    bool
    CCPopupRadioButtonGroup::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
    {
        SuperClass::ccTouchBegan(pTouch, pEvent);
        
        const CCPoint loc = pTouch->getLocation();
        
        if (!isTouchInside(loc))
        {
            _hide();
        }
        
        return true; // In any case we want to consume the hit on screen when visible
    }
    
    const int kActionTag = 10;
    const float kActionDuration = 0.2f;
    
    void
    CCPopupRadioButtonGroup::_show()
    {
        stopActionByTag(kActionTag);
        
        
        if (fContainer != nullptr)
        {
            const CCSize& containerSize = fContainer->getContentSize();
            ignoreAnchorPointForPosition(false);
            setAnchorPoint(CCPoint(0.5f, 0.5f));
            setPosition(CCPoint(containerSize.width / 2, containerSize.height / 2));
            
            fContainer->addChild(this, +10000);
        }
        else
        {
            // Floating mode
            CCNode* parent = CCDirector::sharedDirector()->getRunningScene();
            CCPoint worldPos = CCNode::convertToNodeSpace(parent, fTriggerButton->getParent(), fTriggerButton->getPosition());
            
            parent->addChild(this, +100000);
            
            CCRect bounds = boundingBox();
            
            bounds.origin = worldPos;
            bounds.origin.x -= (bounds.size.width / 2);
            bounds.origin.y -= (bounds.size.height / 2);
            
            bounds = bounds.constrainBoundsInScreen();
            
            ignoreAnchorPointForPosition(false);
            setAnchorPoint(CCPoint(0, 0));
            setPosition(bounds.origin);
            
            CCOverallScaleProtocol* proto = dynamic_cast<CCOverallScaleProtocol*>(parent);
            const float scale = (proto != nullptr) ? proto->overallScale() : 1.f;
            setScale(scale);
        }

        useRenderTexture(true);
        
        CCNode::setOpacity(this, 0);
        
        auto* action = CCFadeIn::create(kActionDuration);
        action->setTag(kActionTag);
        
        runAction(action);
    }
    
    void
    CCPopupRadioButtonGroup::_hide()
    {
        if (getParent() == nullptr)
        {
            return;
        }
        
        stopActionByTag(kActionTag);
        
        auto* fadeOut = CCFadeOut::create(kActionDuration);
        
        CCCallLambda* completion = CCCallLambda::create(
            [this]()
            {
                _updateTriggerButtonAppearance();
                removeFromParentAndCleanup(true);
            });
        
        auto* action = CCSequence::createWithTwoActions(fadeOut, completion);
        action->setTag(kActionTag);
        
        runAction(action);
    }
    
    bool
    CCPopupRadioButtonGroup::select(CCControl* iControl)
    {
        const bool changed = SuperClass::select(iControl);
        
        _updateTriggerButtonAppearance();
        
        _hide();
        
        return changed;
    }

NS_CC_EXT_END
