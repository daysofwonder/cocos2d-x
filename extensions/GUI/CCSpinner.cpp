//
//  CCSpinner.cpp
//
//  Created by Gérald Guyomard on 22/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "GUI/CCSpinner.h"
#include "CCBReader/CCBAnimationManager.h"
#include "NodeControllers/NodeController.h"

NS_CC_EXT_BEGIN

    class CCSpinner::Content : public NodeController
    {
    public:
        static CCObjectPtr<Content> create();
    };
    
    CCObjectPtr< CCSpinner::Content>
    CCSpinner::Content::create()
    {
        CCObjectPtr<Content> content;
        content << new Content;
        
        content->_load("BigSpinner");
        
        return content;
    }

    CCSpinner*
    CCSpinner::create()
    {
        CCSpinner* pRet = new CCSpinner();
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
    
    CCSpinner::CCSpinner()
    : fAutoAnimateWhenVisible(true)
    {
        
    }
    
    void
    CCSpinner::setVisible(bool iVisible)
    {
        const bool changed = (iVisible != isVisible());
        
        SuperClass::setVisible(iVisible);
        
        if (changed && autoAnimatesWhenVisible())
        {
            if (isVisible())
            {
                startAnimating();
            }
            else
            {
                stopAnimating();
            }
        }
        
    }
    
    bool
    CCSpinner::isAnimating() const
    {
        return fContent != nullptr;
    }
    
    void
    CCSpinner::startAnimating()
    {
        if (fContent == nullptr)
        {
            fContent = Content::create();
            CCNode* contentNode = fContent->rootNode();
            addChild(contentNode);
            
            contentNode->ignoreAnchorPointForPosition(true);
            contentNode->setPosition(CCPoint(0, 0));
            
            setContentSize(contentNode->getContentSize());
            
            // Get manually the animation and run it
            CCBAnimationManager* animManager = dynamic_cast<CCBAnimationManager*>(contentNode->getUserObject());
            assert(animManager != nullptr);
            if (animManager != nullptr)
            {
                animManager->runAnimationsForSequenceNamed("SpinningAnimation");
            }
        }
    }
    void
    CCSpinner::stopAnimating()
    {
        if (fContent != nullptr)
        {
            fContent->rootNode()->removeFromParentAndCleanup(true);
            fContent = nullptr;
        }
    }
    
    void
    CCSpinner::onNodeLoaded(CCNode * pNode, CCNodeLoader * pNodeLoader)
    {
        assert(pNode == this);
        
        if (autoAnimatesWhenVisible() && isVisible())
        {
            startAnimating();
        }
    }
    
    void
    CCSpinnerLoader::onHandlePropTypeCheck(CCNode * pNode, CCNode * pParent, const char * pPropertyName, bool pCheck, CCBReader * pCCBReader)
    {
        CCSpinner* spinner = static_cast<CCSpinner*>(pNode);
        assert(dynamic_cast<CCSpinner*>(pNode) != nullptr);
        
        if (strcmp(pPropertyName, "autoAnimatesWhenVisible") == 0)
        {
            spinner->setAutoAnimateWhenVisible(pCheck);
        }
        else
        {
            SuperClass::onHandlePropTypeCheck(pNode, pParent, pPropertyName, pCheck, pCCBReader);
        }
    }

NS_CC_EXT_END
