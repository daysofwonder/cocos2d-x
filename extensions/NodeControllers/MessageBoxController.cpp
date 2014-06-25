//
//  MessageBoxController.cpp
//
//  Created by Gérald Guyomard on 12/11/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "NodeControllers/MessageBoxController.h"

#include "layers_scenes_transitions_nodes/CCSmartLayer.h"


#include "DOWFoundation/SoundManager.h"
//#include "Sounds.h"
#include <algorithm>

#include "DOWFoundation/NotificationCenter.h"
#include "DOWFoundation/Logger.h"

#include "GUI/CCControlExtension/CCControlButton.h"

using namespace DOW;

NS_CC_EXT_BEGIN
   
const std::string kMessageBoxControllerWillShowNotifID = "MessageBoxControllerWillShow";
const std::string kMessageBoxControllerDidHideNotifID = "MessageBoxControllerDidHide";

MessageBoxController::MessageBoxController()
{}
    
MessageBoxController::~MessageBoxController()
{}

MessageBoxControllerPtr
MessageBoxController::_createFromResource(const std::string& iResourceFileName)
{
    MessageBoxControllerPtr controller;
    controller << new MessageBoxController;
    
    controller->_load(iResourceFileName);
    return controller;
}
    
static const std::string kDefaultResourceFileName = "MessageBoxController";
    
MessageBoxControllerPtr
MessageBoxController::create(const std::string& iTitle, const std::string& iMessage, const std::string& iSingleButton)
{
    MessageBoxControllerPtr controller = _createFromResource(kDefaultResourceFileName);
    controller->_setup(iTitle, iMessage, iSingleButton);
    
    return controller;
}
    
MessageBoxControllerPtr
MessageBoxController::create(const std::string& iTitle, const std::string& iMessage, const std::string& iFirstButton, const std::string& iSecondButton)
{
    MessageBoxControllerPtr controller = _createFromResource(kDefaultResourceFileName);
    controller->_setup(iTitle, iMessage, iFirstButton, &iSecondButton);
    
    return controller;    
}
    
MessageBoxControllerPtr
MessageBoxController::createWithErrorCode(const boost::system::error_code& error)
{
    MessageBoxControllerPtr controller = _createFromResource(kDefaultResourceFileName);
    controller->_setup("Unexpected Error", error.message(), CCLocalizedStdString("MessageBox_OKButton"));
    return controller;

}


MessageBoxControllerPtr
MessageBoxController::createCustomized(const std::string& iResourceFileName, const std::string& iTitle, const std::string& iMessage, const std::string& iSingleButton)
{
    MessageBoxControllerPtr controller = _createFromResource(iResourceFileName);
    controller->_setup(iTitle, iMessage, iSingleButton);
    
    return controller;
}

MessageBoxControllerPtr
MessageBoxController::createCustomized(const std::string& iResourceFileName, const std::string& iTitle, const std::string& iMessage, const std::string& iFirstButton, const std::string& iSecondButton)
{
    MessageBoxControllerPtr controller = _createFromResource(iResourceFileName);
    controller->_setup(iTitle, iMessage, iFirstButton, &iSecondButton);
    
    return controller;    
}
    
void
MessageBoxController::_fillResolvers(Resolvers& iResolvers)
{
    SelectorResolver& resolver = iResolvers.selectorResolver();
    
    DOWCC_ADD_CONTROL_SELECTOR(resolver, ThisClass, _onLeftClicked);
    DOWCC_ADD_CONTROL_SELECTOR(resolver, ThisClass, _onCenterClicked);
    DOWCC_ADD_CONTROL_SELECTOR(resolver, ThisClass, _onRightClicked);
    
    MemberVariableAssigner& assigner =  iResolvers.memberVariableAssigner();
    
    assigner.addMember("titleLabel", fTitleLabel);
    assigner.addMember("messageLabel", fMessageLabel);

    assigner.addMember("leftButton", fLeftButton);
    assigner.addMember("centerButton", fCenterButton);
    assigner.addMember("rightButton", fRightButton);
}
    
void
MessageBoxController::_didLoad()
{
    SuperClass::_didLoad();
    
    CCSmartLayer* layer = dynamic_cast<CCSmartLayer*>(rootNode());
    if (layer != nullptr)
    {
        layer->setShouldDiscardNonDescendantNativeNodes(true);
    }
}
    
