//
//  KarmaIndicatorController.h
//
//  Created by Gérald Guyomard on 07/08/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __KarmaIndicatorController__
#define __KarmaIndicatorController__

#include "NodeControllers/NodeController.h"
#include <vector>

NS_CC_EXT_BEGIN

    class KarmaIndicatorController : public NodeController
    {
    public:
        DOW_DECLARE_HIERARCHY(KarmaIndicatorController, NodeController);
        static CCObjectPtr<KarmaIndicatorController> create();
        
        void setKarma(int32_t iKarma);
        
        bool isValid() const;
        void uninstall();
        
    protected:
        KarmaIndicatorController();
        
        virtual void _fillResolvers(Resolvers& iResolvers) override;
        virtual void _didLoad();
        
        CCObjectPtr<CCSprite> fSprite;
        
        std::vector<CCObjectPtr<CCSpriteFrame>> fSpriteFrames;
    };

NS_CC_EXT_END

#endif /* defined(__KarmaIndicatorController__) */
