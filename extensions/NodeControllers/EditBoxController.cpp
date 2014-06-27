//
//  EditBoxController.cpp
//
//  Created by Gérald Guyomard on 11/01/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "NodeControllers/EditBoxController.h"

NS_CC_EXT_BEGIN

    EditBoxController::EditBoxController()
    {}
    
    EditBoxController::~EditBoxController()
    {}

    void
    EditBoxController::_fillResolvers(Resolvers& ioResolvers)
    {
        MemberVariableAssigner& assigner =  ioResolvers.memberVariableAssigner();
        
        assigner.addMember("editBoxBackground", fEditBoxBackground);
        assigner.addMember("label", fLabel);
    }
    
    void
    EditBoxController::replaceBackgroundTexture(const std::string& iFileName)
    {
        assert(fEditBoxBackground != nullptr);
        
        CCRect capsInset = fEditBoxBackground->getCapInsets();
        fEditBoxBackground->removeAllChildrenWithCleanup(true);
        
        CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(iFileName.c_str());
        if (frame != nullptr)
        {
            fEditBoxBackground = CCScale9Sprite::createWithSpriteFrame(frame, capsInset);
        }
        else
        {
            fEditBoxBackground = CCScale9Sprite::create(iFileName.c_str(), CCRect(), capsInset);
        }
    }
    
    void
    EditBoxController::_prepareEditBoxBackground()
    {
        fEditBoxBackground->removeFromParentAndCleanup(false);
        fEditBoxBackground->ignoreAnchorPointForPosition(false);
        fEditBoxBackground->setAnchorPoint(ccp(0.5f, 0.5f));        
    }
    
    void
    EditBoxController::setup(CCNode* iRootNode, CCScale9Sprite* iBackground, CCLabelTTF* iLabel)
    {
        fRootNode = iRootNode;
        fEditBoxBackground = iBackground;
        fLabel = iLabel;
        
        _didLoad();
    }
    
    void
    EditBoxController::fillResolvers(const std::string& iPrefixMember, Resolvers& iResolvers)
    {
        auto& assigner  = iResolvers.memberVariableAssigner();
        
        std::string memberName;
        
        memberName = iPrefixMember + "_EditBoxRootNode";
        assigner.addMember(memberName, fRootNode);
        
        memberName = iPrefixMember + "_EditBoxBackground";
        assigner.addMember(memberName, fEditBoxBackground);
        
        memberName = iPrefixMember + "_EditBoxLabel";
        assigner.addMember(memberName, fLabel);
    }

    void
    EditBoxController::_didLoad()
    {
        SuperClass::_didLoad();
        
        assert(fEditBoxBackground != nullptr);
        
        // Transfer the background to create the edit box
        _prepareEditBoxBackground();
        
        CCNode* root = rootNode();
        const CCSize& size = root->getContentSize();
        
        fEditBox = CCEditBox::create(size, fEditBoxBackground);
        
        fEditBoxBackground = nullptr;
                
        fEditBox->ignoreAnchorPointForPosition(true);
        fEditBox->setDoAnimationWhenKeyboardMove(false);

        if (fLabel != nullptr)
        {
            CCRect inputBounds = fLabel->boundingBox();
            CCRect inputInsets;
            inputInsets.origin = inputBounds.origin;
            inputInsets.size.width = size.width - inputBounds.size.width;
            inputInsets.size.height = size.height - inputBounds.size.height;
            if (inputInsets.size.height<0) {
                inputInsets.size.height = 0; //JFS happens in play with buddies "Filter"
            }
			fEditBox->setInputInsets(inputInsets);
            fEditBox->setLabel(fLabel);

            fLabel = nullptr;
        }
        
        root->addChild(fEditBox);
    }

    void
    EditBoxController::setSize(const CCSize& iSize)
    {
        rootNode()->setContentSize(iSize);
        fEditBox->setContentSize(iSize);
        fEditBox->setPreferredSize(iSize);
    }
    
    EditBoxControllerHost::~EditBoxControllerHost()
    {
        if (fEditBoxController != nullptr)
        {
            fEditBoxController->editBox()->setDelegate(nullptr);
        }
    }
    
    void
    EditBoxControllerHost::setup(CCEditBoxDelegate* iDelegate, const std::string& iDefaultValue, const std::string& iPlaceHolderValue)
    {
        assert(fHost != nullptr);
        fEditBoxController = _createEditBoxController(fHost->getContentSize());

        fHost->addChild(fEditBoxController->rootNode());
        
        CCEditBox* editBox = fEditBoxController->editBox();
        editBox->setDelegate(iDelegate);
        editBox->setText(iDefaultValue.c_str());
        editBox->setPlaceHolder(iPlaceHolderValue.c_str());
    }

NS_CC_EXT_END
