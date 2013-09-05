//
//  HitTestProtocol.cpp
//  SmallWorld2
//
//  Created by Gérald Guyomard on 11/10/12.
//
//

#include "HitTestProtocol.h"

NS_CC_EXT_BEGIN

bool HitTestProtocol::isTouchInsideBounds(CCNode* iNode, const CCPoint& iWorldLocation)
{
    CCPoint touchLocation = iNode->getParent()->convertToNodeSpace(iWorldLocation);
    CCRect bBox= iNode->boundingBox();
    return bBox.containsPoint(touchLocation);
}

NS_CC_EXT_END