//
//  HitTestProtocol.cpp
//
//  Created by Gérald Guyomard on 11/10/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "touch_dispatcher/HitTestProtocol.h"

NS_CC_BEGIN

bool HitTestProtocol::isTouchInsideBounds(CCNode* iNode, const CCPoint& iWorldLocation)
{
    CCPoint touchLocation = iNode->getParent()->convertToNodeSpace(iWorldLocation);
    CCRect bBox= iNode->boundingBox();
    return bBox.containsPoint(touchLocation);
}

NS_CC_END