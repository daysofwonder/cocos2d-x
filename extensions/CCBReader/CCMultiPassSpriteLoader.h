//
//  CCMultiPassSpriteLoader.h
//
//  Created by Gérald Guyomard on 12/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCMultiPassSpriteLoader__
#define __CCMultiPassSpriteLoader__

#include <cocos2d.h>
#include "CCSpriteLoader.h"
#include "sprite_nodes/CCMultiPassSprite.h"

NS_CC_EXT_BEGIN

class CCMultiPassSpriteLoader : public CCSpriteLoader
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCMultiPassSpriteLoader, loader);
    
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CCMultiPassSprite);
    
    virtual void onHandlePropTypeInteger(CCNode * pNode, CCNode * pParent, const char* pPropertyName, int pInteger, CCBReader * pCCBReader) override;
};

NS_CC_EXT_END

#endif // __CCMultiPassSpriteLoader__
