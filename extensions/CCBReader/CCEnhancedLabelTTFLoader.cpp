//
//  CCEnhancedLabelTTFLoader.cpp
//
//  Created by Gérald Guyomard on 6/24/14.
// Copyright (c) 2013 Days of Wonder Inc.
//

#include "CCEnhancedLabelTTFLoader.h"

NS_CC_EXT_BEGIN

void
CCEnhancedLabelTTFLoader::onHandlePropTypeFloat(CCNode * pNode, CCNode * pParent, const char* pPropertyName, float pFloat, CCBReader * pCCBReader)
{
    CCEnhancedLabelTTF* label = static_cast<CCEnhancedLabelTTF*>(pNode);
    assert(dynamic_cast<CCEnhancedLabelTTF*>(pNode) != nullptr);
    
    if (::strcmp(pPropertyName, "outlineThickness") == 0)
    {
        label->setOutlineThickness(pFloat);
    }
    else
    {
        SuperClass::onHandlePropTypeFloat(pNode, pParent, pPropertyName, pFloat, pCCBReader);
    }
}

void
CCEnhancedLabelTTFLoader::onHandlePropTypeString(CCNode * pNode, CCNode * pParent, const char* pPropertyName, const char * pString, CCBReader * pCCBReader)
{
    CCEnhancedLabelTTF* label = static_cast<CCEnhancedLabelTTF*>(pNode);
    assert(dynamic_cast<CCEnhancedLabelTTF*>(pNode) != nullptr);
    
    if (::strcmp(pPropertyName, "shadowOffset") == 0)
    {
        label->setShadowOffsetFromPlainText(pString);
    }
    else if (::strcmp(pPropertyName, "shadowColor") == 0)
    {
        label->setShadowColorFromPlainText(pString);
    }
    else if (::strcmp(pPropertyName, "outlineColor") == 0)
    {
        label->setOutlineColorFromPlainText(pString);
    }
    else if (::strcmp(pPropertyName, "localizedStringKey") == 0)
    {
        label->setLocalizedStringKey(pString);
    }
    else
    {
        SuperClass::onHandlePropTypeString(pNode, pParent, pPropertyName, pString, pCCBReader);
    }
}

NS_CC_EXT_END
