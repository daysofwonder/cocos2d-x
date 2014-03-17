/****************************************************************************
 Copyright (c) 2012 cocos2d-x.org
 Copyright (c) 2010 Sangwoo Im
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "CCScrollView.h"
#include "GUI/CCControlExtension/CCControl.h"

NS_CC_EXT_BEGIN

#define SCROLL_DEACCEL_RATE  0.95f
#define SCROLL_DEACCEL_DIST  1.0f
#define BOUNCE_DURATION      0.15f
#define INSET_RATIO          0.2f
#define MOVE_INCH            7.0f/160.0f

static float convertDistanceFromPointToInch(float pointDis)
{
    float factor = ( CCEGLView::sharedOpenGLView()->getScaleX() + CCEGLView::sharedOpenGLView()->getScaleY() ) / 2;
    return pointDis * factor / CCDevice::getDPI();
}

CCScrollBarFactory* CCScrollView::s_DefaultScrollBarFactory = NULL;

CCScrollView::CCScrollView()
: m_fZoomScale(0.0f)
, m_fMinZoomScale(0.0f)
, m_fMaxZoomScale(0.0f)
, m_pDelegate(NULL)
, m_eDirection(kCCScrollViewDirectionBoth)
, m_bDragging(false)
, m_pContainer(NULL)
, m_bTouchMoved(false)
, m_bBounceable(false)
, m_bClippingToBounds(false)
, m_fTouchLength(0.0f)
, m_pTouches(NULL)
, m_fMinScale(0.0f)
, m_fMaxScale(0.0f)
, m_bScrollEnabled(true)
, m_bDelaysContentTouches(false),
m_DelayedHitChild(NULL),
m_DelayedTouch(NULL),
m_ScrollBarFactory(NULL),
m_HorizontalScrollBar(NULL),
m_VerticalScrollBar(NULL),
m_ScrollBarsFlags(fNoScrollBars),
m_HorizontalScrollBarMargin(0),
m_VerticalScrollBarMargin(0),
m_ScrollBarZOrder(0)
{

}

CCScrollView::~CCScrollView()
{
    CC_SAFE_RELEASE(m_DelayedTouch);
    
    unscheduleStillTouchDown();
    CC_SAFE_RELEASE(m_pTouches);
    
    CC_SAFE_RELEASE(m_ScrollBarFactory);
    CC_SAFE_RELEASE(m_HorizontalScrollBar);
    CC_SAFE_RELEASE(m_VerticalScrollBar);
}

CCScrollView* CCScrollView::create(CCSize size, CCNode* container/* = NULL*/)
{
    CCScrollView* pRet = new CCScrollView();
    if (pRet && pRet->initWithViewSize(size, container))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    return pRet;
}

CCScrollView* CCScrollView::create()
{
    CCScrollView* pRet = new CCScrollView();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    return pRet;
}


bool CCScrollView::initWithViewSize(CCSize size, CCNode *container/* = NULL*/)
{
    if (CCLayer::init())
    {
        m_pContainer = container;
        
        if (!this->m_pContainer)
        {
            m_pContainer = CCLayer::create();
            this->m_pContainer->ignoreAnchorPointForPosition(false);
            this->m_pContainer->setAnchorPoint(ccp(0.0f, 0.0f));
        }

        this->setViewSize(size);

        setTouchEnabled(true);
        m_pTouches = new CCArray();
        m_pDelegate = NULL;
        m_bBounceable = true;
        m_bClippingToBounds = true;
        //m_pContainer->setContentSize(CCSizeZero);
        m_eDirection  = kCCScrollViewDirectionBoth;
        m_pContainer->setPosition(ccp(0.0f, 0.0f));
        m_fTouchLength = 0.0f;
        
        this->addChild(m_pContainer);
        m_fMinScale = m_fMaxScale = 1.0f;
        return true;
    }
    return false;
}

bool CCScrollView::init()
{
    return this->initWithViewSize(CCSizeMake(200, 200), NULL);
}

void CCScrollView::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, CCLayer::getTouchPriority(), false);
}

bool CCScrollView::isEnabled()
{
    return isTouchEnabled();
}

void CCScrollView::setIsEnabled(bool iEnabled)
{
    setTouchEnabled(iEnabled);
}

bool CCScrollView::isTouchInside(const CCPoint& iLocation)
{
    CCPoint touchLocation = getParent()->convertToNodeSpace(iLocation);
    CCRect bBox= boundingBox();

    // adjust touchable rect with touch insets
    bBox.origin.x += m_TouchInsets.origin.x;
    bBox.origin.y += m_TouchInsets.origin.y;
    bBox.size.width -= m_TouchInsets.getMaxX();
    bBox.size.height -= m_TouchInsets.getMaxY();
    
    return bBox.containsPoint(touchLocation);
}

