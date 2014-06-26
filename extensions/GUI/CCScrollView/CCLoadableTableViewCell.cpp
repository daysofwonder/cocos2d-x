//
//  CCLoadableTableViewCell.cpp
//
//  Created by Gérald Guyomard on 21/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCLoadableTableViewCell.h"

NS_CC_EXT_BEGIN

    CCLoadableTableViewCell::CCLoadableTableViewCell()
    {}
    
    CCObject*
    CCLoadableTableViewCell::owner()
    {
        return this;
    }
    
    void
    CCLoadableTableViewCell::_didLoad()
    {
        
    }
    
    void
    CCLoadableTableViewCell::_postLoad(CCNode* iRoot)
    {
        fRootNode = iRoot;
        fRootNode->ignoreAnchorPointForPosition(true);
        fRootNode->setPosition(CCPoint(0, 0));
        
        addChild(fRootNode);
        
        setContentSize(fRootNode->getContentSize());
        
        _didLoad();
    }

NS_CC_EXT_END

