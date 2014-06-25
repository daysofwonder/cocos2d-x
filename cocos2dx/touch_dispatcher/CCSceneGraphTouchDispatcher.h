//
//  CCSceneGraphTouchDispatcher.h
//
//  Created by Gérald Guyomard on 11/10/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCSceneGraphTouchDispatcher__
#define __CCSceneGraphTouchDispatcher__

#include <cocos2d.h>
#include "touch_dispatcher/CCTouchDispatcher.h"

NS_CC_BEGIN

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

    // Desktop specific
    virtual CCTouchDelegate* getMouseTracker() { return m_MouseTracker; }
    virtual void setMouseTracker(CCTouchDelegate* iTracker);
    
    virtual void wheel(const CCPoint& iWorldMousePosition, float iDeltaX, float iDeltaY, float iDeltaZ);
    
    virtual void mouseMoved(const CCPoint& iWorldMousePosition);
    virtual void secondaryButtonDown(const CCPoint& iWorldMousePosition, int iButtonID);
    virtual void secondaryButtonUp(const CCPoint& iWorldMousePosition, int iButtonID);
    
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
    
    bool _dispatchMouseMoved(CCNode* iRoot, const CCPoint& iWorldMousePosition);
    bool _dispatchSecondaryButtonDown(CCNode* iRoot, const CCPoint& iWorldMousePosition, int iButtonID);
    bool _dispatchSecondaryButtonUp(CCNode* iRoot, const CCPoint& iWorldMousePosition, int iButtonID);
    
    CCTouchDelegate* _dispatchMouseOver(CCNode* iRoot, const CCPoint& iWorldMousePosition);
    
    CCTargetedTouchHandler* m_TrackedTouchHandler;
    CCTouchDelegate*        m_MouseTracker;
    CCTouchDelegate*        m_MouseOverDelegate;
};

NS_CC_END

#endif /* defined(__CCSceneGraphTouchDispatcher__) */
