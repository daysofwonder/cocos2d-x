//
//  CCSmartLayerLoader.cpp
//
//  Created by Gérald Guyomard on 6/25/14.
// Copyright (c) 2013 Days of Wonder Inc.
//

#include "CCSmartLayerLoader.h"

NS_CC_EXT_BEGIN

void
CCSmartLayerLoader::onHandlePropTypeCheck(CCNode * pNode, CCNode * pParent, const char * pPropertyName, bool pCheck, CCBReader * pCCBReader)
{
    CCSmartLayer* node = static_cast<CCSmartLayer*>(pNode);
    assert(dynamic_cast<CCSmartLayer*>(pNode) != nullptr);
    
    if (::strcmp(pPropertyName, "clipToBounds") == 0)
    {
        node->setClipToBounds(pCheck);
    }
    else if (::strcmp(pPropertyName, "useRenderTexture") == 0)
    {
        node->useRenderTexture(pCheck);
    }
    else if (::strcmp(pPropertyName, "discardSelfInRenderTexture") == 0)
    {
        node->setDiscardSelfInRenderTexture(pCheck);
    }
    else if (::strcmp(pPropertyName, "saveOpacitiesAfterLoad") == 0)
    {
        node->setSaveOpacitiesAfterLoad(pCheck);
    }
    else if (::strcmp(pPropertyName, "shouldDiscardNonDescendantNativeNodes") == 0)
    {
        node->setShouldDiscardNonDescendantNativeNodes(pCheck);
    }
    else if (::strcmp(pPropertyName, "isEnabled") == 0)
    {
		node->setIsEnabled(pCheck);
    }
    else if (::strcmp(pPropertyName, "isClickThrough") == 0)
    {
		node->setIsClickThrough(pCheck);
    }
    else
    {
        CCLayerLoader::onHandlePropTypeCheck(pNode, pParent, pPropertyName, pCheck, pCCBReader);
    }
}

void
CCSmartLayerLoader::onHandlePropTypeString(CCNode * pNode, CCNode * pParent, const char* pPropertyName, const char * pString, CCBReader * pCCBReader)
{
    CCSmartLayer* layer = static_cast<CCSmartLayer*>(pNode);
    assert(dynamic_cast<CCSmartLayer*>(pNode) != nullptr);
    
    if (::strcmp(pPropertyName, "tooltipText") == 0)
    {
        layer->setTooltipText(pString);
    }
    else if (::strcmp(pPropertyName, "tooltipLocalizedStringKey") == 0)
    {
        layer->setTooltipLocalizedTextKey(pString);
    }
    else
    {
        CCLayerLoader::onHandlePropTypeString(pNode, pParent, pPropertyName, pString, pCCBReader);
    }
}

NS_CC_EXT_END
