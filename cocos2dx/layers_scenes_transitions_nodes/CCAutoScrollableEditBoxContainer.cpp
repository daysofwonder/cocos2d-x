//
//  CCAutoScrollableEditBoxContainer.cpp
//
//  Created by Gérald Guyomard on 28/01/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCAutoScrollableEditBoxContainer.h"
#include "CCNativeSmartLayer.h"


NS_CC_BEGIN

    CCAutoScrollableEditBoxContainer* CCAutoScrollableEditBoxContainer::create()
    {
        CCAutoScrollableEditBoxContainer* pRet = new CCAutoScrollableEditBoxContainer();
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
    
    CCAutoScrollableEditBoxContainer::CCAutoScrollableEditBoxContainer()
    : fScrolledUp(false), fDistanceFromKeyboard(20)
    {}
    
    const static int kActionTag = 10;
    
    void
    CCAutoScrollableEditBoxContainer::keyboardWillShow(CCIMEKeyboardNotificationInfo& info)
    {
        CCNode* focusedNode = nodeWithKeyboardFocus();
        if ((focusedNode != nullptr) && isAncestor(focusedNode))
        {
            // Verify overlap
            const CCRect& keyboardBox = info.end;
            const float keyboardTop = keyboardBox.origin.y + keyboardBox.size.height;
            
            CCRect bbox = CCRectApplyAffineTransform(focusedNode->boundingBox(), focusedNode->getParent()->nodeToWorldTransform());
            const float distance = bbox.origin.y - (keyboardTop + fDistanceFromKeyboard);
            if (distance < 0)
            {
                // Must move
                if (!fScrolledUp)
                {
                    fSavedPosition = getPosition();
                    fScrolledUp = true;
                }
                
                stopActionByTag(kActionTag);
                
                CCMoveBy* moveBy = CCMoveBy::create(info.duration, CCPoint(0, -distance));
                moveBy->setTag(kActionTag);
                runAction(moveBy);
            }
        }
    }
    
    void
    CCAutoScrollableEditBoxContainer::keyboardDidShow(CCIMEKeyboardNotificationInfo& info)
    {
        
    }
    
    void
    CCAutoScrollableEditBoxContainer::keyboardWillHide(CCIMEKeyboardNotificationInfo& info)
    {
        if (fScrolledUp)
        {
            stopActionByTag(kActionTag);
            fScrolledUp = false;
            
            CCMoveTo* moveTo = CCMoveTo::create(info.duration, fSavedPosition);
            moveTo->setTag(kActionTag);
            runAction(moveTo);            
        }
    }
    
    void
    CCAutoScrollableEditBoxContainer::keyboardDidHide(CCIMEKeyboardNotificationInfo& info)
    {
        
    }
    
    void
    CCAutoScrollableEditBoxContainer::onEnter()
    {
        SuperClass::onEnter();
    }
    
    void
    CCAutoScrollableEditBoxContainer::onExit()
    {
        SuperClass::onExit();
    }

NS_CC_END

NS_CC_EXT_BEGIN

    void
    CCAutoScrollableEditBoxContainerLoader::onHandlePropTypeFloat(CCNode * pNode, CCNode * pParent, const char* pPropertyName, float pFloat, CCBReader * pCCBReader)
    {
        CCAutoScrollableEditBoxContainer* container = static_cast<CCAutoScrollableEditBoxContainer*>(pNode);
        if (::strcmp(pPropertyName, "distanceFromKeyboard") == 0)
        {
            container->setDistanceFromKeyboard(pFloat);
        }
        else
        {
            CCSmartLayerLoader::onHandlePropTypeFloat(pNode, pParent, pPropertyName, pFloat, pCCBReader);
        }
    }

NS_CC_EXT_END
