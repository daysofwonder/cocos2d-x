//
//  SelectorResolver.cpp
//
//  Created by Gérald Guyomard on 28/09/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "SelectorResolver.h"

NS_CC_EXT_BEGIN

    
SEL_MenuHandler
SelectorResolver::onResolveCCBCCMenuItemSelector(CCObject * pTarget, const char* pSelectorName)
{
    std::string key(pSelectorName);
    const TMenuItemSelectorNameToHandler::const_iterator it = fMenuItemSelectorNameToHandler.find(key);
    if (it != fMenuItemSelectorNameToHandler.end())
    {
        return it->second;
    }
        
    return nullptr;
}
    
SEL_CCControlHandler
SelectorResolver::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    std::string key(pSelectorName);
    const TControlSelectorNameToHandler::const_iterator it = fControlSelectorNameToHandler.find(key);
    if (it != fControlSelectorNameToHandler.end())
    {
        return it->second;
    }
    
    return nullptr;
}

void
SelectorResolver::addMenuItemSelector(const std::string& iSelectorName, const SEL_MenuHandler& iHandler)
{
    fMenuItemSelectorNameToHandler[iSelectorName] = iHandler;
}
    
void
SelectorResolver::addControlSelector(const std::string& iSelectorName, const SEL_CCControlHandler& iHandler)
{
    fControlSelectorNameToHandler[iSelectorName] = iHandler;
}

NS_CC_EXT_END
