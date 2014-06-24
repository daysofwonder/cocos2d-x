//
//  CCMoveAnchorPointTo.cpp
//  SmallWorld2
//
//  Created by Gérald Guyomard on 21/11/12.
//
//

#include "CCMoveAnchorPointTo.h"

NS_CC_BEGIN

CCMoveAnchorPointTo* CCMoveAnchorPointTo::create(float duration, const CCPoint& anchorPoint)
{
    CCMoveAnchorPointTo *pMoveTo = new CCMoveAnchorPointTo();
    pMoveTo->initWithDuration(duration, anchorPoint);
    pMoveTo->autorelease();
    
    return pMoveTo;
}

bool CCMoveAnchorPointTo::initWithDuration(float duration, const CCPoint& anchorPoint)
{
    if (CCActionInterval::initWithDuration(duration))
    {
        m_endAnchorPoint = anchorPoint;
        return true;
    }
    
    return false;
}

CCObject* CCMoveAnchorPointTo::copyWithZone(CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    CCMoveAnchorPointTo* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        //in case of being called at sub class
        pCopy = (CCMoveAnchorPointTo*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CCMoveAnchorPointTo();
        pZone = pNewZone = new CCZone(pCopy);
    }
    
    CCActionInterval::copyWithZone(pZone);
    
    pCopy->initWithDuration(m_fDuration, m_endAnchorPoint);
    
    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

void CCMoveAnchorPointTo::startWithTarget(CCNode *pTarget)
{
    CCActionInterval::startWithTarget(pTarget);
    m_startAnchorPoint = pTarget->getAnchorPoint();
    m_delta = ccpSub(m_endAnchorPoint, m_startAnchorPoint);
}

void CCMoveAnchorPointTo::update(float time)
{
    if (m_pTarget)
    {
        CCPoint anchorPoint(m_startAnchorPoint.x + m_delta.x * time,
                            m_startAnchorPoint.y + m_delta.y * time);
        m_pTarget->setAnchorPoint(anchorPoint);
    }
}

NS_CC_END

