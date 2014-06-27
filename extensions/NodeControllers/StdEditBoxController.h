//
//  StdEditBoxController.h
//
//  Created by Gérald Guyomard on 11/01/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __StdEditBoxController__
#define __StdEditBoxController__

#include "NodeControllers/EditBoxController.h"

NS_CC_EXT_BEGIN
    class CCScale9Sprite;

    class StdEditBoxController;
    typedef CCObjectPtr<StdEditBoxController> StdEditBoxControllerPtr;
    
    class StdEditBoxController : public EditBoxController
    {
    public:
        DOW_DECLARE_HIERARCHY(StdEditBoxController, EditBoxController);
        
        static StdEditBoxControllerPtr createFromResource();
        static StdEditBoxControllerPtr createFromResource(const CCSize& iSize);
                
    protected:
        StdEditBoxController();
        virtual void _didLoad() override;
    };
    
    class StdEditBoxControllerHost : public EditBoxControllerHost
    {
    public:
        StdEditBoxControllerHost() {}
        
    private:
        virtual CCObjectPtr<EditBoxController> _createEditBoxController(const CCSize& iSize) override;
    };

NS_CC_EXT_END

#endif /* defined(__StdEditBoxController__) */
