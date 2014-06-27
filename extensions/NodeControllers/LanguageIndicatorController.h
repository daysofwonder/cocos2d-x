//
//  LanguageIndicatorController.h
//
//  Created by Gérald Guyomard on 07/08/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __LanguageIndicatorController__
#define __LanguageIndicatorController__

#include "NodeControllers/NodeController.h"
#include <vector>

NS_CC_EXT_BEGIN

    class LanguageIndicatorController : public NodeController
    {
    public:
        DOW_DECLARE_HIERARCHY(LanguageIndicatorController, NodeController);
        static CCObjectPtr<LanguageIndicatorController> create();
        
        void setLanguage(uint32_t iLanguage);
        
        bool isValid() const;
        void uninstall();
        
    protected:
        LanguageIndicatorController();
        
        virtual void _fillResolvers(Resolvers& iResolvers) override;
        virtual void _didLoad();
        
        CCObjectPtr<CCSprite> fSprite;
    };

NS_CC_EXT_END

#endif /* defined(__LanguageIndicatorController__) */
