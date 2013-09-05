//
//  CCSceneGraphTouchDispatcher.h
//
//  Created by Gérald Guyomard on 11/10/12.
//
//

#ifndef __CCSceneGraphTouchDispatcher__
#define __CCSceneGraphTouchDispatcher__

#include <cocos2d.h>
#include "touch_dispatcher/CCTouchDispatcher.h"
#include "ExtensionMacros.h"

NS_CC_EXT_BEGIN

class HitTestProtocol;
class CCSceneGraphTargetedTouchHandler;

class CCSceneGraphTouchDispatcher : public CCTouchDispatcher
{
public:
    
    static CCSceneGraphTouchDispatcher* create();
    
    void dispatchTouches(CCSet *pTouches, CCEvent *pEvent, unsigned int uIndex);
    
    virtual void touchesBegan(CCSet* touches, CCEvent* pEvent);
    virtual void touchesMoved(CCSet* touches, CCEvent* pEvent);
    virtual void touchesEnded(CCSet* touches, CCEvent* pEvent);
    virtual void touchesCancelled(CCSet* touches, CCEvent* pEvent);
    
    virtual CCTouchDelegate* simulateTouchDown(CCTouch* iTouch);

protected:
    CCSceneGraphTouchDispatcher();

    virtual CCTouchHandler* createTargetedTouchHandler(CCTouchDelegate *pDelegate, int nPriority, bool bSwallowsTouches);
    virtual void forceRemoveDelegate(CCTouchDelegate *pDelegate);

    // Return true to stop
    bool _dispatchTouchToTargetedDelegates(CCNode* pRootNode, unsigned int uIndex, CCTouch* iTouch, CCEvent* iEvent, CCSet* pMutableTouches, bool iSetACopy);
    bool _dispatchTouchToTargetedHandlerOnly(CCTargetedTouchHandler* handler, unsigned int uIndex, CCTouch* iTouch, CCEvent* iEvent, CCSet* pMutableTouches, bool iSetACopy);

    bool _dispatchToTrackedTouchHandler(unsigned int uIndex, CCEvent *pEvent, CCSet* pMutableTouches, bool iSetACopy);
    
    void  _dispatchTouchToStandardDelegates(CCNode* pRootNode, unsigned int uIndex, CCSet* pTouches, CCEvent* pEvent);
    
    bool _dispatchPreTouchToTargetedDelegate(CCTouchDelegate* iDelegate, HitTestProtocol* iProtocol, CCSceneGraphTargetedTouchHandler* iHandler, unsigned int uIndex, CCTouch* iTouch, CCEvent* iEvent, CCSet* pMutableTouches, bool iSetACopy);
    
    CCTargetedTouchHandler* m_TrackedTouchHandler;

};

NS_CC_EXT_END

#endif /* defined(__CCSceneGraphTouchDispatcher__) */
