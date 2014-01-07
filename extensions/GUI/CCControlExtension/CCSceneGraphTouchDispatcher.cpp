//
//  CCSceneGraphTouchDispatcher.cpp
//
//  Created by Gérald Guyomard on 11/10/12.
//
//

#include "CCSceneGraphTouchDispatcher.h"
#include "HitTestProtocol.h"

NS_CC_EXT_BEGIN

class CCSceneGraphTargetedTouchHandler : public CCTargetedTouchHandler
{
public:
    static CCTouchHandler* handlerWithDelegate(CCTouchDelegate *pDelegate, int nPriority, bool bSwallow);
    virtual ~CCSceneGraphTargetedTouchHandler();
    
    /** MutableSet that contains the claimed touches */
    CCSet* getPreClaimedTouches(void) { return m_pPreClaimedTouches; }
    
protected:
    CCSceneGraphTargetedTouchHandler();
    
    CCSet* m_pPreClaimedTouches;
};

CCSceneGraphTargetedTouchHandler::CCSceneGraphTargetedTouchHandler()
: m_pPreClaimedTouches(new CCSet)
{
    
}
CCSceneGraphTargetedTouchHandler::~CCSceneGraphTargetedTouchHandler()
{
    m_pPreClaimedTouches->release();
}

CCTouchHandler*
CCSceneGraphTargetedTouchHandler::handlerWithDelegate(CCTouchDelegate *pDelegate, int nPriority, bool bSwallow)
{
    CCSceneGraphTargetedTouchHandler* pHandler = new CCSceneGraphTargetedTouchHandler();
    
    if (pHandler)
    {
        if (pHandler->initWithDelegate(pDelegate, nPriority, bSwallow))
        {
            pHandler->autorelease();
        }
        else
        {
            CC_SAFE_RELEASE_NULL(pHandler);
        }
    }
    
    return pHandler;

}

CCSceneGraphTouchDispatcher*
CCSceneGraphTouchDispatcher::create()
{
    CCSceneGraphTouchDispatcher * pRet = new CCSceneGraphTouchDispatcher;
    pRet->init();
    pRet->autorelease();
    return pRet;
}

CCSceneGraphTouchDispatcher::CCSceneGraphTouchDispatcher()
: m_TrackedTouchHandler(NULL), m_MouseTracker(NULL)
{}

CCTouchHandler*
CCSceneGraphTouchDispatcher::createTargetedTouchHandler(CCTouchDelegate *pDelegate, int nPriority, bool bSwallowsTouches)
{
    return CCSceneGraphTargetedTouchHandler::handlerWithDelegate(pDelegate, nPriority, bSwallowsTouches);
}

void
CCSceneGraphTouchDispatcher::forceRemoveDelegate(CCTouchDelegate *pDelegate)
{
    if ((m_TrackedTouchHandler != NULL) && (m_TrackedTouchHandler->getDelegate() == pDelegate))
    {
        m_TrackedTouchHandler = NULL;
    }

    if ((m_MouseTracker != NULL) && (m_MouseTracker == pDelegate))
    {
        m_MouseTracker = NULL;
    }
    
    CCTouchDispatcher::forceRemoveDelegate(pDelegate);
}

static bool _isNodeEligibleForHitDispatch(CCNode* iNode)
{
    if (iNode == NULL)
    {
        return false;
    }
    
    if (!iNode->isVisible())
    {
        return false;
    }
    
    HitTestProtocol* hitTestProto = dynamic_cast<HitTestProtocol*>(iNode);
    if ((hitTestProto != NULL) && !hitTestProto->isEnabled())
    {
        return false;
    }
    
    return true;
}

