//
//  CCTooltip.cpp
//
//  Created by Gérald Guyomard
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCTooltip.h"
#include "GUI/CCControlExtension/HitTestProtocol.h"
#include "NodeControllers/NodeController.h"

#include "misc_nodes/CCOverallScaleProtocol.h"

#include <algorithm> // for std::min
#undef min

NS_CC_BEGIN

    static CCTooltip* s_CurrentTooltip = nullptr;

    
    CCTooltip::~CCTooltip()
    {
        // Hey, calling terminate may crash
        // because the latter calls _hide which is virtual,
        // and vtable is destroyed when calling destructor chain !
        //terminate();
        
        if (s_CurrentTooltip == this)
        {
            s_CurrentTooltip = nullptr;
        }
    }
    
    static inline CCScheduler* _scheduler()
    {
        return CCDirector::sharedDirector()->getScheduler();
    }
    
    void
    CCTooltip::terminate()
    {
        _scheduler()->unscheduleAllForTarget(this);
        
        if (s_CurrentTooltip == this)
        {
            _hide();
            s_CurrentTooltip = nullptr;
        }
    }
    
    void
    CCTooltip::hideCurrentTooltip()
    {
        if (s_CurrentTooltip != nullptr)
        {
            s_CurrentTooltip->terminate();
        }
    }
    
    const float kShowWaitDuration = 1.f;
    void
    CCTooltip::onMouseEnter(const CCPoint& iWorldMouseLocation)
    {
        //assert(s_CurrentTooltip != this);
        
        fLastWorldMousePosition = iWorldMouseLocation;
        _scheduler()->scheduleSelector((SEL_SCHEDULE) &ThisClass::_onMouseStill, this, 0, 0, kShowWaitDuration, false);
    }
    
    void
    CCTooltip::_onMouseStill(float iDt)
    {
        if (s_CurrentTooltip != nullptr)
        {
            s_CurrentTooltip->_hide();
        }
        
        s_CurrentTooltip = this;

        _show(fLastWorldMousePosition);
    }
    
    void
    CCTooltip::onMouseOver(const CCPoint& iWorldMouseLocation)
    {
        if (iWorldMouseLocation != fLastWorldMousePosition)
        {
            terminate();
            _hide();
            
            fLastWorldMousePosition = iWorldMouseLocation;
            _scheduler()->scheduleSelector((SEL_SCHEDULE) &ThisClass::_onMouseStill, this, 0, 0, kShowWaitDuration, false);
        }
    }
    
    void
    CCTooltip::onMouseExit(const CCPoint& iWorldMouseLocation)
    {
        terminate();
        _hide();
    }

    class CCTextTooltip::Controller : public extension::NodeController
    {
    public:
        DOW_DECLARE_HIERARCHY(Controller, NodeController);
        
        static CCObjectPtr<Controller> create();
        
        void setText(const std::string& iText);
        
        void show(const CCPoint& iWorldMouseLocation);
        void hide();
        
    private:
        Controller() {}
        
        virtual void _fillResolvers(Resolvers& iResolvers) override;
        virtual void _didLoad() override;
        
        CCObjectPtr<CCLabelTTF> fLabel;
        float                   fMaximumWidth = 0;
    };

    CCObjectPtr<CCTextTooltip::Controller>
    CCTextTooltip::Controller::create()
    {
        CCObjectPtr<Controller> controller;
        controller << new Controller;
        controller->_load("textTooltip");
        
        return controller;
    }
    
    void
    CCTextTooltip::Controller::_fillResolvers(Resolvers& iResolvers)
    {
        SuperClass::_fillResolvers(iResolvers);
        
        iResolvers.memberVariableAssigner().addMember("label", fLabel);
    }
    
    void
    CCTextTooltip::Controller::_didLoad()
    {
        SuperClass::_didLoad();
        
        assert(fLabel != nullptr);
        fMaximumWidth = rootNode()->getContentSize().width;
    }
    
    void
    CCTextTooltip::Controller::show(const CCPoint& iWorldMouseLocation)
    {
        CCPoint pos = iWorldMouseLocation;
        
        CCNode* root = rootNode();
        CCNode* parent = root->getParent();
        if (parent == nullptr)
        {
            parent = CCDirector::sharedDirector()->getRunningScene();
            parent->addChild(root, +100000);
            
            CCRect bounds = root->boundingBox();
            
            bounds.origin = iWorldMouseLocation;
            bounds.origin.y += 4;
            
            bounds = bounds.constrainBoundsInScreen();
            
            root->ignoreAnchorPointForPosition(false);
            root->setAnchorPoint(CCPoint(0, 0));
            root->setPosition(bounds.origin);
            
            CCOverallScaleProtocol* proto = dynamic_cast<CCOverallScaleProtocol*>(parent);
            const float scale = (proto != nullptr) ? proto->overallScale() : 1.f;
            root->setScale(scale);
            
            CCNode::setOpacity(root, 0);
            root->runAction(CCFadeIn::create(0.2f));
        }
    }
    
    void
    CCTextTooltip::Controller::hide()
    {
        CCObjectPtr<Controller> self = this;
        
        rootNode()->runAction(CCSequence::createWithTwoActions(CCFadeOut::create(0.2f),
                                                         CCCallLambda::create([self]()
                                                        {
                                                            CCNode* root = self->rootNode();
                                                           root->removeFromParentAndCleanup(true);
                                                        })));
            

    }
    
    void
    CCTextTooltip::Controller::setText(const std::string& iText)
    {
        fLabel->setDimensions(CCSize(0, 0));
        
        fLabel->setString(iText.c_str());
        CCSize labelSize = fLabel->getContentSize();
        
        const float kMargin = 8;
        CCSize size = labelSize;
        size.width += kMargin * 2;
        size.height += kMargin * 2;
        
        size.width = std::min(size.width, fMaximumWidth);
        
        labelSize.width = size.width - (kMargin * 2);
        fLabel->setDimensions(CCSize(labelSize.width + 1, 0));
        labelSize = fLabel->getContentSize();
        
        size = labelSize;
        size.width += kMargin * 2;
        size.height += kMargin * 2;
        
        rootNode()->setContentSize(size);
    }
    
    CCTextTooltip::~CCTextTooltip()
    {
        if (fController != nullptr)
        {
            fController->hide();
        }
    }
    void
    CCTextTooltip::_show(const CCPoint& iWorldMouseLocation)
    {
        if (fController == nullptr)
        {
            fController = Controller::create();
            fController->setText(fText);
        }
        
        fController->show(iWorldMouseLocation);
    }
    
    void
    CCTextTooltip::_hide()
    {
        if (fController != nullptr)
        {
            fController->hide();
            fController = nullptr;
        }
    }
    
    void
    CCTextTooltip::setText(const std::string& iText)
    {
        fText = iText;
        
        if (fController != nullptr)
        {
             fController->setText(iText);
        }
    }
    
    
    CCTextTooltip::CCTextTooltip()
    {
    }
    
    CCTooltip::Ptr
    CCTextTooltip::create(const std::string& iText)
    {
        if (iText.empty())
        {
            return nullptr;
        }
        
        CCObjectPtr<CCTextTooltip> tooltip;
        tooltip << new CCTextTooltip;
        tooltip->setText(iText);
        
        return tooltip;
    }
    
    CCTooltip::Ptr
    CCTextTooltip::createWithLocalizedTextKey(const std::string& iKey)
    {
        std::string text = CCLocalizedStdString(iKey);
        return CCTextTooltip::create(text);
    }
    
NS_CC_END
