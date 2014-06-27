//
//  BadgeController.cpp
//
//  Created by Gérald Guyomard on 27/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "NodeControllers/BadgeController.h"
#include "DOWFoundation/StringUtils.h"
#include "misc/NumberRasterizer.h"

NS_CC_EXT_BEGIN

    const std::string BadgeController::kDefaultResourceName = "BadgeController";

    BadgeControllerPtr
    BadgeController::create(uint32_t iBadgeCount, const std::string& iResourceName, const std::string& iAtlasName)
    {
        BadgeControllerPtr controller;
        controller << new BadgeController(iAtlasName);
        
        controller->_load(iResourceName);
        
        controller->setBadgeCount(iBadgeCount);
        
        return controller;
    }
    
    BadgeController::BadgeController(const std::string& iAtlasName)
    : fRasterizer(iAtlasName)
    {}
    
    void
    BadgeController::setBadgeCount(uint32_t iCount)
    {
        if (fLabel != nullptr)
        {
            fLabel->removeFromParentAndCleanup(true);
        }
        
        fLabel = fRasterizer.rasterize(iCount, fLabelContainer, 0.8f);
    }
    
    void
    BadgeController::_fillResolvers(Resolvers& iResolvers)
    {
        SuperClass::_fillResolvers(iResolvers);
        
        iResolvers.memberVariableAssigner().addMember("labelContainer", fLabelContainer);
    }
    
    void
    BadgeHost::setup(const std::string& iResourceName, const std::string& iAtlasName)
    {
        assert(fHost != nullptr);
        fController = BadgeController::create(1, iResourceName, iAtlasName);
        
        CCNode* root = fController->rootNode();
        root->ignoreAnchorPointForPosition(false);
        root->setAnchorPoint(CCPoint(0.5f, 0.5f));
        
        const CCSize& size = fHost->getContentSize();
        root->setPosition(size.width / 2, size.height / 2);
        
        fHost->addChild(root);
    }

NS_CC_EXT_END