void
MessageBoxController::_setup(const std::string& iTitle, const std::string& iMessage, const std::string& iFirstButton, const std::string* iSecondButton)
{
    dowlog::myLog() << "Preparing dialog box - " << iTitle << std::endl;
    dowlog::myLog() << " dialog box message - " << iMessage << std::endl;

    assert(fTitleLabel != nullptr);
    fTitleLabel->setString(iTitle.c_str());
    
    assert(fMessageLabel != nullptr);
    fMessageLabel->setString(iMessage.c_str());
        
    if (iSecondButton != nullptr)
    {
        if (fCenterButton != nullptr)
        {
            fCenterButton->removeFromParentAndCleanup(true);
            fCenterButton = nullptr;            
        }
        
        assert(fLeftButton != nullptr);
        fLeftButton->setTitle(iFirstButton);
        fLeftButton->addTargetWithActionForControlEvents(this, (SEL_CCControlHandler) &ThisClass::_onClickDown, CCControlEventTouchDown);
        
        assert(fRightButton != nullptr);
        fRightButton->setTitle(*iSecondButton);
        fRightButton->addTargetWithActionForControlEvents(this, (SEL_CCControlHandler) &ThisClass::_onClickDown, CCControlEventTouchDown);

    }
    else
    {
        if (fLeftButton != nullptr)
        {
            fLeftButton->removeFromParentAndCleanup(true);
            fLeftButton = nullptr;            
        }
        
        if (fRightButton != nullptr)
        {
            fRightButton->removeFromParentAndCleanup(true);
            fRightButton = nullptr;            
        }
        
        assert(fCenterButton != nullptr);
        fCenterButton->setTitle(iFirstButton);
        fCenterButton->addTargetWithActionForControlEvents(this, (SEL_CCControlHandler) &ThisClass::_onClickDown, CCControlEventTouchDown);

    }
}
    
void
MessageBoxController::_onLeftClicked(CCObject*, CCControlEvent)
{
    _hide(0);
}
    
void
MessageBoxController::_onCenterClicked(CCObject*, CCControlEvent)
{
    _hide(0);
}
    
void
MessageBoxController::_onRightClicked(CCObject*, CCControlEvent)
{
    _hide(1);
}

std::string MessageBoxController::s_ButtonClickedSoundID = "ButtonClick";

void
MessageBoxController::_onClickDown(CCObject*, CCControlEvent)
{
    SoundManager::instance().play(s_ButtonClickedSoundID);
}
    
static std::vector<MessageBoxControllerPtr>* s_ShownControllers = nullptr;
    
void
MessageBoxController::show(CCNode& iParent)
{
    MessageBoxControllerPtr self = this;
    
    if (s_ShownControllers == nullptr)
    {
        s_ShownControllers = new std::vector<MessageBoxControllerPtr>;
    }
    
    if (std::find(s_ShownControllers->begin(), s_ShownControllers->end(), self) == s_ShownControllers->end())
    {
        s_ShownControllers->push_back(this);
        
        const CCSize& parentSize = iParent.getContentSize();
        
        CCNode* root = rootNode();
        
        // Resize the root to the size of the parent
        root->setContentSize(parentSize);
        
        iParent.addChild(root);
        
        CCNode::setOpacity(root, 0);
        
        auto* fadeInAction = CCFadeIn::create(0.25f);
        CCCallLambda* completion = CCCallLambda::create(
                        [this]()
                        {
                            _didEndToShow();
                        });
        
        
        NotificationCenter::defaultNotificationCenter().postNotification(kMessageBoxControllerWillShowNotifID, this);
        
        _didStartToShow();
        root->runAction(CCSequence::createWithTwoActions(fadeInAction, completion));
    }
}
    
void
MessageBoxController::show(const TMessageBoxCloseCallback& iCloseCallback)
{
    fCloseCallback = iCloseCallback;
    
    CCNode* node = CCDirector::sharedDirector()->getRunningScene();
    assert(node != nullptr);
    show(*node);
}
    
void
MessageBoxController::show(CCNode& iParent, const TMessageBoxCloseCallback& iCloseCallback)
{
    fCloseCallback = iCloseCallback;
    
    show(iParent);
}

void
MessageBoxController::_hide(uint32_t iButtonIndex)
{
    dowlog::myLog() << "Closing dialog box due to button " << iButtonIndex << std::endl;
    CCFadeOut* fadeOut = CCFadeOut::create(0.25f);
    CCCallLambda* completion = CCCallLambda::create([this, iButtonIndex]
                                                    {
                                                        MessageBoxControllerPtr sentinel = this;
                                                        
                                                        NotificationCenter::defaultNotificationCenter().postNotification(kMessageBoxControllerDidHideNotifID, this);

                                                        _didEndToHide();
                                                        
                                                        rootNode()->removeFromParentAndCleanup(true);
                                                        
                                                        if (fCloseCallback != nullptr)
                                                        {
                                                            fCloseCallback(this, iButtonIndex);
                                                        }
                                                        
                                                        assert(s_ShownControllers != nullptr);
                                                        
                                                        std::vector<MessageBoxControllerPtr>::iterator it = std::find(s_ShownControllers->begin(), s_ShownControllers->end(), this);
                                                        
                                                        if (it != s_ShownControllers->end())
                                                        {
                                                            s_ShownControllers->erase(it);
                                                            if (s_ShownControllers->empty())
                                                            {
                                                                delete s_ShownControllers;
                                                                s_ShownControllers = nullptr;
                                                            }
                                                        }
                                                        else
                                                        {
                                                            assert(false);
                                                        }                                                        
                                                    });
    
    _didStartToHide();
    
    CCSequence* action = CCSequence::createWithTwoActions(fadeOut, completion);
    rootNode()->runAction(action);
}
    
bool
MessageBoxController::isMessageBoxShown() {
    return (s_ShownControllers != nullptr) && !s_ShownControllers->empty();
}
    
void
MessageBoxController::keyBackClicked() {
    if (isMessageBoxShown())
    {
        MessageBoxControllerPtr topMessageBox = s_ShownControllers->back();
        topMessageBox->_hide(0);
    }
}
    
NS_CC_EXT_END
