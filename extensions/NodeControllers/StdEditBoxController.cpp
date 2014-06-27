//
//  StdEditBoxController.cpp
//
//  Created by Gérald Guyomard on 11/01/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "NodeControllers/StdEditBoxController.h"

NS_CC_EXT_BEGIN

    StdEditBoxController::StdEditBoxController()
    {}
    

    StdEditBoxControllerPtr
    StdEditBoxController::createFromResource()
    {
        StdEditBoxControllerPtr controller;
        controller << new StdEditBoxController;
        
        controller->_load("StdEditBoxController");
        
        return controller;
    }
    
    StdEditBoxControllerPtr
    StdEditBoxController::createFromResource(const CCSize& iSize)
    {
        StdEditBoxControllerPtr controller = createFromResource();
        controller->setSize(iSize);
        return controller;
    }

    
    void
    StdEditBoxController::_didLoad()
    {
        SuperClass::_didLoad();
        
        assert(fEditBox != nullptr);
        
        fEditBox->setFontColor(ccBLACK);
    }

    
    CCObjectPtr<EditBoxController>
    StdEditBoxControllerHost::_createEditBoxController(const CCSize& iSize)
    {
        return StdEditBoxController::createFromResource(iSize);
    }

NS_CC_EXT_END