bool CCScrollView::wheel(const CCPoint& iWorldMouseLocation, float iDeltaX, float iDeltaY, float iDeltaZ)
{
    if ((m_pContainer != NULL) && isTouchInside(iWorldMouseLocation))
    {
        CCPoint offset = getContentOffset();
        float scale = 1.f;
        
        // Adjust the delta with the global scale
        CCNode* parent = getParent();
        if (parent != nullptr)
        {
            CCRect worldRect = CCRectApplyAffineTransform(boundingBox(), parent->nodeToWorldTransform());
            scale = getContentSize().width / worldRect.size.width;
        }
        
        switch (m_eDirection)
        {
            case kCCScrollViewDirectionBoth:
            {
                offset.x += iDeltaX * scale;
                offset.y -= iDeltaY * scale;
                break;
            }
                
            case kCCScrollViewDirectionHorizontal:
            {
                offset.x += iDeltaX * scale;
                break;
            }
                
            case kCCScrollViewDirectionVertical:
            {
                offset.y -= iDeltaY * scale;
                break;
            }
                
            default:
                break;
        }
        
        setContentOffset(offset);
        relocateContainer(false);
        
        return true;
    }
    
    return false;
}

bool CCScrollView::ccPreTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    if (m_bDelaysContentTouches && m_bScrollEnabled && scrollEnabledOnTouch() && (m_pTouches->count() == 0) && isTouchInside(pTouch->getLocation()))
    {
        if ((m_HorizontalScrollBar != NULL) && m_HorizontalScrollBar->isVisible() && m_HorizontalScrollBar->isTouchInside(pTouch))
        {
            return false;
        }

        if ((m_VerticalScrollBar != NULL) && m_VerticalScrollBar->isVisible() && m_VerticalScrollBar->isTouchInside(pTouch))
        {
            return false;
        }
        
        if (m_DelayedTouch != pTouch)
        {
            CC_SAFE_RELEASE(m_DelayedTouch);
            m_DelayedTouch = pTouch;
            CC_SAFE_RETAIN(m_DelayedTouch);
        }
        
        m_InitialDelayedTouchPosition = convertTouchToNodeSpace(m_DelayedTouch);
        
        // Eat this touch, wait for next move before redirecting it
        // to the proper child
        scheduleOnce((SEL_SCHEDULE) &CCScrollView::onStillTouchDown, 0.1f);
        
        return true;
    }
    
    return false;
}

void
CCScrollView::unscheduleStillTouchDown()
{
    unschedule((SEL_SCHEDULE) &CCScrollView::onStillTouchDown);
}

void
CCScrollView::onStillTouchDown(float iDelay)
{
    // If we reach this point, this means, we must redirect
    // eaten touch to the proper child
    CCAssert(m_DelayedTouch != NULL, "There should be a delayed touch");
        
    m_DelayedHitChild = CCDirector::sharedDirector()->getTouchDispatcher()->simulateTouchDown(m_DelayedTouch);
    
}

bool CCScrollView::ccPreTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    if (pTouch == m_DelayedTouch)
    {
        if (m_DelayedHitChild != NULL)
        {
            CC_SAFE_RELEASE_NULL(m_DelayedTouch);
            
            // this child received a touch began
            m_DelayedHitChild = NULL;
            
            return true;
        }
        
        // discard still touch down if moved distance is far enough        
        CCPoint newPosition = convertTouchToNodeSpace(pTouch);
        CCPoint dist = ccpSub(newPosition, m_InitialDelayedTouchPosition);
        
        const float sqrDist = (dist.x * dist.x) + (dist.y * dist.y);
        const float kMaxDist = 8;
        if (sqrDist <= (kMaxDist * kMaxDist))
        {
            // still tracking
            return true;
        }
        else
        {
            unscheduleStillTouchDown();
            
            // The intent is actually to scroll
            // Call the sequence that was ignore previously
            ccTouchBegan(m_DelayedTouch, pEvent);
            CC_SAFE_RELEASE_NULL(m_DelayedTouch);
            
            // ccTouchMoved(m_DelayedTouch, pEvent);
            // will be called in casual dispatching sequence

        }
    }
    
    return false;
}

bool CCScrollView::ccPreTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    unscheduleStillTouchDown();
    
    if (m_DelayedTouch == pTouch)
    {
        m_DelayedHitChild = NULL;
        
        // Emulate click down, up on the hit child
        CCTouchDelegate* hit = CCDirector::sharedDirector()->getTouchDispatcher()->simulateTouchDown(m_DelayedTouch);
        if (hit != NULL)
        {
            hit->ccTouchEnded(pTouch, pEvent);
        }
        
        CC_SAFE_RELEASE_NULL(m_DelayedTouch);
    }
    else if (m_pTouches->containsObject(pTouch))
    {
        ccTouchEnded(pTouch, pEvent);
    }
    
    return true;
}

bool CCScrollView::ccPreTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
    return false;
}

bool CCScrollView::isNodeVisible(CCNode* node)
{
    const CCPoint offset = this->getContentOffset();
    const CCSize  size   = this->getViewSize();
    const float   scale  = this->getZoomScale();
    
    CCRect viewRect;
    
    viewRect = CCRectMake(-offset.x/scale, -offset.y/scale, size.width/scale, size.height/scale); 
    
    return viewRect.intersectsRect(node->boundingBox());
}

