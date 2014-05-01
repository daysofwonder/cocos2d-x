//
//  HitTestProtocol.h
//  SmallWorld2
//
//  Created by Gérald Guyomard on 11/10/12.
//
//

#ifndef __HitTestProtocol__
#define __HitTestProtocol__

#include <cocos2d.h>
#include "ExtensionMacros.h"

NS_CC_EXT_BEGIN

class HitTestProtocol
{
public:
    
    // When enabled you receive touch (if click through property not set though) and mouse over
    virtual bool isEnabled() = 0; // Should be const but cocos2d-x is not that const friendly
    virtual void setIsEnabled(bool iEnabled) = 0;
    
    // If true, then no click are propagated throught the entire hierarchy where this node is root
    virtual bool isClickThrough()  { return false; }

    virtual bool isTouchInside(const CCPoint& iWorldLocation) = 0;

    // Default implementation tests against the bounding box
    static bool isTouchInsideBounds(CCNode* iNode, const CCPoint& iWorldLocation);
    
    // Pre processing of touches before dispatching to children
    // Only relevant if used when CCSceneGraphTouchDispatcher installed
    virtual bool ccPreTouchBegan(CCTouch *pTouch, CCEvent *pEvent) {CC_UNUSED_PARAM(pTouch); CC_UNUSED_PARAM(pEvent); return false;}
    virtual bool ccPreTouchMoved(CCTouch *pTouch, CCEvent *pEvent) {CC_UNUSED_PARAM(pTouch); CC_UNUSED_PARAM(pEvent); return false;}
    
    virtual bool ccPreTouchEnded(CCTouch *pTouch, CCEvent *pEvent) {CC_UNUSED_PARAM(pTouch); CC_UNUSED_PARAM(pEvent); return  false; }
    virtual bool ccPreTouchCancelled(CCTouch *pTouch, CCEvent *pEvent) {CC_UNUSED_PARAM(pTouch); CC_UNUSED_PARAM(pEvent); return false; }

protected:
    ~HitTestProtocol() {}
    
};

NS_CC_EXT_END

#endif /* defined(__HitTestProtocol__) */
