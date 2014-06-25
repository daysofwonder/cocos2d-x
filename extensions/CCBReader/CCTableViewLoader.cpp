//
//  CCTableViewLoader.cpp
//
//  Created by Gérald Guyomard on 13/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCTableViewLoader.h"

NS_CC_EXT_BEGIN

CCTableView*
CCTableViewLoader::createCCNode(cocos2d::CCNode * pParent, CCBReader * pCCBReader)
{    
    CCTableView* view = new CCTableView;
    view->initWithViewSize(CCSizeMake(200, 200), NULL);
    
    view->autorelease();
    return view;
}

NS_CC_EXT_END