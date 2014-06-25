//
//  MessageBoxController.h
//
//  Created by Gérald Guyomard on 12/11/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __MessageBoxController__
#define __MessageBoxController__

#include "NodeControllers/NodeController.h"
#include "cocoa/CCObjectPtr.h"
#include "boost/system/error_code.hpp"

NS_CC_EXT_BEGIN

    class CCControlButton;

    class MessageBoxController;
    typedef CCObjectPtr<MessageBoxController> MessageBoxControllerPtr;
    
    typedef std::function<void (MessageBoxController* iController, uint32_t iButtonIndex)> TMessageBoxCloseCallback;
    
    class MessageBoxController : public NodeController
    {
    public:
        DOW_DECLARE_HIERARCHY(MessageBoxController, NodeController);
        
        static MessageBoxControllerPtr create(const std::string& iTitle, const std::string& iMessage, const std::string& iSingleButton);
        static MessageBoxControllerPtr create(const std::string& iTitle, const std::string& iMessage, const std::string& iFirstButton, const std::string& iSecondButton);

        static MessageBoxControllerPtr createWithErrorCode(const boost::system::error_code& error);
        
        static bool isMessageBoxShown();
        static void keyBackClicked();
        
        // Custom resource
        static MessageBoxControllerPtr createCustomized(const std::string& iResourceFileName, const std::string& iTitle, const std::string& iMessage, const std::string& iSingleButton);
        static MessageBoxControllerPtr createCustomized(const std::string& iResourceFileName, const std::string& iTitle, const std::string& iMessage, const std::string& iFirstButton, const std::string& iSecondButton);

        void show(const TMessageBoxCloseCallback& iCloseCallback = nullptr); // in current scene
        void show(CCNode& iParent);
        void show(CCNode& iParent, const TMessageBoxCloseCallback& iCloseCallback);

    public:
        // GG: Put it public because some code does not compile on Android...
        virtual void _onLeftClicked(CCObject*, CCControlEvent);
        
    protected:
        MessageBoxController();
        virtual ~MessageBoxController();
        
        using SuperClass::_load;
        static MessageBoxControllerPtr _createFromResource(const std::string& iResourceFileName);
        
        void _setup(const std::string& iTitle, const std::string& iMessage, const std::string& iFirstButton, const std::string* iSecondButton = nullptr);
        
        virtual void _fillResolvers(Resolvers& iResolvers) override;
        virtual void _didLoad() override;

        virtual void _didStartToShow() {}
        virtual void _didEndToShow() {}

        virtual void _didStartToHide() {}
        virtual void _didEndToHide() {}

        virtual void _onCenterClicked(CCObject*, CCControlEvent);
        virtual void _onRightClicked(CCObject*, CCControlEvent);

        void _onClickDown(CCObject*, CCControlEvent);
        void _hide(uint32_t iButtonIndex);
        
        TMessageBoxCloseCallback        fCloseCallback;
        
        CCObjectPtr<CCLabelTTF>         fTitleLabel;
        CCObjectPtr<CCLabelTTF>         fMessageLabel;
        
        CCObjectPtr<CCControlButton>    fLeftButton;
        CCObjectPtr<CCControlButton>    fCenterButton;
        CCObjectPtr<CCControlButton>    fRightButton;
        
        static std::string s_ButtonClickedSoundID;
    };
    
    // Notifications
    extern const std::string kMessageBoxControllerWillShowNotifID;
    extern const std::string kMessageBoxControllerDidHideNotifID;
    
NS_CC_EXT_END


#endif /* defined(__MessageBoxController__) */
