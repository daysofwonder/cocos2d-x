//
//  LanguageIndicatorController.cpp
//
//  Created by Gérald Guyomard on 07/08/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "NodeControllers/LanguageIndicatorController.h"
#include "DOWFoundation/StringUtils.h"

NS_CC_EXT_BEGIN

    CCObjectPtr<LanguageIndicatorController>
    LanguageIndicatorController::create()
    {
        CCObjectPtr<LanguageIndicatorController> controller;
        controller << new LanguageIndicatorController;
        
        return controller;
    }
    
    void
    LanguageIndicatorController::setLanguage(uint32_t iLanguage)
    {
        if (iLanguage == 0)
        {
            rootNode()->setVisible(false);
        }
        else
        {
            // Language
            const char* languageFrameName = nullptr;
            switch (iLanguage)
            {
                case 0:
                    languageFrameName = "online_arena-playerInfo-flagUS.png";
                    break;
                    
                case 1:
                    languageFrameName = "online_arena-playerInfo-flagFR.png";
                    break;
                    
                case 2:
                    languageFrameName = "online_arena-playerInfo-flagDE.png";
                    break;
                    
                default:
                    break;
            }
            
            if (languageFrameName != nullptr)
            {
                fSprite->setDisplayFrame(CCSpriteFrame::spriteFrameWithName(languageFrameName));
                rootNode()->setVisible(true);
            }
            else
            {
                rootNode()->setVisible(false);
            }
        }
    }
    

    LanguageIndicatorController::LanguageIndicatorController()
    {}
    
    void
    LanguageIndicatorController::_fillResolvers(Resolvers& iResolvers)
    {
        SuperClass::_fillResolvers(iResolvers);
        
        auto& assigner = iResolvers.memberVariableAssigner();
        
        assigner.addMember("languageIndicatorController", fRootNode);
        assigner.addMember("languageIndicator", fSprite);
    }
    
    bool
    LanguageIndicatorController::isValid() const
    {
        return (rootNode() != nullptr) && (fSprite != nullptr);
    }
    
    void
    LanguageIndicatorController::uninstall()
    {
        auto* root = rootNode();
        if (root != nullptr)
        {
            root->removeFromParentAndCleanup(true);
        }
    }

    void
    LanguageIndicatorController::_didLoad()
    {
        SuperClass::_didLoad();
        
        if (isValid())
        {
            setLanguage(0);
        }
    }

NS_CC_EXT_END
