//
//  NbGamesIndicatorController.cpp
//
//  Created by Gérald Guyomard on 07/08/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "NodeControllers/NbGamesIndicatorController.h"
#include "DOWFoundation/StringUtils.h"

NS_CC_EXT_BEGIN

    CCObjectPtr<NbGamesIndicatorController>
    NbGamesIndicatorController::create()
    {
        CCObjectPtr<NbGamesIndicatorController> controller;
        controller << new NbGamesIndicatorController;
        
        return controller;
    }
    
    void
    NbGamesIndicatorController::setNbGames(int32_t iNbGames)
    {
        if (iNbGames < 0)
        {
            rootNode()->setVisible(false);
        }
        else
        {
            rootNode()->setVisible(true);
            
            std::string s;
            DOW::StringUtils::format(s, "%d", iNbGames);
            
            fNbGamesLabel->setString(s.c_str());
        }
    }
    

    NbGamesIndicatorController::NbGamesIndicatorController()
    {}
    
    void
    NbGamesIndicatorController::_fillResolvers(Resolvers& iResolvers)
    {
        SuperClass::_fillResolvers(iResolvers);
        
        auto& assigner = iResolvers.memberVariableAssigner();
        
        assigner.addMember("nbGamesIndicatorController", fRootNode);
        assigner.addMember("nbGamesLabel", fNbGamesLabel);
    }
    
    bool
    NbGamesIndicatorController::isValid() const
    {
        return (rootNode() != nullptr) && (fNbGamesLabel != nullptr);
    }
    
    void
    NbGamesIndicatorController::uninstall()
    {
        auto* root = rootNode();
        if (root != nullptr)
        {
            root->removeFromParentAndCleanup(true);
        }
    }

    void
    NbGamesIndicatorController::_didLoad()
    {
        SuperClass::_didLoad();
        
        if (isValid())
        {
            setNbGames(-1);
        }
    }

NS_CC_EXT_END