void CCScrollView::pause(CCObject* sender)
{
    m_pContainer->pauseSchedulerAndActions();

    CCObject* pObj = NULL;
    CCArray* pChildren = m_pContainer->getChildren();

    CCARRAY_FOREACH(pChildren, pObj)
    {
        CCNode* pChild = (CCNode*)pObj;
        pChild->pauseSchedulerAndActions();
    }
}

void CCScrollView::resume(CCObject* sender)
{
    CCObject* pObj = NULL;
    CCArray* pChildren = m_pContainer->getChildren();

    CCARRAY_FOREACH(pChildren, pObj)
    {
        CCNode* pChild = (CCNode*)pObj;
        pChild->resumeSchedulerAndActions();
    }

    m_pContainer->resumeSchedulerAndActions();
}

void CCScrollView::setTouchEnabled(bool e)
{
    CCLayer::setTouchEnabled(e);
    if (!e)
    {
        m_bDragging = false;
        m_bTouchMoved = false;
        m_pTouches->removeAllObjects();
    }
}

void CCScrollView::setContentOffset(CCPoint offset, bool animated/* = false*/)
{
    if (animated)
    { //animate scrolling
        this->setContentOffsetInDuration(offset, BOUNCE_DURATION);
    } 
    else
    { //set the container position directly
        if (!m_bBounceable)
        {
            const CCPoint minOffset = this->minContainerOffset();
            const CCPoint maxOffset = this->maxContainerOffset();
            
            offset.x = MAX(minOffset.x, MIN(maxOffset.x, offset.x));
            offset.y = MAX(minOffset.y, MIN(maxOffset.y, offset.y));
        }

        m_pContainer->setPosition(offset);
        updateScrollBarsPositions();

        if (m_pDelegate != NULL)
        {
            m_pDelegate->scrollViewDidScroll(this);   
        }
    }
}

void CCScrollView::setContentOffsetInDuration(CCPoint offset, float dt)
{
    CCFiniteTimeAction *scroll, *expire;
    
    scroll = CCMoveTo::create(dt, offset);
    expire = CCCallFuncN::create(this, callfuncN_selector(CCScrollView::stoppedAnimatedScroll));
    m_pContainer->runAction(CCSequence::create(scroll, expire, NULL));
    this->schedule(schedule_selector(CCScrollView::performedAnimatedScroll));
}

CCPoint CCScrollView::getContentOffset()
{
    return m_pContainer->getPosition();
}

void CCScrollView::setZoomScale(float s)
{
    if (m_pContainer->getScale() != s)
    {
        CCPoint oldCenter, newCenter;
        CCPoint center;
        
        if (m_fTouchLength == 0.0f) 
        {
            center = ccp(m_tViewSize.width*0.5f, m_tViewSize.height*0.5f);
            center = this->convertToWorldSpace(center);
        }
        else
        {
            center = m_tTouchPoint;
        }
        
        oldCenter = m_pContainer->convertToNodeSpace(center);
        m_pContainer->setScale(MAX(m_fMinScale, MIN(m_fMaxScale, s)));
        newCenter = m_pContainer->convertToWorldSpace(oldCenter);
        
        updateInset();
        
        const CCPoint offset = ccpSub(center, newCenter);
        if (m_pDelegate != NULL)
        {
            m_pDelegate->scrollViewDidZoom(this);
        }
        this->setContentOffset(ccpAdd(m_pContainer->getPosition(),offset));
        relocateContainer(false);
        
        updateScrollBarsPositions();
    }
}

float CCScrollView::getZoomScale()
{
    return m_pContainer->getScale();
}

void CCScrollView::setZoomScale(float s, bool animated)
{
    if (animated)
    {
        this->setZoomScaleInDuration(s, BOUNCE_DURATION);
    }
    else
    {
        this->setZoomScale(s);
    }
}

void CCScrollView::setZoomScaleInDuration(float s, float dt)
{
    if (dt > 0)
    {
        if (m_pContainer->getScale() != s)
        {
            CCActionTween *scaleAction;
            scaleAction = CCActionTween::create(dt, "zoomScale", m_pContainer->getScale(), s);
            this->runAction(scaleAction);
        }
    }
    else
    {
        this->setZoomScale(s);
    }
}

void CCScrollView::setMinZoomScale(float iMinZoomScale)
{
    m_fMinScale = iMinZoomScale;
}

void CCScrollView::setMaxZoomScale(float iMaxZoomScale)
{
    m_fMaxScale = iMaxZoomScale;
}

void CCScrollView::setViewSize(CCSize size)
{
    m_tViewSize = size;
    CCLayer::setContentSize(size);
    
    updateContainerOffset();
    
    updateScrollBarsPositions();
}

CCNode * CCScrollView::getContainer()
{
    return this->m_pContainer;
}

void CCScrollView::setContainer(CCNode * pContainer)
{
    /*
    // Make sure that 'm_pContainer' has a non-NULL value since there are
    // lots of logic that use 'm_pContainer'.
    // DoW: I don't agree: if we return right now, we may leave the scroll view
     // with a non null container, which is not also the expected behavior
    if (NULL == pContainer)
        return;*/

    this->removeAllChildrenWithCleanup(true);
    this->m_pContainer = pContainer;

    // Previous container removed, new container set up,
    // It's now safe to return if new installed container is NULL
    if (pContainer == NULL)
    {
        return;
    }
    
    this->m_pContainer->ignoreAnchorPointForPosition(false);
    this->m_pContainer->setAnchorPoint(ccp(0.0f, 0.0f));

    this->addChild(this->m_pContainer);

    this->setViewSize(this->m_tViewSize);
}