bool
CCSceneGraphTouchDispatcher::_dispatchPreTouchToTargetedDelegate(CCTouchDelegate* iDelegate, HitTestProtocol* iProtocol, CCSceneGraphTargetedTouchHandler* iHandler, unsigned int uIndex, CCTouch* iTouch, CCEvent* iEvent, CCSet* pMutableTouches, bool iSetACopy)
{
    bool handled = false;
    
    if (uIndex == CCTOUCHBEGAN)
    {
        handled = iProtocol->ccPreTouchBegan(iTouch, iEvent);
        
        if (handled)
        {
            iHandler->getPreClaimedTouches()->addObject(iTouch);
        }
    }
    else if (iHandler->getPreClaimedTouches()->containsObject(iTouch))
    {        
        switch (m_sHandlerHelperData[uIndex].m_type)
        {
            case CCTOUCHMOVED:
                handled = iProtocol->ccPreTouchMoved(iTouch, iEvent);
                if (!handled)
                {
                    // Move the touch from the preclaimed touches to the usual claimed touches
                    iHandler->getClaimedTouches()->addObject(iTouch);
                    iHandler->getPreClaimedTouches()->removeObject(iTouch);
                }
                break;
            case CCTOUCHENDED:
                handled = iProtocol->ccPreTouchEnded(iTouch, iEvent);
                iHandler->getPreClaimedTouches()->removeObject(iTouch);
                break;
            case CCTOUCHCANCELLED:
                handled = iProtocol->ccPreTouchCancelled(iTouch, iEvent);
                iHandler->getPreClaimedTouches()->removeObject(iTouch);
                break;
        }
    }
    
    if (handled && iSetACopy)
    {
        pMutableTouches->removeObject(iTouch);
    }

    return handled;
}

bool
CCSceneGraphTouchDispatcher::_dispatchTouchToTargetedDelegates(CCNode* iNode, unsigned int uIndex, CCTouch* iTouch, CCEvent* iEvent, CCSet *pMutableTouches, bool iSetACopy)
{
    if (!_isNodeEligibleForHitDispatch(iNode))
    {
        return false;
    }
    
    // Verify this node pre handles the touch
    CCSceneGraphTargetedTouchHandler* handler = NULL;
    CCTouchDelegate* delegate = dynamic_cast<CCTouchDelegate*>(iNode);
    if (delegate != NULL)
    {
        handler = (CCSceneGraphTargetedTouchHandler*) findHandler(m_pTargetedHandlers, delegate);
        if (handler != NULL)
        {
            HitTestProtocol* proto = dynamic_cast<HitTestProtocol*>(iNode);
            if ((proto != NULL) && _dispatchPreTouchToTargetedDelegate(delegate, proto, handler, uIndex, iTouch, iEvent, pMutableTouches, iSetACopy))
            {
                return true;
            }
        }
    }
    
    // Parse first the children from front to back
    CCArray* children = iNode->getChildren();
    
    const int childrenCount = (children != NULL) ? children->count() : 0;
    
    if (childrenCount > 0)
    {
        iNode->sortAllChildren();
        
        // Iterate reverse
        for (int i = childrenCount - 1; i >= 0; --i)
        {
            CCNode* child = dynamic_cast<CCNode*>(children->objectAtIndex(i));
            if (_dispatchTouchToTargetedDelegates(child, uIndex, iTouch, iEvent, pMutableTouches, iSetACopy))
            {
                return true;
            }
        }        
    }

    //
    // NOW work on this node particularly
    //
    return _dispatchTouchToTargetedHandlerOnly(handler, uIndex, iTouch, iEvent, pMutableTouches, iSetACopy);
}

bool
CCSceneGraphTouchDispatcher::_dispatchTouchToTargetedHandlerOnly(CCTargetedTouchHandler* iHandler, unsigned int uIndex, CCTouch* iTouch, CCEvent* iEvent, CCSet* pMutableTouches, bool iSetACopy)
{
    if (iHandler == NULL)
    {
        return false;
    }
    
    CCTouchDelegate* delegate = iHandler->getDelegate();
    if (delegate == NULL)
    {
        return false;
    }
    
    bool bClaimed = false;
    if (uIndex == CCTOUCHBEGAN)
    {
        if ((m_TrackedTouchHandler == NULL) || (m_TrackedTouchHandler == iHandler))
        {
            bClaimed = delegate->ccTouchBegan(iTouch, iEvent);
            
            if (bClaimed)
            {
                iHandler->getClaimedTouches()->addObject(iTouch);
                
                CCAssert(delegate != NULL, "Delegate should not be null");
                m_TrackedTouchHandler = iHandler;
            }    
        }
    }
    else if (iHandler->getClaimedTouches()->containsObject(iTouch))
    {
        // moved ended canceled
        bClaimed = true;
        
        switch (m_sHandlerHelperData[uIndex].m_type)
        {
            case CCTOUCHMOVED:
                delegate->ccTouchMoved(iTouch, iEvent);
                break;
            case CCTOUCHENDED:
                delegate->ccTouchEnded(iTouch, iEvent);
                iHandler->getClaimedTouches()->removeObject(iTouch);
                break;
            case CCTOUCHCANCELLED:
                delegate->ccTouchCancelled(iTouch, iEvent);
                iHandler->getClaimedTouches()->removeObject(iTouch);
                break;
        }
    }
    
    if (bClaimed && iHandler->isSwallowsTouches())
    {
        if (iSetACopy)
        {
            pMutableTouches->removeObject(iTouch);
        }
        
        return true;
    }
    
    return false;

}

