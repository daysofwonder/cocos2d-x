//
//  CCLambdaAction.cpp
//
//  Created by Gérald Guyomard on 18/12/12.
//
//

#include "CCLambdaAction.h"

NS_CC_BEGIN

CCLambdaAction::CCLambdaAction()
: fLambda(nullptr)
{}

CCLambdaAction* CCLambdaAction::create(float iDuration, const std::function<void(float)>& iLambda)
{
    CCLambdaAction *pRet = new CCLambdaAction();
    
    if (pRet && pRet->initWithLambda(iDuration, iLambda))
    {
        pRet->autorelease();
        return pRet;
    }
    
    CC_SAFE_DELETE(pRet);
    return NULL;
}


bool CCLambdaAction::initWithLambda(float iDuration, const std::function<void (float)> &iLambda)
{
    if (!initWithDuration(iDuration))
    {
        return false;
    }
    
    fLambda = iLambda;
    return true;
}

CCObject * CCLambdaAction::copyWithZone(CCZone *pZone) {
    CCZone* pNewZone = NULL;
    CCLambdaAction* pRet = NULL;
    
    if (pZone && pZone->m_pCopyObject) {
        //in case of being called at sub class
        pRet = (CCLambdaAction*) (pZone->m_pCopyObject);
    } else {
        pRet = new CCLambdaAction();
        pZone = pNewZone = new CCZone(pRet);
    }
    
    CCActionInterval::copyWithZone(pZone);
    pRet->initWithLambda(m_fDuration, fLambda);
    
    CC_SAFE_DELETE(pNewZone);
    return pRet;
}

void CCLambdaAction::update(float time) {
    CC_UNUSED_PARAM(time);
    
    if (fLambda)
    {
        fLambda(time);
    }
}

NS_CC_END
