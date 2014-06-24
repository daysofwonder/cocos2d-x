//
//  CCMoveAnchorPointTo.h
//
//  Created by Gérald Guyomard on 21/11/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCMoveAnchorPointTo__
#define __CCMoveAnchorPointTo__

#include "cocos2d.h"

NS_CC_BEGIN

class CC_DLL CCMoveAnchorPointTo : public CCActionInterval
{
public:
    /** initializes the action */
    bool initWithDuration(float duration, const CCPoint& position);
    
    virtual CCObject* copyWithZone(CCZone* pZone);
    virtual void startWithTarget(CCNode *pTarget);
    virtual void update(float time);
    
public:
    /** creates the action */
    static CCMoveAnchorPointTo* create(float duration, const CCPoint& anchorPoint);
    
protected:
    CCPoint m_endAnchorPoint;
    CCPoint m_startAnchorPoint;
    CCPoint m_delta;
};

NS_CC_END


#endif /* defined(__CCMoveAnchorPointTo__) */
