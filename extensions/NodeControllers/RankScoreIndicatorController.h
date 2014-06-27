//
//  RankScoreIndicatorController.h
//
//  Created by Gérald Guyomard on 07/08/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __RankScoreIndicatorController__
#define __RankScoreIndicatorController__

#include "NodeControllers/NodeController.h"
#include <vector>

NS_CC_EXT_BEGIN

    class RankScoreIndicatorController : public NodeController
    {
    public:
        DOW_DECLARE_HIERARCHY(RankScoreIndicatorController, NodeController);
        static CCObjectPtr<RankScoreIndicatorController> create();
        
        void setRankScore(float iRankScore);
        
        bool isValid() const;
        void uninstall();
        
    protected:
        RankScoreIndicatorController();
        
        virtual void _fillResolvers(Resolvers& iResolvers) override;
        virtual void _didLoad();
        
        CCObjectPtr<CCLabelTTF> fRankScoreLabel;
    };

NS_CC_EXT_END

#endif /* defined(__RankScoreIndicatorController__) */