void
CCScrollView::updateContainerOffset()
{
    if (m_pContainer != NULL)
    {
        updateInset();
        setContentOffset(minContainerOffset());
        relocateContainer(false);
    }
}

void CCScrollView::relocateContainer(bool animated)
{
    CCPoint oldPoint, min, max;
    float newX, newY;
    
    min = this->minContainerOffset();
    max = this->maxContainerOffset();
    
    oldPoint = m_pContainer->getPosition();

    newX     = oldPoint.x;
    newY     = oldPoint.y;
    if (m_eDirection == kCCScrollViewDirectionBoth || m_eDirection == kCCScrollViewDirectionHorizontal)
    {
        newX     = MAX(newX, min.x);
        newX     = MIN(newX, max.x);
    }

    if (m_eDirection == kCCScrollViewDirectionBoth || m_eDirection == kCCScrollViewDirectionVertical)
    {
        newY     = MIN(newY, max.y);
        newY     = MAX(newY, min.y);
    }

    if (newY != oldPoint.y || newX != oldPoint.x)
    {
        this->setContentOffset(ccp(newX, newY), animated);
    }
}

CCPoint CCScrollView::maxContainerOffset()
{
    CCPoint maxOffset;
    
    maxOffset.x = 0;
    
    maxOffset.y = m_tViewSize.height - m_pContainer->getContentSize().height*m_pContainer->getScaleY();
    maxOffset.y = MAX(maxOffset.y, 0);

    return maxOffset;
}

CCPoint CCScrollView::minContainerOffset()
{
    CCPoint minOffset = ccp(m_tViewSize.width - m_pContainer->getContentSize().width*m_pContainer->getScaleX(),
               m_tViewSize.height - m_pContainer->getContentSize().height*m_pContainer->getScaleY());
    
    minOffset.x = MIN(minOffset.x, 0);
    
    return minOffset;
}

void CCScrollView::deaccelerateScrolling(float dt)
{
    if (m_bDragging)
    {
        this->unschedule(schedule_selector(CCScrollView::deaccelerateScrolling));
        return;
    }
    
    float newX, newY;
    CCPoint maxInset, minInset;
    
    m_pContainer->setPosition(ccpAdd(m_pContainer->getPosition(), m_tScrollDistance));
    
    if (m_bBounceable)
    {
        maxInset = m_fMaxInset;
        minInset = m_fMinInset;
    }
    else
    {
        maxInset = this->maxContainerOffset();
        minInset = this->minContainerOffset();
    }
    
    //check to see if offset lies within the inset bounds
    newX     = MIN(m_pContainer->getPosition().x, maxInset.x);
    newX     = MAX(newX, minInset.x);
    newY     = MIN(m_pContainer->getPosition().y, maxInset.y);
    newY     = MAX(newY, minInset.y);
    
    m_tScrollDistance     = ccpSub(m_tScrollDistance, ccp(newX - m_pContainer->getPosition().x, newY - m_pContainer->getPosition().y));
    m_tScrollDistance     = ccpMult(m_tScrollDistance, SCROLL_DEACCEL_RATE);
    this->setContentOffset(ccp(newX,newY));
    
    if ((fabsf(m_tScrollDistance.x) <= SCROLL_DEACCEL_DIST &&
         fabsf(m_tScrollDistance.y) <= SCROLL_DEACCEL_DIST) ||
        newY > maxInset.y || newY < minInset.y ||
        newX > maxInset.x || newX < minInset.x ||
        newX == maxInset.x || newX == minInset.x ||
        newY == maxInset.y || newY == minInset.y)
    {
        this->unschedule(schedule_selector(CCScrollView::deaccelerateScrolling));
        this->relocateContainer(true);
    }
    
    updateScrollBarsPositions();
}

void CCScrollView::stoppedAnimatedScroll(CCNode * node)
{
    this->unschedule(schedule_selector(CCScrollView::performedAnimatedScroll));
    // After the animation stopped, "scrollViewDidScroll" should be invoked, this could fix the bug of lack of tableview cells.
    if (m_pDelegate != NULL)
    {
        m_pDelegate->scrollViewDidScroll(this);
    }
}

void CCScrollView::performedAnimatedScroll(float dt)
{
    if (m_bDragging)
    {
        this->unschedule(schedule_selector(CCScrollView::performedAnimatedScroll));
        return;
    }

    if (m_pDelegate != NULL)
    {
        m_pDelegate->scrollViewDidScroll(this);
    }
}


const CCSize& CCScrollView::getContentSize() const
{
	return m_pContainer->getContentSize();
}

void CCScrollView::setContentSize(const CCSize & size)
{
    if (this->getContainer() != NULL)
    {
        this->getContainer()->setContentSize(size);
		this->updateInset();
        updateScrollBarsPositions();
    }
}

