//
//  CCTableViewLoader.h
//
//  Created by Gérald Guyomard on 13/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCTableViewLoader__
#define __CCTableViewLoader__

#include "cocos2d.h"
#include "CCScrollViewLoader.h"
#include "GUI/CCScrollView/CCTableView.h"

NS_CC_EXT_BEGIN

class CCTableViewLoader : public CCScrollViewLoader
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCTableViewLoader, loader);
    
protected:
    virtual CCTableView* createCCNode(cocos2d::CCNode * pParent, CCBReader * pCCBReader) override;

};

NS_CC_EXT_END


#endif /* defined(__CCTableViewLoader__) */
