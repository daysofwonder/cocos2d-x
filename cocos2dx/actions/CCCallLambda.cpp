//
//  CCCallLambda.cpp
//  SmallWorld2
//
//  Created by Gérald Guyomard on 16/10/12.
//
//

#include "CCCallLambda.h"

NS_CC_BEGIN

CCCallLambda::CCCallLambda()
{}

CCCallLambda * CCCallLambda::create(const std::function<void()>& iLambda)
{
    CCCallLambda *pRet = new CCCallLambda();
    
    if (pRet && pRet->initWithLambda(iLambda))
    {
        pRet->autorelease();
        return pRet;
    }
    
    CC_SAFE_DELETE(pRet);
    return NULL;
}


bool CCCallLambda::initWithLambda(const std::function<void ()> &iLambda) {
    fLambda = iLambda;
    return true;
}

CCObject * CCCallLambda::copyWithZone(CCZone *pZone) {
    CCZone* pNewZone = NULL;
    CCCallLambda* pRet = NULL;
    
    if (pZone && pZone->m_pCopyObject) {
        //in case of being called at sub class
        pRet = (CCCallLambda*) (pZone->m_pCopyObject);
    } else {
        pRet = new CCCallLambda();
        pZone = pNewZone = new CCZone(pRet);
    }
    
    CCActionInstant::copyWithZone(pZone);
    pRet->initWithLambda(fLambda);

    CC_SAFE_DELETE(pNewZone);
    return pRet;
}

void CCCallLambda::update(float time) {
    CC_UNUSED_PARAM(time);
    
    if (fLambda)
    {
        fLambda();
    }
}

NS_CC_END