void CCScrollView::updateInset()
{
	if (this->getContainer() != NULL)
	{
		m_fMaxInset = this->maxContainerOffset();
		m_fMaxInset = ccp(m_fMaxInset.x + m_tViewSize.width * INSET_RATIO,
			m_fMaxInset.y + m_tViewSize.height * INSET_RATIO);
		m_fMinInset = this->minContainerOffset();
		m_fMinInset = ccp(m_fMinInset.x - m_tViewSize.width * INSET_RATIO,
			m_fMinInset.y - m_tViewSize.height * INSET_RATIO);
	}
}

/**
 * make sure all children go to the container
 */
void CCScrollView::addChild(CCNode * child, int zOrder, int tag)
{
    child->ignoreAnchorPointForPosition(false);
    child->setAnchorPoint(ccp(0.0f, 0.0f));
    if (m_pContainer != child) {
        m_pContainer->addChild(child, zOrder, tag);
    } else {
        CCLayer::addChild(child, zOrder, tag);
    }
}

void CCScrollView::addChild(CCNode * child, int zOrder)
{
    this->addChild(child, zOrder, child->getTag());
}

void CCScrollView::addChild(CCNode * child)
{
    this->addChild(child, child->getZOrder(), child->getTag());
}

/**
 * clip this view so that outside of the visible bounds can be hidden.
 */
void CCScrollView::beforeDraw()
{
    if (m_bClippingToBounds)
    {
        glEnable(GL_SCISSOR_TEST);
        
        CCRect worldBox = CCRectApplyAffineTransform(boundingBox(), getParent()->nodeToWorldTransform());
        CCEGLView::sharedOpenGLView()->setScissorInPoints(worldBox.origin.x, worldBox.origin.y, worldBox.size.width, worldBox.size.height);
    }
}

/**
 * retract what's done in beforeDraw so that there's no side effect to
 * other nodes.
 */
void CCScrollView::afterDraw()
{
    if (m_bClippingToBounds)
    {
        /*if (m_bScissorRestored) {//restore the parent's scissor rect
            CCEGLView::sharedOpenGLView()->setScissorInPoints(m_tParentScissorRect.origin.x, m_tParentScissorRect.origin.y, m_tParentScissorRect.size.width, m_tParentScissorRect.size.height);
        }
        else */{
            glDisable(GL_SCISSOR_TEST);
        }
    }
}

void CCScrollView::visit()
{
	// quick return if not visible
	if (!isVisible())
    {
		return;
    }

	kmGLPushMatrix();
	
    if (m_pGrid && m_pGrid->isActive())
    {
        m_pGrid->beforeDraw();
        this->transformAncestors();
    }

	this->transform();
    this->beforeDraw();

	if(m_pChildren)
    {
		ccArray *arrayData = m_pChildren->data;
		unsigned int i=0;
		
		// draw children zOrder < 0
		for( ; i < arrayData->num; i++ )
        {
			CCNode *child =  (CCNode*)arrayData->arr[i];
			if ( child->getZOrder() < 0 )
            {
				child->visit();
			}
            else
            {
				break;
            }
		}
		
		// this draw
		this->draw();
		
		// draw children zOrder >= 0
		for( ; i < arrayData->num; i++ )
        {
			CCNode* child = (CCNode*)arrayData->arr[i];
			child->visit();
		}
        
	}
    else
    {
		this->draw();
    }

    this->afterDraw();
	if ( m_pGrid && m_pGrid->isActive())
    {
		m_pGrid->afterDraw(this);
    }

	kmGLPopMatrix();
}

bool CCScrollView::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    if (!this->isVisible() || !m_bScrollEnabled)
    {
        return false;
    }
    
    //dispatcher does not know about clipping. reject touches outside visible bounds.
    if (m_pTouches->count() > 2 ||
        m_bTouchMoved          ||
        !isTouchInside(touch->getLocation()))
    {
        return false;
    }

    if (!m_pTouches->containsObject(touch))
    {
        m_pTouches->addObject(touch);
    }

    if (scrollEnabledOnTouch())
    {
        if (m_pTouches->count() == 1)
        { // scrolling
            if (m_bForbidScrollWhenZoomedOut && (getZoomScale() <= m_fMinZoomScale))
            {
                return true;
            }
            
            m_tTouchPoint     = this->convertTouchToNodeSpace(touch);
            m_bTouchMoved     = false;
            m_bDragging     = true; //dragging started
            m_tScrollDistance = ccp(0.0f, 0.0f);
            m_fTouchLength    = 0.0f;
        }
        else if (m_pTouches->count() == 2)
        {
            m_tTouchPoint  = ccpMidpoint(this->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(0)),
                                         this->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(1)));
            m_fTouchLength = ccpDistance(m_pContainer->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(0)),
                                         m_pContainer->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(1)));
            m_bDragging  = false;
        }
    }

    return true;
}

