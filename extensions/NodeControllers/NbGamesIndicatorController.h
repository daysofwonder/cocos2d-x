//
//  NbGamesIndicatorController.h
//
//  Created by Gérald Guyomard on 07/08/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __NbGamesIndicatorController__
#define __NbGamesIndicatorController__

#include "NodeControllers/NodeController.h"

NS_CC_EXT_BEGIN

    class NbGamesIndicatorController : public NodeController
    {
    public:
        DOW_DECLARE_HIERARCHY(NbGamesIndicatorController, NodeController);
        static CCObjectPtr<NbGamesIndicatorController> create();
        
        void setNbGames(int32_t iNbGames);
        
        bool isValid() const;
        void uninstall();
        
    protected:
        NbGamesIndicatorController();
        
        virtual void _fillResolvers(Resolvers& iResolvers) override;
        virtual void _didLoad();
        
        CCObjectPtr<CCLabelTTF> fNbGamesLabel;
    };

NS_CC_EXT_END

#endif /* defined(__NbGamesIndicatorController__) */