void
CCSceneGraphTouchDispatcher::_dispatchTouchToStandardDelegates(CCNode* iNode, unsigned int uIndex, CCSet* pTouches, CCEvent* pEvent)
{
    if (!_isNodeEligibleForHitDispatch(iNode))
    {
        return;
    }
    
    // Parse first the children from front to back
    CCArray* children = iNode->getChildren();
    
    const int childrenCount = (children != NULL) ? children->count() : 0;
    
    if (childrenCount > 0)
    {
        iNode->sortAllChildren();
        
        // Iterate reverse
        for (int i = childrenCount - 1; i >= 0; --i)
        {
            CCNode* child = dynamic_cast<CCNode*>(children->objectAtIndex(i));
            _dispatchTouchToStandardDelegates(child, uIndex, pTouches, pEvent);
        }
    }
    
    //
    // NOW work on this node particularly
    //
    CCTouchDelegate* delegate = dynamic_cast<CCTouchDelegate*>(iNode);
    if (delegate == NULL)
    {
        return;
    }
    
    CCTouchHandler* handler = findHandler(m_pStandardHandlers, delegate);
    if (handler != NULL)
    {
        switch (m_sHandlerHelperData[uIndex].m_type)
        {
            case CCTOUCHBEGAN:
                delegate->ccTouchesBegan(pTouches, pEvent);
                break;
            case CCTOUCHMOVED:
                delegate->ccTouchesMoved(pTouches, pEvent);
                break;
            case CCTOUCHENDED:
                delegate->ccTouchesEnded(pTouches, pEvent);
                break;
            case CCTOUCHCANCELLED:
                delegate->ccTouchesCancelled(pTouches, pEvent);
                break;
        }
    }
}
    
void
CCSceneGraphTouchDispatcher::dispatchTouches(CCSet *pTouches, CCEvent *pEvent, unsigned int uIndex)
{
    //touches(pTouches, pEvent, uIndex);
    
    CCAssert(uIndex >= 0 && uIndex < 4, "");
    
    CCSet *pMutableTouches;
    m_bLocked = true;
    
    // optimization to prevent a mutable copy when it is not necessary
    const unsigned int uTargetedHandlersCount = m_pTargetedHandlers->count();
    const unsigned int uStandardHandlersCount = m_pStandardHandlers->count();
    const bool bNeedsMutableSet = (uTargetedHandlersCount && uStandardHandlersCount);
    
    pMutableTouches = (bNeedsMutableSet ? pTouches->mutableCopy() : pTouches);

    if (!_dispatchToTrackedTouchHandler(uIndex, pEvent, pMutableTouches, bNeedsMutableSet))
    {
        CCNode* scene = CCDirector::sharedDirector()->getRunningScene();
        
        // Targeted delegates: dispatch all the touches one by one
        
        bool swallowed = false;
        
        if (uTargetedHandlersCount > 0)
        {
            CCTouch *pTouch;
            CCSetIterator setIter;
            
            for (setIter = pMutableTouches->begin(); setIter != pMutableTouches->end(); ++setIter)
            {
                pTouch = (CCTouch *)(*setIter);
                if (_dispatchTouchToTargetedDelegates(scene, uIndex, pTouch, pEvent, pMutableTouches, bNeedsMutableSet))
                {
                    swallowed = true;
                    break;
                }
            }
        }
        
        if (!swallowed && (uStandardHandlersCount > 0))
        {
            // Try now the standard delegates
            _dispatchTouchToStandardDelegates(scene, uIndex, pMutableTouches, pEvent);
        }
    }
            
    if (bNeedsMutableSet)
    {
        pMutableTouches->release();
    }
    
    //
    // Optimization. To prevent a [handlers copy] which is expensive
    // the add/removes/quit is done after the iterations
    //
    m_bLocked = false;
    if (m_bToRemove)
    {
        m_bToRemove = false;
        for (unsigned int i = 0; i < m_pHandlersToRemove->num; ++i)
        {
            forceRemoveDelegate((CCTouchDelegate*)m_pHandlersToRemove->arr[i]);
        }
        ccCArrayRemoveAllValues(m_pHandlersToRemove);
    }
    
    if (m_bToAdd)
    {
        m_bToAdd = false;
        CCTouchHandler* pHandler = NULL;
        CCObject* pObj = NULL;
        CCARRAY_FOREACH(m_pHandlersToAdd, pObj)
        {
            pHandler = (CCTouchHandler*)pObj;
            if (! pHandler)
            {
                break;
            }
            
            if (dynamic_cast<CCTargetedTouchHandler*>(pHandler) != NULL)
            {
                forceAddHandler(pHandler, m_pTargetedHandlers);
            }
            else
            {
                forceAddHandler(pHandler, m_pStandardHandlers);
            }
        }
        
        m_pHandlersToAdd->removeAllObjects();    
    }
    
    if (m_bToQuit)
    {
        m_bToQuit = false;
        forceRemoveAllDelegates();
    }
}