void CCScrollView::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
    if (!this->isVisible() || !m_bScrollEnabled || !scrollEnabledOnTouch())
    {
        return;
    }

    if (m_pTouches->containsObject(touch))
    {
        if (m_pTouches->count() == 1 && m_bDragging)
        { // scrolling
            CCPoint moveDistance, newPoint, maxInset, minInset;
            CCRect  frame;
            float newX, newY;
            
            frame = getViewRect();

            newPoint     = this->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(0));
            moveDistance = ccpSub(newPoint, m_tTouchPoint);
            
            float dis = 0.0f;
            if (m_eDirection == kCCScrollViewDirectionVertical)
            {
                dis = moveDistance.y;
            }
            else if (m_eDirection == kCCScrollViewDirectionHorizontal)
            {
                dis = moveDistance.x;
            }
            else
            {
                dis = sqrtf(moveDistance.x*moveDistance.x + moveDistance.y*moveDistance.y);
            }

            if (!m_bTouchMoved && fabs(convertDistanceFromPointToInch(dis)) < MOVE_INCH )
            {
                //CCLOG("Invalid movement, distance = [%f, %f], disInch = %f", moveDistance.x, moveDistance.y);
                return;
            }
            
            if (!m_bTouchMoved)
            {
                moveDistance = CCPointZero;
            }
            
            m_tTouchPoint = newPoint;
            m_bTouchMoved = true;
            
            if (frame.containsPoint(this->convertToWorldSpace(newPoint)))
            {
                switch (m_eDirection)
                {
                    case kCCScrollViewDirectionVertical:
                        moveDistance = ccp(0.0f, moveDistance.y);
                        break;
                    case kCCScrollViewDirectionHorizontal:
                        moveDistance = ccp(moveDistance.x, 0.0f);
                        break;
                    default:
                        break;
                }
                
                maxInset = m_fMaxInset;
                minInset = m_fMinInset;

                newX     = m_pContainer->getPosition().x + moveDistance.x;
                newY     = m_pContainer->getPosition().y + moveDistance.y;

                m_tScrollDistance = moveDistance;
                this->setContentOffset(ccp(newX, newY));
            }
        }
        else if (m_pTouches->count() == 2 && !m_bDragging)
        {
            const float len = ccpDistance(m_pContainer->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(0)),
                                            m_pContainer->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(1)));
            this->setZoomScale(this->getZoomScale()*len/m_fTouchLength);
        }
    }
}

void CCScrollView::ccTouchEnded(CCTouch* touch, CCEvent* event)
{
    if (!this->isVisible() || !m_bScrollEnabled)
    {
        return;
    }
    
    if (m_pTouches->containsObject(touch))
    {
        if (m_pTouches->count() == 1 && m_bTouchMoved)
        {
            this->schedule(schedule_selector(CCScrollView::deaccelerateScrolling));
        }
        else
        {
            this->relocateContainer(true);
        }
        
        m_pTouches->removeObject(touch);
    } 

    if (m_pTouches->count() == 0)
    {
        m_bDragging = false;    
        m_bTouchMoved = false;
    }
}

void CCScrollView::ccTouchCancelled(CCTouch* touch, CCEvent* event)
{
    if (!this->isVisible() || !m_bScrollEnabled)
    {
        return;
    }
    m_pTouches->removeObject(touch);
    
    if (m_pTouches->count() == 0)
    {
        m_bDragging = false;    
        m_bTouchMoved = false;
    }
}

CCRect CCScrollView::getViewRect()
{
    CCPoint screenPos = this->convertToWorldSpace(CCPointZero);
    
    float scaleX = this->getScaleX();
    float scaleY = this->getScaleY();
    
    for (CCNode *p = m_pParent; p != NULL; p = p->getParent()) {
        scaleX *= p->getScaleX();
        scaleY *= p->getScaleY();
    }

    // Support negative scaling. Not doing so causes intersectsRect calls
    // (eg: to check if the touch was within the bounds) to return false.
    // Note, CCNode::getScale will assert if X and Y scales are different.
    if(scaleX<0.f) {
        screenPos.x += m_tViewSize.width*scaleX;
        scaleX = -scaleX;
    }
    if(scaleY<0.f) {
        screenPos.y += m_tViewSize.height*scaleY;
        scaleY = -scaleY;
    }

    return CCRectMake(screenPos.x, screenPos.y, m_tViewSize.width*scaleX, m_tViewSize.height*scaleY);
}

void CCScrollView::updateTweenAction(float value, const char* key)
{
    if (strcmp(key, "zoomScale") == 0)
    {
        setZoomScale(value);
    }
}

void CCScrollView::setDefaultScrollBarFactory(CCScrollBarFactory* iFactory)
{
    if (s_DefaultScrollBarFactory != iFactory)
    {
        CC_SAFE_RELEASE(s_DefaultScrollBarFactory);
        s_DefaultScrollBarFactory = iFactory;
        CC_SAFE_RETAIN(s_DefaultScrollBarFactory);
    }
}

void CCScrollView::setScrollBarFactory(CCScrollBarFactory* iFactory)
{
    if (m_ScrollBarFactory != iFactory)
    {
        CC_SAFE_RELEASE(m_ScrollBarFactory);
        m_ScrollBarFactory = iFactory;
        CC_SAFE_RETAIN(m_ScrollBarFactory);
    }
}

CCScrollBar::CCScrollBar(CCScrollView* iOwner, bool iIsHorizontal)
: m_Owner(iOwner), m_IsHorizontal(iIsHorizontal), m_InitialTouchPos(0), m_InitialScrollPos(0)
{
    
}

