//
//  SelectorResolver.h
//
//  Created by Gérald Guyomard on 28/09/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __SelectorResolver__
#define __SelectorResolver__

#include "CCBReader/CCBSelectorResolver.h"
#include <map>
#include <memory>

NS_CC_EXT_BEGIN

    class SelectorResolver : public CCBSelectorResolver
    {
    public:        
        virtual SEL_MenuHandler onResolveCCBCCMenuItemSelector(CCObject * pTarget, const char* pSelectorName) override;
        virtual SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName) override;

        void addMenuItemSelector(const std::string& iSelectorName, const SEL_MenuHandler& iHandler);
        void addControlSelector(const std::string& iSelectorName, const SEL_CCControlHandler& iHandler);
        
    private:        
        typedef std::map<std::string, SEL_MenuHandler> TMenuItemSelectorNameToHandler;
        TMenuItemSelectorNameToHandler fMenuItemSelectorNameToHandler;
        
        typedef std::map<std::string, SEL_CCControlHandler> TControlSelectorNameToHandler;
        TControlSelectorNameToHandler fControlSelectorNameToHandler;
    };
    
#define DOWCC_ADD_MENU_ITEM_SELECTOR(resolver, Class, methodName) \
    { \
        (resolver).addMenuItemSelector(#methodName, (SEL_MenuHandler) &(Class::methodName)); \
    } \

#define DOWCC_ADD_CONTROL_SELECTOR(resolver, Class, methodName) \
    { \
        (resolver).addControlSelector(#methodName, (SEL_CCControlHandler) &Class::methodName); \
    } \

NS_CC_EXT_END

#endif /* defined(__SelectorResolver__) */