bool
CCSceneGraphTouchDispatcher::_dispatchToTrackedTouchHandler(unsigned int uIndex, CCEvent* pEvent, CCSet* pTouches, bool iSetACopy)
{
    if (m_TrackedTouchHandler == NULL)
    {
        return false;
    }
    
    CCTouch* pTouch;
    CCSetIterator setIter;
    
    CCSet* touchesToIterate = pTouches->copy();

    for (setIter = touchesToIterate->begin(); setIter != touchesToIterate->end(); ++setIter)
    {
        if (m_TrackedTouchHandler == NULL)
        {
            return false;
        }
        
        pTouch = (CCTouch *)(*setIter);
        
        // this delegate should eat all the events until touchEnded or cancelled
        if (uIndex == CCTOUCHBEGAN)
        {
            // Simply ignore it
            CCAssert(!m_TrackedTouchHandler->getClaimedTouches()->containsObject(pTouch), "This new touch should not have been claimed by the tracked touch handler");
            
            _dispatchTouchToTargetedHandlerOnly(m_TrackedTouchHandler, uIndex, pTouch, pEvent, pTouches, iSetACopy);
        }
        else
        {
            // maybe it's the touch being tracked by the handler
            // Directly dispatch it to this handler and nobody else
            
            if (m_TrackedTouchHandler->getClaimedTouches()->containsObject(pTouch))
            {
                _dispatchTouchToTargetedHandlerOnly(m_TrackedTouchHandler, uIndex, pTouch, pEvent, pTouches, iSetACopy);
                
                if (m_TrackedTouchHandler->getClaimedTouches()->count() == 0)
                {
                    m_TrackedTouchHandler = NULL;
                }
            }
        }
    }
    
    touchesToIterate->release();
    return true;
}

void
CCSceneGraphTouchDispatcher::touchesBegan(CCSet* touches, CCEvent* pEvent)
{
    dispatchTouches(touches, pEvent, CCTOUCHBEGAN);
}
    
void
CCSceneGraphTouchDispatcher::touchesMoved(CCSet* touches, CCEvent* pEvent)
{
    dispatchTouches(touches, pEvent, CCTOUCHMOVED);
}
    
void
CCSceneGraphTouchDispatcher::touchesEnded(CCSet* touches, CCEvent* pEvent)
{
    dispatchTouches(touches, pEvent, CCTOUCHENDED);

}
    
void
CCSceneGraphTouchDispatcher::touchesCancelled(CCSet* touches, CCEvent* pEvent)
{
    dispatchTouches(touches, pEvent, CCTOUCHCANCELLED);
}

void
CCSceneGraphTouchDispatcher::setMouseTracker(CCTouchDelegate* iTracker)
{
    m_MouseTracker = iTracker;
}

