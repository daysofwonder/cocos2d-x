//
//  KarmaIndicatorController.cpp
//
//  Created by Gérald Guyomard on 07/08/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "NodeControllers/KarmaIndicatorController.h"
#include "DOWFoundation/StringUtils.h"


NS_CC_EXT_BEGIN

    CCObjectPtr<KarmaIndicatorController>
    KarmaIndicatorController::create()
    {
        CCObjectPtr<KarmaIndicatorController> controller;
        controller << new KarmaIndicatorController;
        
        return controller;
    }
    
    void
    KarmaIndicatorController::setKarma(int32_t iKarma)
    {
        if (iKarma < 0)
        {
            rootNode()->setVisible(false);
        }
        else
        {
            rootNode()->setVisible(true);
            
            const size_t nbFrames = fSpriteFrames.size();
            size_t index = (nbFrames * iKarma) / 100;
            if (index >= nbFrames)
            {
                index = nbFrames - 1;
            }
            
            fSprite->setDisplayFrame(fSpriteFrames[index]);
        }
    }
    

    KarmaIndicatorController::KarmaIndicatorController()
    {}
    
    void
    KarmaIndicatorController::_fillResolvers(Resolvers& iResolvers)
    {
        SuperClass::_fillResolvers(iResolvers);
        
        auto& assigner = iResolvers.memberVariableAssigner();
        
        assigner.addMember("karmaIndicatorController", fRootNode);
        assigner.addMember("karmaIndicator", fSprite);
    }
    
    bool
    KarmaIndicatorController::isValid() const
    {
        return (rootNode() != nullptr) && (fSprite != nullptr);
    }
    
    void
    KarmaIndicatorController::uninstall()
    {
        auto* root = rootNode();
        if (root != nullptr)
        {
            root->removeFromParentAndCleanup(true);
        }
    }

    void
    KarmaIndicatorController::_didLoad()
    {
        SuperClass::_didLoad();
        
        if (isValid())
        {
            const size_t kNbFrames = 5;
            fSpriteFrames.resize(kNbFrames);
            
            for (size_t i=0; i < kNbFrames; ++i)
            {
                std::string name;
                DOW::StringUtils::format(name, "karma-%ddots.png", i + 1);
                fSpriteFrames[i] = CCSpriteFrame::spriteFrameWithName(name);
                assert(fSpriteFrames[i] != nullptr);
            }
            
            setKarma(-1);
        }
    }

NS_CC_EXT_END
