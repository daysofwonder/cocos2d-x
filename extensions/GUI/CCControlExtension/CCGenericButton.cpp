//
//  CCGenericButton.cpp
//
//  Created by Gérald Guyomard on 3/6/14.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCGenericButton.h"
#include "NodeControllers/NodeController.h"

NS_CC_EXT_BEGIN

    enum
    {
        kZoomActionTag = 0xCCCB0001,
    };

    
    CCGenericButton::CCGenericButton()
    {
        
    }
    
    CCGenericButton::~CCGenericButton()
    {
        
    }
    
    CCGenericButton*
    CCGenericButton::create()
    {
        CCGenericButton* button = new CCGenericButton;
        if (!button->init())
        {
            button->release();
            return nullptr;
        }
            
        button->autorelease();
        return button;
    }
    
    bool
    CCGenericButton::init()
    {
        if (!SuperClass::init())
        {
            return false;
        }
        
        setTouchEnabled(true);
        
        return true;
    }
    
    void
    CCGenericButton::registerWithTouchDispatcher(void)
    {
        CCTouchDispatcher* dispatcher = CCDirector::sharedDirector()->getTouchDispatcher();
        dispatcher->addTargetedDelegate(this, 0, true);
    }
    
    void
    CCGenericButton::onNodeLoaded(CCNode * pNode, CCNodeLoader * pNodeLoader)
    {
        // Get all the children for states
        for (uint32_t i=0; i < kStateCount; ++i)
        {
            fStateNode[i] = getChildByTag(i);
        }
        
        needsLayout();
    }
    
    void
    CCGenericButton::needsLayout()
    {
        SuperClass::needsLayout();
        
        if (isEnabled())
        {
            if (isHighlighted())
            {
                if (!_updateState(kHighlightedState))
                {
                    if (!_updateState(kSelectedState))
                    {
                        _updateState(kEnabledState);
                    }
                }
            }
            else if (isSelected())
            {
                if (!_updateState(kSelectedState))
                {
                    _updateState(kEnabledState);
                }
            }
            else
            {
                _updateState(kEnabledState);
            }
        }
        else
        {
            _updateState(kDisabledState);
        }
    }
    
    bool
    CCGenericButton::_updateState(EState iState)
    {
        bool done = false;
        
        for (uint32_t i=0; i < kStateCount; ++i)
        {
            CCNode* node = fStateNode[i];
            if (node != nullptr)
            {
                const bool visible = (i == iState);
                node->setVisible(visible);
                
                if (visible)
                {
                    done = true;
                }
            }
        }
        
        return done;
    }
    
    bool
    CCGenericButton::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
    {
        if (!isTouchInside(pTouch) || !isEnabled() || !isVisible() || !hasVisibleParents() )
        {
            return false;
        }

        fSavedScale = getScale();
        
        fIsPushed = true;
        setHighlighted(true);
        
        sendActionsForControlEvents(CCControlEventTouchDown);
        return true;

    }
    
    void
    CCGenericButton::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
    {
        if (!isEnabled() || !isPushed() || isSelected())
        {
            if (isHighlighted())
            {
                setHighlighted(false);
            }
            return;
        }
        
        bool isTouchMoveInside = isTouchInside(pTouch);
        if (isTouchMoveInside && !isHighlighted())
        {
            setHighlighted(true);
            sendActionsForControlEvents(CCControlEventTouchDragEnter);
        }
        else if (isTouchMoveInside && isHighlighted())
        {
            sendActionsForControlEvents(CCControlEventTouchDragInside);
        }
        else if (!isTouchMoveInside && isHighlighted())
        {
            setHighlighted(false);
            
            sendActionsForControlEvents(CCControlEventTouchDragExit);
        }
        else if (!isTouchMoveInside && !isHighlighted())
        {
            sendActionsForControlEvents(CCControlEventTouchDragOutside);        
        }
    }
    
    void
    CCGenericButton::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
    {
        fIsPushed = false;
        setHighlighted(false);
        
        if (isTouchInside(pTouch))
        {
            sendActionsForControlEvents(CCControlEventTouchUpInside);
        }
        else
        {
            sendActionsForControlEvents(CCControlEventTouchUpOutside);
        }
    }
    
    void
    CCGenericButton::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
    {
        fIsPushed = false;
        setHighlighted(false);
        
        sendActionsForControlEvents(CCControlEventTouchCancel);
    }
    
    void
    CCGenericButton::setHighlighted(bool bHighlighted)
    {
        SuperClass::setHighlighted(bHighlighted);
        
        CCAction *action = getActionByTag(kZoomActionTag);
        if (action)
        {
            stopAction(action);
        }
        
        needsLayout();
        
        if(isZoomedOnTouch())
        {
            float scaleValue = (isHighlighted() && isEnabled() && !isSelected()) ? 1.1f : 1.0f;
            scaleValue *= fSavedScale;
            
            CCAction *zoomAction = CCScaleTo::create(0.05f, scaleValue);
            zoomAction->setTag(kZoomActionTag);
            runAction(zoomAction);
        }

    }
    
    void
    CCGenericButtonLoader::onHandlePropTypeCheck(CCNode * pNode, CCNode * pParent, const char * pPropertyName, bool pCheck, CCBReader * pCCBReader)
    {
        CCGenericButton* button = static_cast<CCGenericButton*>(pNode);
        
        if (strcmp(pPropertyName, "isZoomedOnTouch") == 0)
        {
            button->setIsZoomedOnTouch(pCheck);
        }
        else
        {
            SuperClass::onHandlePropTypeCheck(pNode, pParent, pPropertyName, pCheck, pCCBReader);
        }
    }
    
NS_CC_EXT_END
