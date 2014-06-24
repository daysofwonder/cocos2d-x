//
//  CCMultiPassSpriteLoader.cpp
//
//  Created by Gérald Guyomard on 12/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//

#include "CCMultiPassSpriteLoader.h"

NS_CC_EXT_BEGIN

void
CCMultiPassSpriteLoader::onHandlePropTypeInteger(CCNode * pNode, CCNode * pParent, const char* pPropertyName, int pInteger, CCBReader * pCCBReader)
{
    CCMultiPassSprite* node = static_cast<CCMultiPassSprite*>(pNode);
    assert(dynamic_cast<CCMultiPassSprite*>(pNode) != nullptr);
    
    if (::strcmp(pPropertyName, "nbRenderingPasses") == 0)
    {
        assert(pInteger >= 0);
        node->setNbRenderingPasses((uint32_t) pInteger);
    }
    else
    {
        CCSpriteLoader::onHandlePropTypeInteger(pNode, pParent, pPropertyName, pInteger, pCCBReader);
    }
}


NS_CC_EXT_END