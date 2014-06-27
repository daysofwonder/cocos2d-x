//
//  RankScoreIndicatorController.cpp
//
//  Created by Gérald Guyomard on 07/08/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "NodeControllers/RankScoreIndicatorController.h"
#include "DOWFoundation/StringUtils.h"


NS_CC_EXT_BEGIN

    CCObjectPtr<RankScoreIndicatorController>
    RankScoreIndicatorController::create()
    {
        CCObjectPtr<RankScoreIndicatorController> controller;
        controller << new RankScoreIndicatorController;
        
        return controller;
    }
    
    void
    RankScoreIndicatorController::setRankScore(float iRankScore)
    {
        if (iRankScore <= 0)
        {
            rootNode()->setVisible(false);
        }
        else
        {
            rootNode()->setVisible(true);

            const int32_t score = (int32_t) roundf(iRankScore);
            std::string s;
            DOW::StringUtils::format(s, "%d", score);
            
            fRankScoreLabel->setString(s.c_str());
        }
    }
    

    RankScoreIndicatorController::RankScoreIndicatorController()
    {}
    
    void
    RankScoreIndicatorController::_fillResolvers(Resolvers& iResolvers)
    {
        SuperClass::_fillResolvers(iResolvers);
        
        auto& assigner = iResolvers.memberVariableAssigner();
        
        assigner.addMember("rankScoreIndicatorController", fRootNode);
        assigner.addMember("rankScoreLabel", fRankScoreLabel);
    }
    
    bool
    RankScoreIndicatorController::isValid() const
    {
        return (rootNode() != nullptr) && (fRankScoreLabel != nullptr);
    }
    
    void
    RankScoreIndicatorController::uninstall()
    {
        auto* root = rootNode();
        if (root != nullptr)
        {
            root->removeFromParentAndCleanup(true);
        }
    }

    void
    RankScoreIndicatorController::_didLoad()
    {
        SuperClass::_didLoad();
        
        if (isValid())
        {            
            setRankScore(-1);
        }
    }

NS_CC_EXT_END
