//
//  FakeInAppPurchaseManager.cpp
//
//  Created by Gérald Guyomard on 30/01/13.
//  Copyright (c) 2013 Days of Wonder. All rights reserved.
//
//

#include <cocos2d.h>

#include "misc/FakeInAppPurchaseManager.h"
#include "DOWFoundation/NotificationCenter.h"
#include "NodeControllers/MessageBoxController.h"
#include "DOWFoundation/StringUtils.h"


namespace cocos2d
{
    using namespace DOW;
    using namespace extension;
    
    void
    FakeInAppPurchaseManager::init()
    {
        static FakeInAppPurchaseManager instance;
    }

    bool
    FakeInAppPurchaseManager::canMakePurchases()
    {
        return true;
    }
    
    bool
    FakeInAppPurchaseManager::isRestoringPurchases() const
    {
        return false;
    }
    
    
    class FakeInAppPurchaseManager::FakeIAP : public ::DOW::InAppPurchase
    {
    public:
        DOW_DECLARE_HIERARCHY(FakeInAppPurchaseManager::FakeIAP, InAppPurchase);
        FakeIAP(const std::string& iID);
        
        virtual void retrieveInfo() override;
        virtual void purchase() override;
        
        void resetPurchase();
        
        void setLocalizedPrice(const std::string& iPrice) { fLocalizedPrice = iPrice; }
        
    protected:
        std::string _statusKey() const;
        virtual bool _setStatus(EStatus iStatus) override;
        
        std::string fLocalizedPrice;
    };

    FakeInAppPurchaseManager::FakeIAP::FakeIAP(const std::string& iId)
    : SuperClass(iId)
    {}

    void
    FakeInAppPurchaseManager::FakeIAP::retrieveInfo()
    {
        if (!fInfo)
        {
            std::string description = "Description of " + fIdentifier;
            
            std::string price = !fLocalizedPrice.empty() ? fLocalizedPrice :  "$ 2.99";
            fInfo.reset(new InAppPurchaseInfo(fIdentifier, description, price));
            
            InAppPurchaseInfoRetrievedNotification notif(*this);
            NotificationCenter::defaultNotificationCenter().postNotification(notif);
            
            // Update status from user defaults
            CCUserDefault* defaults = CCUserDefault::sharedUserDefault();
            _setStatus((EStatus) defaults->getIntegerForKey(_statusKey().c_str(), eNotPurchased));
        }
    }
    
    void
    FakeInAppPurchaseManager::FakeIAP::purchase()
    {
        if (fStatus == eNotPurchased)
        {
            std::string message;
            StringUtils::format(message, "Do you want to purchase %s?", fIdentifier.c_str());
            
            MessageBoxControllerPtr messageBox = MessageBoxController::create("Purchasing", message, "Yes", "No");
            
            CCScene* parent = CCDirector::sharedDirector()->getRunningScene();
            
            messageBox->show(*parent, [this](MessageBoxController* iController, uint32_t iButtonIndex)
             {
                 if (iButtonIndex == 0)
                 {
                     _setStatus(ePurchasing);
                     
                     // TO DO : simulate a latency....
                     
                     _setStatus(ePurchased);

                 }
             });
        }
    }
    
    void
    FakeInAppPurchaseManager::FakeIAP::resetPurchase()
    {
        _setStatus(eNotPurchased);
    }

    std::string
    FakeInAppPurchaseManager::FakeIAP::_statusKey() const
    {
        return identifier() + "_IAPState";
    }

    bool
    FakeInAppPurchaseManager::FakeIAP::_setStatus(EStatus iStatus)
    {
        if (SuperClass::_setStatus(iStatus))
        {
            CCUserDefault::sharedUserDefault()->setIntegerForKey(_statusKey().c_str(), fStatus);
            return true;
        }
        else
        {
            return false;
        }
    }
    
    void
    FakeInAppPurchaseManager::restorePurchases()
    {
        MessageBoxControllerPtr messageBox = MessageBoxController::create("Restoring Purchases", "Do you want to restore your Purchases?", "Yes", "No");
        
        CCScene* parent = CCDirector::sharedDirector()->getRunningScene();
        
        messageBox->show(*parent, [this](MessageBoxController* iController, uint32_t iButtonIndex)
                         {
                            if (iButtonIndex == 0)
                            {
                                InAppPurchasesRestorationNotification preNotif(*this, InAppPurchasesRestorationNotification::eRestoring);
                                NotificationCenter::defaultNotificationCenter().postNotification(preNotif);
                                
                                // Dummy impl: reset all in app purchases
                                for (auto& pair : fIdToIAP)
                                {
                                    auto& iap = pair.second;
                                    iap->resetPurchase();
                                }
                                
                                InAppPurchasesRestorationNotification postNotif(*this, InAppPurchasesRestorationNotification::eRestored);
                                NotificationCenter::defaultNotificationCenter().postNotification(postNotif);
                            }
                         });
        
    }
    
    void
    FakeInAppPurchaseManager::registerInAppPurchase(const InAppPurchase::Identifier& iId)
    {
        assert(inAppPurchase(iId) == nullptr);
        
        std::unique_ptr<FakeIAP> iap(new FakeIAP(iId));
        fIdToIAP[iId] = std::move(iap);
    }
    
    InAppPurchase*
    FakeInAppPurchaseManager::inAppPurchase(const InAppPurchase::Identifier& iId)
    {
        const TIdToIAP::const_iterator it = fIdToIAP.find(iId);
        if (it != fIdToIAP.end())
        {
            return it->second.get();
        }
        else
        {
            return nullptr;
        }
    }
    
    bool
    FakeInAppPurchaseManager::iterateInAppPurchases(const TIterator& iIterator) const
    {
        for (const auto& pair : fIdToIAP)
        {
            auto& iap = pair.second;
            if (!iIterator(*iap))
            {
                return false;
            }
        }
        
        return true;
    }
    
    void
    FakeInAppPurchaseManager::setLocalizedPrice(const InAppPurchase::Identifier& iId, const std::string& iLocalizedPrice)
    {
        FakeInAppPurchaseManager::FakeIAP* iap = static_cast<FakeInAppPurchaseManager::FakeIAP*>(inAppPurchase(iId));
        assert(iap != nullptr);
        
        if (iap != nullptr)
        {
            iap->setLocalizedPrice(iLocalizedPrice);
        }
    }
    
    FakeInAppPurchaseManager::FakeInAppPurchaseManager()
    {}
    
    FakeInAppPurchaseManager::~FakeInAppPurchaseManager()
    {}

NS_CC_END
