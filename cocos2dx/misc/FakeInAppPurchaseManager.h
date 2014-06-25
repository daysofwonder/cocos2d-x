//
//  FakeInAppPurchaseManager.h
//
//  Created by Gérald Guyomard on 30/01/13.
//  Copyright (c) 2013 Days of Wonder. All rights reserved.
//
//

#ifndef __FakeInAppPurchaseManager__
#define __FakeInAppPurchaseManager__

#include "DOWFoundation/InAppPurchaseManager.h"
#include <map>
#include <memory>

namespace cocos2d
{
    class FakeInAppPurchaseManager : public DOW::InAppPurchaseManager
    {
    public:
        
        static void init();
        
        virtual bool canMakePurchases() override;
        virtual bool isRestoringPurchases() const override;
        virtual void restorePurchases() override;
        
        virtual void registerInAppPurchase(const DOW::InAppPurchase::Identifier& iId) override;
        virtual DOW::InAppPurchase* inAppPurchase(const DOW::InAppPurchase::Identifier& iId) override;
        virtual bool iterateInAppPurchases(const TIterator& iIterator) const override;

        void setLocalizedPrice(const DOW::InAppPurchase::Identifier& iId, const std::string& iLocalizedPrice);
        
    private:
        FakeInAppPurchaseManager();
        virtual ~FakeInAppPurchaseManager();
        
        class FakeIAP;
        typedef std::map<std::string, std::unique_ptr<FakeIAP>> TIdToIAP;
        TIdToIAP fIdToIAP;
    };
}

#endif /* defined(__FakeInAppPurchaseManager__) */
