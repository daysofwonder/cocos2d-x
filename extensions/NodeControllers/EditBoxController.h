//
//  EditBoxController.h
//
//  Created by Gérald Guyomard on 11/01/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __EditBoxController__
#define __EditBoxController__

#include "NodeControllers/NodeController.h"
#include "cocoa/CCObjectPtr.h"
#include "GUI/CCEditBox/CCEditBox.h"

NS_CC_EXT_BEGIN

    class CCScale9Sprite;
    class EditBoxController;
    typedef CCObjectPtr<EditBoxController> EditBoxControllerPtr;
    
    class EditBoxController : public NodeController
    {
    public:
        DOW_DECLARE_HIERARCHY(EditBoxController, NodeController);
        EditBoxController();
        
        void setup(CCNode* iRootNode, CCScale9Sprite* iBackground, CCLabelTTF* iLabel = nullptr);
        
        void fillResolvers(const std::string& iPrefixMember, Resolvers& iResolvers);
        
        void replaceBackgroundTexture(const std::string& iFileName); // To be done before _didLoad
        
        CCEditBox* editBox() { return fEditBox; }
        
        void setSize(const CCSize& iSize);
        
    protected:
        virtual ~EditBoxController();
        
        virtual void _fillResolvers(Resolvers& ioResolvers) override;
        virtual void _didLoad() override;
        virtual void _prepareEditBoxBackground();
        
        CCObjectPtr<CCScale9Sprite> fEditBoxBackground;
        CCObjectPtr<CCLabelTTF>     fLabel;
        CCObjectPtr<CCEditBox>      fEditBox;
    };
    
    class EditBoxControllerHost
    {
    public:
        virtual ~EditBoxControllerHost();
        
        void setup(CCEditBoxDelegate* iDelegate = nullptr, const std::string& iDefaultValue = "", const std::string& iPlaceHolderValue = "");
        
        CCObjectPtr<CCNode>                 fHost;
        CCObjectPtr<EditBoxController>      fEditBoxController;
        
    protected:
        virtual CCObjectPtr<EditBoxController> _createEditBoxController(const CCSize& iSize) = 0;
    };

NS_CC_EXT_END

#endif /* defined(__EditBoxController__) */