static
bool _dispatchWheel(CCNode* iRoot, const CCPoint& iWorldMouseLocation, float iDeltaX, float iDeltaY, float iDeltaZ)
{
    if (!_isNodeEligibleForHitDispatch(iRoot))
    {
        return false;
    }
    
    CCArray* children = iRoot->getChildren();
    const int childrenCount = (children != NULL) ? children->count() : 0;
    
    if (childrenCount > 0)
    {
        iRoot->sortAllChildren();
        
        // Iterate reverse
        for (int i = childrenCount - 1; i >= 0; --i)
        {
            CCNode* child = static_cast<CCNode*>(children->objectAtIndex(i));
            if (_dispatchWheel(child, iWorldMouseLocation, iDeltaX, iDeltaY, iDeltaZ))
            {
                return true;
            }
        }
    }
    
    CCTouchDelegate* delegate = dynamic_cast<CCTouchDelegate*>(iRoot);
    if ((delegate != NULL) && (CCDirector::sharedDirector()->getTouchDispatcher()->findHandler(delegate) != NULL))
    {
        if (delegate->wheel(iWorldMouseLocation, iDeltaX, iDeltaY, iDeltaZ))
        {
            return true;
        }
    }
    
    return false;
}

void
CCSceneGraphTouchDispatcher::wheel(const CCPoint& iWorldMousePosition, float iDeltaX, float iDeltaY, float iDeltaZ)
{
    const CCPoint worldLoc = CCDirector::sharedDirector()->convertToGL(iWorldMousePosition);
    
    _dispatchWheel(CCDirector::sharedDirector()->getRunningScene(), worldLoc, iDeltaX, iDeltaY, iDeltaZ);
}

bool
CCSceneGraphTouchDispatcher::_dispatchMouseMoved(CCNode* iRoot, const CCPoint& iWorldMousePosition)
{
    if (!_isNodeEligibleForHitDispatch(iRoot))
    {
        return false;
    }
    
    // First parse children
    CCArray* children = iRoot->getChildren();
    const int childrenCount = (children != NULL) ? children->count() : 0;
    
    if (childrenCount > 0)
    {
        iRoot->sortAllChildren();
        
        // Iterate reverse
        for (int i = childrenCount - 1; i >= 0; --i)
        {
            CCNode* child = static_cast<CCNode*>(children->objectAtIndex(i));
            if (_dispatchMouseMoved(child, iWorldMousePosition))
            {
                return true;
            }
        }
    }

    
    // Then this node
    CCTouchDelegate* delegate = dynamic_cast<CCTouchDelegate*>(iRoot);
    if (delegate != NULL)
    {
        CCTouchHandler* handler = findHandler(m_pTargetedHandlers, delegate);
        if (handler != NULL)
        {
            assert(handler->getDelegate() == delegate);
            if (delegate->mouseMoved(iWorldMousePosition))
            {
                return true;
            }
        }
    }
    
    return false;
}

void
CCSceneGraphTouchDispatcher::mouseMoved(const CCPoint& iWorldMousePosition)
{
    if ((m_MouseTracker == NULL) || !m_MouseTracker->mouseMoved(iWorldMousePosition))
    {
        CCNode* root = CCDirector::sharedDirector()->getRunningScene();
        _dispatchMouseMoved(root, iWorldMousePosition);
    }
}

bool
CCSceneGraphTouchDispatcher::_dispatchSecondaryButtonDown(CCNode* iRoot, const CCPoint& iWorldMousePosition, int iButtonID)
{
    if (!_isNodeEligibleForHitDispatch(iRoot))
    {
        return false;
    }
    
    // First parse children
    CCArray* children = iRoot->getChildren();
    const int childrenCount = (children != NULL) ? children->count() : 0;
    
    if (childrenCount > 0)
    {
        iRoot->sortAllChildren();
        
        // Iterate reverse
        for (int i = childrenCount - 1; i >= 0; --i)
        {
            CCNode* child = static_cast<CCNode*>(children->objectAtIndex(i));
            if (_dispatchSecondaryButtonDown(child, iWorldMousePosition, iButtonID))
            {
                return true;
            }
        }
    }
    
    
    // Then this node
    CCTouchDelegate* delegate = dynamic_cast<CCTouchDelegate*>(iRoot);
    if (delegate != NULL)
    {
        CCTouchHandler* handler = findHandler(m_pTargetedHandlers, delegate);
        if (handler != NULL)
        {
            assert(handler->getDelegate() == delegate);
            if (delegate->secondaryButtonDown(iWorldMousePosition, iButtonID))
            {
                return true;
            }
        }
    }
    
    return false;
}

