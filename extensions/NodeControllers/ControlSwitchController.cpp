//
//  ControlSwitchController.cpp
//
//  Created by Gérald Guyomard on 14/05/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "NodeControllers/ControlSwitchController.h"

NS_CC_EXT_BEGIN

    class ControlSwitchController::Parts
    {
    public:
        CCObjectPtr<CCSprite> fMaskSprite;
        CCObjectPtr<CCSprite> fOnPartSprite;
        CCObjectPtr<CCSprite> fOffPartSprite;
        CCObjectPtr<CCSprite> fThumbSprite;    
    };
    
    CCObjectPtr<ControlSwitchController>
    ControlSwitchController::create(const std::string& iMemberPrefix)
    {
        CCObjectPtr<ControlSwitchController> controller;
        controller << new ControlSwitchController(iMemberPrefix);
        
        return controller;
    }
    
    ControlSwitchController::ControlSwitchController(const std::string& iMemberPrefix)
    : fMemberPrefix(iMemberPrefix) {}
    
    ControlSwitchController::~ControlSwitchController()
    {
        
    }
    
    void
    ControlSwitchController::_fillResolvers(Resolvers& iResolvers)
    {
        SuperClass::_fillResolvers(iResolvers);

        fParts.reset(new Parts);
        
        auto& assigner = iResolvers.memberVariableAssigner();

        assigner.addMember(fMemberPrefix + "RootNode", fRootNode);

        assigner.addMember(fMemberPrefix + "Mask", fParts->fMaskSprite);
        assigner.addMember(fMemberPrefix + "OnPart", fParts->fOnPartSprite);
        assigner.addMember(fMemberPrefix + "OffPart", fParts->fOffPartSprite);
        assigner.addMember(fMemberPrefix + "Thumb", fParts->fThumbSprite);
    }
    
    void
    ControlSwitchController::_didLoad()
    {
        SuperClass::_didLoad();
        
        assert(fParts->fMaskSprite != nullptr);
        assert(fParts->fOnPartSprite != nullptr);
        assert(fParts->fOffPartSprite != nullptr);
        assert(fParts->fThumbSprite != nullptr);
        
        fParts->fMaskSprite->removeFromParentAndCleanup(false);
        fParts->fOnPartSprite->removeFromParentAndCleanup(false);
        fParts->fOffPartSprite->removeFromParentAndCleanup(false);
        fParts->fThumbSprite->removeFromParentAndCleanup(false);
        
        const CCSize& thumbSize = fParts->fThumbSprite->getContentSize();
        const CCSize& totalSize = fParts->fMaskSprite->getContentSize();
        
        const float xRatio = thumbSize.width / totalSize.width;
        const float yRatio = thumbSize.height / totalSize.height;
        const bool isVertical = xRatio > yRatio;
        
		fSwitch = CCControlSwitch::create(fParts->fMaskSprite, fParts->fOnPartSprite, fParts->fOffPartSprite, fParts->fThumbSprite, isVertical);

        fParts.reset();
        
        assert(fRootNode != nullptr);
        fSwitch->ignoreAnchorPointForPosition(true);

        fRootNode->addChild(fSwitch);
        
        fSwitch->addTargetWithActionForControlEvents(this, (SEL_CCControlHandler) &ThisClass::_onValueChanged,  CCControlEventValueChanged);
    }

    void
    ControlSwitchController::_onValueChanged(CCObject*, CCControlEvent)
    {
        if (fValueChangedListener != nullptr)
        {
            fValueChangedListener(this);
        }
    }
    
    bool
    ControlSwitchController::isOn() const
    {
        return fSwitch->isOn();
    }
    
    void
    ControlSwitchController::setOn(bool on, bool iAnimated)
    {
        if (on != isOn())
        {
            fSwitch->setOn(on, iAnimated);
        }
    }

NS_CC_EXT_END
