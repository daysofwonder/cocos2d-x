//
//  CCEnhancedLabelTTFLoader.h
//
//  Created by Gérald Guyomard on 6/24/14.
// Copyright (c) 2013 Days of Wonder Inc.
//

#ifndef __CCEnhancedLabelTTFLoader__
#define __CCEnhancedLabelTTFLoader__

#include <cocos2d.h>
#include "CCLabelTTFLoader.h"
#include "label_nodes/CCEnhancedLabelTTF.h"

NS_CC_EXT_BEGIN

class CCEnhancedLabelTTFLoader : public CCLabelTTFLoader
{
public:
    DOW_DECLARE_HIERARCHY(CCEnhancedLabelTTFLoader, CCLabelTTFLoader);
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCEnhancedLabelTTFLoader, loader);
    
    virtual void onHandlePropTypeFloat(CCNode * pNode, CCNode * pParent, const char* pPropertyName, float pFloat, CCBReader * pCCBReader) override;
    virtual void onHandlePropTypeString(CCNode * pNode, CCNode * pParent, const char* pPropertyName, const char * pString, CCBReader * pCCBReader) override;
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(cocos2d::CCEnhancedLabelTTF);
    
};

NS_CC_EXT_END

#endif /* defined(__CCEnhancedLabelTTFLoader__) */