void
CCSceneGraphTouchDispatcher::secondaryButtonDown(const CCPoint& iWorldMousePosition, int iButtonID)
{
    if ((m_MouseTracker == NULL) || !m_MouseTracker->secondaryButtonDown(iWorldMousePosition, iButtonID))
    {
        CCNode* root = CCDirector::sharedDirector()->getRunningScene();
        _dispatchSecondaryButtonDown(root, iWorldMousePosition, iButtonID);
    }
}

bool
CCSceneGraphTouchDispatcher::_dispatchSecondaryButtonUp(CCNode* iRoot, const CCPoint& iWorldMousePosition, int iButtonID)
{
    if (!_isNodeEligibleForHitDispatch(iRoot))
    {
        return false;
    }
    
    // First parse children
    CCArray* children = iRoot->getChildren();
    const int childrenCount = (children != NULL) ? children->count() : 0;
    
    if (childrenCount > 0)
    {
        iRoot->sortAllChildren();
        
        // Iterate reverse
        for (int i = childrenCount - 1; i >= 0; --i)
        {
            CCNode* child = static_cast<CCNode*>(children->objectAtIndex(i));
            if (_dispatchSecondaryButtonUp(child, iWorldMousePosition, iButtonID))
            {
                return true;
            }
        }
    }
    
    
    // Then this node
    CCTouchDelegate* delegate = dynamic_cast<CCTouchDelegate*>(iRoot);
    if (delegate != NULL)
    {
        CCTouchHandler* handler = findHandler(m_pTargetedHandlers, delegate);
        if (handler != NULL)
        {
            assert(handler->getDelegate() == delegate);
            if (delegate->secondaryButtonUp(iWorldMousePosition, iButtonID))
            {
                return true;
            }
        }
    }
    
    return false;
}

void
CCSceneGraphTouchDispatcher::secondaryButtonUp(const CCPoint& iWorldMousePosition, int iButtonID)
{
    if ((m_MouseTracker == NULL) || !m_MouseTracker->secondaryButtonDown(iWorldMousePosition, iButtonID))
    {
        CCNode* root = CCDirector::sharedDirector()->getRunningScene();
        _dispatchSecondaryButtonUp(root, iWorldMousePosition, iButtonID);
    }
}

static
CCTouchDelegate* _simulateTouchDown(CCNode* iRoot, CCTouch* iTouch, bool iIncludingRoot)
{
    if (!_isNodeEligibleForHitDispatch(iRoot))
    {
        return NULL;
    }
    
    CCArray* children = iRoot->getChildren();
    const int childrenCount = (children != NULL) ? children->count() : 0;
    
    if (childrenCount > 0)
    {
        iRoot->sortAllChildren();
        
        // Iterate reverse
        for (int i = childrenCount - 1; i >= 0; --i)
        {
            CCNode* child = static_cast<CCNode*>(children->objectAtIndex(i));
            CCTouchDelegate* childDelegate = _simulateTouchDown(child, iTouch, true);
            if (childDelegate != NULL)
            {
                return childDelegate;
            }
        }
    }
    
    if (iIncludingRoot)
    {
        CCTouchDelegate* delegate = dynamic_cast<CCTouchDelegate*>(iRoot);
        if ((delegate != NULL) && (CCDirector::sharedDirector()->getTouchDispatcher()->findHandler(delegate) != NULL))
        {
            if (delegate->ccTouchBegan(iTouch, NULL))
            {
                return delegate;
            }
        }
    }
    
    return NULL;
}

CCTouchDelegate*
CCSceneGraphTouchDispatcher::simulateTouchDown(CCTouch* iTouch)
{
    return _simulateTouchDown(CCDirector::sharedDirector()->getRunningScene(), iTouch, false);
}

NS_CC_EXT_END

