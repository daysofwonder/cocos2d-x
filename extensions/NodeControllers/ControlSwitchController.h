//
//  ControlSwitchController.h
//
//  Created by Gérald Guyomard on 14/05/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __ControlSwitchController__
#define __ControlSwitchController__

#include "NodeControllers/NodeController.h"
#include "GUI/CCControlExtension/CCControlSwitch.h"
#include <memory>
#include <functional>

NS_CC_EXT_BEGIN

    class ControlSwitchController : public NodeController
    {
    public:
        DOW_DECLARE_HIERARCHY(ControlSwitchController, NodeController);
        static CCObjectPtr<ControlSwitchController> create(const std::string& iMemberPrefix);
        
        virtual ~ControlSwitchController();
        
        bool isOn() const;
        void setOn(bool isOn, bool iAnimated = false);
        
        typedef std::function<void(ControlSwitchController*)> TValueChangedListener;
        void setValueChangedListener(const TValueChangedListener& iListener) { fValueChangedListener = iListener; }
        
    private:
        ControlSwitchController(const std::string& iMemberPrefix);
        
        virtual void _fillResolvers(Resolvers& iResolvers) override;
        virtual void _didLoad() override;
        
        void _onValueChanged(CCObject*, CCControlEvent);
        
        const std::string     fMemberPrefix;
        
        class Parts;
        std::unique_ptr<Parts> fParts;
        
        CCObjectPtr<CCControlSwitch> fSwitch;
        TValueChangedListener fValueChangedListener;
    };

NS_CC_EXT_END


#endif /* defined(__ControlSwitchController__) */
