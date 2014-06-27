//
//  BadgeController.h
//
//  Created by Gérald Guyomard on 27/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __BadgeController__
#define __BadgeController__

#include "NodeControllers/NodeController.h"
#include "misc/NumberRasterizer.h"

NS_CC_EXT_BEGIN

    class BadgeController;
    typedef CCObjectPtr<BadgeController> BadgeControllerPtr;

    class BadgeController : public NodeController
    {
    public:
        DOW_DECLARE_HIERARCHY(BadgeController, NodeController);

        static const std::string kDefaultResourceName;
        static BadgeControllerPtr create(uint32_t iBadgeCount, const std::string& iResourceName, const std::string& iAtlasName);

        void setBadgeCount(uint32_t iCount);

    private:
        BadgeController(const std::string& iAtlasName);

        virtual void _fillResolvers(Resolvers& iResolvers) override;

        NumberRasterizer    fRasterizer;
        CCObjectPtr<CCNode> fLabelContainer;
        CCObjectPtr<CCNode> fLabel;
    };

    class BadgeHost
    {
    public:
        CCObjectPtr<CCNode> fHost;
        BadgeControllerPtr  fController;

        void setup(const std::string& iResourceName, const std::string& iAtlasName);
    };

NS_CC_EXT_END

#endif /* defined(__BadgeController__) */