bool
CCScrollBar::init()
{
    if (CCControl::init())
    {
        setTouchEnabled(true);
        
        return true;
    }
    
    return false;
}

CCNode*
CCScrollBar::thumb()
{
    return this;
}

bool
CCScrollBar::isTouchInside(CCTouch* iTouch)
{
    CCPoint worldLoc = iTouch->getLocation();
    
    CCNode* t = thumb();
    
    CCPoint locInParentSpace = t->getParent()->convertToNodeSpace(worldLoc);
    CCRect bBox = t->boundingBox();
    return bBox.containsPoint(locInParentSpace);
}

bool
CCScrollBar::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    if (CCControl::ccTouchBegan(pTouch, pEvent))
    {
        return true;
    }
    
    if (!isTouchInside(pTouch))
    {
        return false;
    }
    
    CCPoint worldLoc = pTouch->getLocation();
    CCNode* t = thumb();
    CCPoint locInParentSpace = t->getParent()->convertToNodeSpace(worldLoc);
    
    const CCPoint scrollPos = m_Owner->getContentOffset();
    
    if (isHorizontal())
    {
        m_InitialTouchPos = locInParentSpace.x;
        m_InitialScrollPos = scrollPos.x;
    }
    else
    {
        m_InitialTouchPos = locInParentSpace.y;
        m_InitialScrollPos = scrollPos.y;
    }
    
    return true;
}

void
CCScrollBar::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    const CCSize& contentSize = m_Owner->getContentSize();
    const CCSize& viewSize = m_Owner->getViewSize();
    
    const float ratio = isHorizontal() ? (contentSize.width / viewSize.width) : (contentSize.height / viewSize.height);
    if (ratio < 1.f)
    {
        return;
    }
    
    CCPoint worldLoc = pTouch->getLocation();
    CCPoint locInParentSpace = getParent()->convertToNodeSpace(worldLoc);
    
    const float newPos = isHorizontal() ? locInParentSpace.x : locInParentSpace.y;
    
    const float delta = m_InitialTouchPos - newPos;
    
    float newScrollPos = m_InitialScrollPos + (delta * ratio);
    
    // clamp pos
    if (newScrollPos > 0)
    {
        newScrollPos = 0;
    }
    
    const float minScrollPos = isHorizontal() ? (viewSize.width - contentSize.width) : (viewSize.height - contentSize.height);
    if (newScrollPos < minScrollPos)
    {
        newScrollPos = minScrollPos;
    }
    
    CCPoint scrollPos = m_Owner->getContentOffset();
    if (isHorizontal())
    {
        scrollPos.x = newScrollPos;
    }
    else
    {
        scrollPos.y = newScrollPos;
    }
    
    m_Owner->setContentOffset(scrollPos);
}

static bool _computeScrollBarPosAndSize(float iViewSize, float iTotalSize, float iOffset, float& oPos, float& oBarSize)
{
    oPos = 0;
    oBarSize = iViewSize;
    
    if (iTotalSize == 0)
    {
        return false;
    }
    
    const float ratio = iViewSize / iTotalSize;
    if (ratio >= 1.f)
    {
        return false;
    }
    
    // Size
    oBarSize = ratio * iViewSize;
    
    // Position
    
    // Compute relative center
    const float posCoeff = -iOffset / iTotalSize;
    oPos = posCoeff * iViewSize;
    
    return true;
}


void
CCScrollBar::updatePositionAndSize()
{
    CCNode* container = m_Owner->getContainer();
    if (container != NULL)
    {
        const CCPoint& offset = container->getPosition();
        CCSize contentSize = container->getContentSize();
        contentSize.width *= container->getScaleX();
        contentSize.height *= container->getScaleY();
        
        const CCSize viewSize = m_Owner->getViewSize();
        
        const unsigned int flags = m_Owner->scrollBarsFlags();
        
        const bool alwaysVisible = isHorizontal() ? ((flags & CCScrollView::fAlwaysVisibleHorizontalScrollBar) == CCScrollView::fAlwaysVisibleHorizontalScrollBar) : ((flags & CCScrollView::fAlwaysVisibleVerticalScrollBar) == CCScrollView::fAlwaysVisibleVerticalScrollBar);
        
        float pos, size;
        if (_computeScrollBarPosAndSize(isHorizontal() ? viewSize.width : viewSize.height, isHorizontal() ? contentSize.width : contentSize.height, isHorizontal() ? offset.x : offset.y, pos, size) || alwaysVisible)
        {
            setThumbSize(size);
            setThumbPos(pos);
            
            CCPoint myPos = getPosition();
            if (isHorizontal())
            {
                const float margin = m_Owner->getHorizontalScrollBarMargin();
                if ((flags & CCScrollView::fTopHorizontalScrollBar) == CCScrollView::fTopHorizontalScrollBar)
                {
                    CCNode* t = thumb();
                    const CCSize& thumbSize = t->getContentSize();
                    myPos.y = viewSize.height - thumbSize.height - margin;
                }
                else
                {
                    myPos.y = margin;
                }
            }
            else
            {
                const float margin = m_Owner->getVerticalScrollBarMargin();
                if ((flags & CCScrollView::fLeftVerticalScrollBar) == CCScrollView::fLeftVerticalScrollBar)
                {
                    myPos.x = margin;
                }
                else
                {
                    CCNode* t = thumb();
                    const CCSize& thumbSize = t->getContentSize();
                    myPos.x = viewSize.width - thumbSize.width - margin;
                }
            }
            
            setPosition(myPos);
            
            setVisible(true);
        }
        else
        {
            setVisible(false);
        }
    }
    else
    {
        setVisible(false);
    }
}

