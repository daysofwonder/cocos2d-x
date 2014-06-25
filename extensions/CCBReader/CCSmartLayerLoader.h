//
//  CCSmartLayerLoader.h
//
//  Created by Gérald Guyomard on 6/25/14.
// Copyright (c) 2013 Days of Wonder Inc.
//

#ifndef __CCSmartLayerLoader__
#define __CCSmartLayerLoader__

#include "CCLayerLoader.h"
#include "layers_scenes_transitions_nodes/CCSmartLayer.h"

NS_CC_EXT_BEGIN

class CCSmartLayerLoader : public CCLayerLoader
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCSmartLayerLoader, loader);
    
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CCSmartLayer);
    
    virtual void onHandlePropTypeCheck(CCNode * pNode, CCNode * pParent, const char * pPropertyName, bool pCheck, CCBReader * pCCBReader) override;
    virtual void onHandlePropTypeString(CCNode * pNode, CCNode * pParent, const char* pPropertyName, const char * pString, CCBReader * pCCBReader) override;
};

NS_CC_EXT_END

#endif /* defined(__CCSmartLayerLoader__) */