void
CCScrollBar::setThumbPos(float iPos)
{
    auto* t = thumb();
    
    t->ignoreAnchorPointForPosition(true);
    
    CCPoint pos = t->getPosition();
    if (isHorizontal())
    {
        pos.x = iPos;
    }
    else
    {
        pos.y = iPos;
    }
    
    t->setPosition(pos);
}

void
CCScrollBar::setThumbSize(float iSize)
{
    CCNode* t = thumb();
    CCSize size = t->getContentSize();
    
    if (isHorizontal())
    {
        size.width = iSize;
    }
    else
    {
        size.height = iSize;
    }
    
    t->setContentSize(size);
}

CCScrollBar* CCScrollView::createScrollBar(bool iHorizontal)
{
    CCScrollBarFactory* factory = (m_ScrollBarFactory != NULL) ? m_ScrollBarFactory : s_DefaultScrollBarFactory;
    if (factory != NULL)
    {
        CCScrollBar* scrollBar = iHorizontal ? factory->createHorizontalScrollBar(this) : factory->createVerticalScrollBar(this);
        if (scrollBar != NULL)
        {
            // Watch out: call the fully featured one
            // otherwise it will indirectly call the one overriden in CCScrollView,
            // and then add the scrollbar to the container!
            CCLayer::addChild(scrollBar, m_ScrollBarZOrder, -1);
            return scrollBar;
        }
    }

    return NULL;
}

void
CCScrollView::setHorizontalScrollBarMargin(float iMargin)
{
    if (m_HorizontalScrollBarMargin != iMargin)
    {
        m_HorizontalScrollBarMargin = iMargin;
        updateScrollBarsPositions();
    }
}

void
CCScrollView::setVerticalScrollBarMargin(float iMargin)
{
    if (m_VerticalScrollBarMargin != iMargin)
    {
        m_VerticalScrollBarMargin = iMargin;
        updateScrollBarsPositions();
    }
}

void
CCScrollView::setScrollBarZOrder(int iZOrder)
{
    if (m_ScrollBarZOrder != iZOrder)
    {
        m_ScrollBarZOrder = iZOrder;
        
        if (m_HorizontalScrollBar != NULL)
        {
            m_HorizontalScrollBar->removeFromParentAndCleanup(true);
            CC_SAFE_RELEASE_NULL(m_HorizontalScrollBar);
        }
        
        if (m_VerticalScrollBar != NULL)
        {
            m_VerticalScrollBar->removeFromParentAndCleanup(true);
            CC_SAFE_RELEASE_NULL(m_VerticalScrollBar);
        }
        
        updateScrollBars();
    }
}

void CCScrollView::updateScrollBars()
{
    if ((m_ScrollBarsFlags & fHorizontalScrollBar) && scrollEnabled())
    {
        if (m_HorizontalScrollBar == NULL)
        {
            m_HorizontalScrollBar = createScrollBar(true);
            CC_SAFE_RETAIN(m_HorizontalScrollBar);
        }
    }
    else if (m_HorizontalScrollBar != NULL)
    {
        m_HorizontalScrollBar->removeFromParentAndCleanup(true);
        CC_SAFE_RELEASE_NULL(m_HorizontalScrollBar);
    }
    
    if ((m_ScrollBarsFlags & fVerticalScrollBar) && scrollEnabled())
    {
        if (m_VerticalScrollBar == NULL)
        {
            m_VerticalScrollBar = createScrollBar(false);
            CC_SAFE_RETAIN(m_VerticalScrollBar);
        }
    }
    else if (m_VerticalScrollBar != NULL)
    {
        m_VerticalScrollBar->removeFromParentAndCleanup(true);
        CC_SAFE_RELEASE_NULL(m_VerticalScrollBar);
    }
    
    updateScrollBarsPositions();
}

void CCScrollView::setScrollEnabled(bool iEnabled)
{
    if (iEnabled != m_bScrollEnabled)
    {
        m_bScrollEnabled = iEnabled;
        
        updateScrollBars();
    }
}

void CCScrollView::setScrollBarsFlags(unsigned int iScrollbarsFlags)
{
    if (m_ScrollBarsFlags != iScrollbarsFlags)
    {
        m_ScrollBarsFlags = iScrollbarsFlags;
        
        updateScrollBars();
    }
}

void CCScrollView::updateScrollBarsPositions()
{
    if (m_HorizontalScrollBar != NULL)
    {
        m_HorizontalScrollBar->updatePositionAndSize();
    }
    
    if (m_VerticalScrollBar != NULL)
    {
        m_VerticalScrollBar->updatePositionAndSize();
    }
}

NS_CC_EXT_END
