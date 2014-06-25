//
//  CCSmartLayer.cpp
//
//  Created by Gérald Guyomard on 10/10/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "layers_scenes_transitions_nodes/CCSmartLayer.h"


#include "DOWFoundation/NotificationCenter.h"
#include <algorithm>
#undef min
#undef max

#include "misc_nodes/CCTooltip.h"
#include "platform/CCCursor.h"

namespace cocos2d
{
    using namespace extension;
    using namespace DOW;
    
class CCSmartLayer::RenderTextureManager : public CCObject
{
public:
    static RenderTextureManager* instance() { return s_Instance; }
    static RenderTextureManager& createInstance();
    
    void registerLayer(CCSmartLayer* iLayer);
    void unregisterLayer(CCSmartLayer* iLayer);
    
    virtual void update(float iDt) override;
    
private:
    static RenderTextureManager* s_Instance;
    
    RenderTextureManager();
    virtual ~RenderTextureManager();
    
#ifndef _MSC_VER
	RenderTextureManager(const RenderTextureManager&) = delete;
#endif
    std::vector<CCSmartLayer*> fSmartLayers;
};

CCSmartLayer::RenderTextureManager* CCSmartLayer::RenderTextureManager::s_Instance = nullptr;

CCSmartLayer::RenderTextureManager&
CCSmartLayer::RenderTextureManager::createInstance()
{
    if (s_Instance == nullptr)
    {
        s_Instance = new RenderTextureManager;
        CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(s_Instance, -100, false);
        s_Instance->release();
    }
    
    return *s_Instance;
}
    
CCSmartLayer::RenderTextureManager::RenderTextureManager()
{
    assert(s_Instance == nullptr);
}

CCSmartLayer::RenderTextureManager::~RenderTextureManager()
{
	if (s_Instance == this)
	{
		// should not be reached
		s_Instance = nullptr;
	}
}

void
CCSmartLayer::RenderTextureManager::registerLayer(CCSmartLayer* iLayer)
{
    if (std::find(fSmartLayers.begin(), fSmartLayers.end(), iLayer) == fSmartLayers.end())
    {
        fSmartLayers.push_back(iLayer);
    }
}
    
void
CCSmartLayer::RenderTextureManager::unregisterLayer(CCSmartLayer* iLayer)
{
    const std::vector<CCSmartLayer*>::iterator it = std::find(fSmartLayers.begin(), fSmartLayers.end(), iLayer);
    if (it != fSmartLayers.end())
    {
        fSmartLayers.erase(it);
        
        if (fSmartLayers.empty())
        {
            s_Instance = nullptr;
            CCDirector::sharedDirector()->getScheduler()->unscheduleUpdateForTarget(this);
        }
    }
}

void
CCSmartLayer::RenderTextureManager::update(float iDt)
{
    assert(!fSmartLayers.empty());
    
    for (auto& layer : fSmartLayers)
    {
        layer->_updateRenderTexture();
    }
}

CCSmartLayer::CCSmartLayer()
: fPackedFlags(0), fOpacity(255), fSingleTouchMethod(nullptr)
{
    fColor = ccWHITE;
}
    
CCSmartLayer::~CCSmartLayer()
{
    if (fUseRenderTexture)
    {
        RenderTextureManager* manager = RenderTextureManager::instance();
        if (manager != nullptr)
        {
            manager->unregisterLayer(this);
        }
    }
    
    if (fTooltip != nullptr)
    {
        fTooltip->terminate();
    }
    
    _resetCustomCursor();
}
    
CCSmartLayer*
CCSmartLayer::create()
{
    CCSmartLayer* pRet = new CCSmartLayer();
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
    
void
CCSmartLayer::registerWithTouchDispatcher(void)
{
    CCTouchDispatcher* dispatcher = CCDirector::sharedDirector()->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, 0, true);
}

bool
CCSmartLayer::isTouchInside(const CCPoint& iLocation)
{
    return isTouchInsideBounds(this, iLocation);
}

void
CCSmartLayer::setSingleTouchMethod(const std::function<bool (CCSmartLayer* iLayer, TouchSequence iTouchSequence, CCTouch *pTouch, CCEvent *pEvent)>& iMethod)
{
    fSingleTouchMethod = iMethod;
}
    
bool
CCSmartLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    CCTooltip::hideCurrentTooltip();
    
    if (isClickThrough())
    {
        return false;
    }
    
    if (fSingleTouchMethod != nullptr)
    {
        return fSingleTouchMethod(this, TouchSequence::Began, pTouch, pEvent);
    }
    
    const CCPoint loc = pTouch->getLocation();

	if (!isTouchInside(loc))
    {
        return false;
    }

    return true;
}
    
void
CCSmartLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    if (fSingleTouchMethod != nullptr)
    {
        fSingleTouchMethod(this, TouchSequence::Moved, pTouch, pEvent);
    }
}
    
void
CCSmartLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    if (fSingleTouchMethod != nullptr)
    {
        fSingleTouchMethod(this, TouchSequence::Ended, pTouch, pEvent);
    }
}
    
void
CCSmartLayer::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
    if (fSingleTouchMethod != nullptr)
    {
        fSingleTouchMethod(this, TouchSequence::Cancelled, pTouch, pEvent);
    }
}
   
bool
CCSmartLayer::canHandleMouseOver()
{
    return isEnabled();
}
    
bool
CCSmartLayer::isMouseOver(const CCPoint& iWorldMouseLocation)
{
    return isTouchInside(iWorldMouseLocation);
}

void
CCSmartLayer::onMouseEnter(const CCPoint& iWorldMouseLocation)
{
    assert(!fHasChangedMouseCursorOnMouseOver);
    
    if (fTooltip != nullptr)
    {
        fTooltip->onMouseEnter(iWorldMouseLocation);
    }
    
    CCCursorManager* manager = CCCursorManager::sharedCursorManager();
    if (manager != nullptr)
    {
        CCCursor* cursor = _cursor();
        if (cursor != nullptr)
        {
            manager->push(cursor);
            fHasChangedMouseCursorOnMouseOver = true;
        }
    }
}

void
CCSmartLayer::onMouseOver(const CCPoint& iWorldMouseLocation)
{
    if (fTooltip != nullptr)
    {
        fTooltip->onMouseOver(iWorldMouseLocation);
    }
}

void
CCSmartLayer::onMouseExit(const CCPoint& iWorldMouseLocation)
{
    if (fTooltip != nullptr)
    {
        fTooltip->onMouseExit(iWorldMouseLocation);
    }
    
    _resetCustomCursor();
}

void
CCSmartLayer::_resetCustomCursor()
{
    if (fHasChangedMouseCursorOnMouseOver)
    {
        fHasChangedMouseCursorOnMouseOver = false;
        
        CCCursorManager* manager = CCCursorManager::sharedCursorManager();
        assert(manager != nullptr);
        manager->pop();
    }
}
    
void
CCSmartLayer::setTooltipText(const std::string& iText)
{
    if (fTooltip != nullptr)
    {
        fTooltip->terminate();
    }
    
    fTooltip = CCTextTooltip::create(iText);
}

void
CCSmartLayer::setTooltipLocalizedTextKey(const std::string& iKey)
{
    if (fTooltip != nullptr)
    {
        fTooltip->terminate();
    }
    
    fTooltip = CCTextTooltip::createWithLocalizedTextKey(iKey);
}


void
CCSmartLayer::onNodeLoaded(CCNode * pNode, CCNodeLoader * pNodeLoader)
{
    if (fSaveOpacitiesAfterLoad)
    {
        saveOpacities();
    }
}
    
bool
CCSmartLayer::isEnabled()
{
    return !fDisabled;
}

void
CCSmartLayer::setIsEnabled(bool iEnabled)
{
    const bool disabled = !iEnabled;
    
    if (disabled != fDisabled)
    {
        fDisabled = disabled;
        
        if (!fDisabled)
        {
            if (isTouchEnabled() && isRunning())
            {
                registerWithTouchDispatcher();
            }
        }
        else
        {
            CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        }
    }
}
    
void
CCSmartLayer::setColor(const ccColor3B& color)
{
    if (color != fColor)
    {
        fColor = color;
        
        _registerUpdatingRenderTexture();
        
        if (fRenderTexture != nullptr)
        {
            fRenderTexture->setColor(fColor);
        }
    }
}
    
const ccColor3B&
CCSmartLayer::getColor(void)
{
    return fColor;
}

GLubyte
CCSmartLayer::getOpacity(void)
{
    return fOpacity;
}
    
void
_SetOpacity(CCNode* iNode, GLubyte iOpacity)
{
    CCRGBAProtocol* protocol = dynamic_cast<CCRGBAProtocol*>(iNode);
    if (protocol != NULL)
    {
        protocol->setOpacity(iOpacity);
    }
    
    CCArray* children = iNode->getChildren();
    const uint32_t count = (children != NULL) ? children->count() : 0;
    
    for (uint32_t i=0; i < count; ++i)
    {
        CCNode* child = (CCNode*) children->objectAtIndex(i);
        _SetOpacity(child, iOpacity);
    }
}
    
void
CCSmartLayer::_saveOpacities(CCNode* iNode)
{
    if (iNode != this)
    {
        CCRGBAProtocol* protocol = dynamic_cast<CCRGBAProtocol*>(iNode);
        if (protocol != NULL)
        {
            const GLubyte opacity = protocol->getOpacity();
            if (opacity != 255)
            {
                if (fDescendantToOpacity == nullptr)
                {
                    fDescendantToOpacity.reset(new TDescendantToOpacity);
                }
                
                (*fDescendantToOpacity)[iNode] = opacity;
            }
        }
    }
    
    CCArray* children = iNode->getChildren();
    const uint32_t count = (children != NULL) ? children->count() : 0;
    
    for (uint32_t i=0; i < count; ++i)
    {
        CCNode* child = (CCNode*) children->objectAtIndex(i);
        _saveOpacities(child);
    }    
}
    
void
CCSmartLayer::_modifyOpacity(CCNode* iNode, GLubyte iGlobalOpacity)
{
    if (iNode != this)
    {
        CCRGBAProtocol* protocol = dynamic_cast<CCRGBAProtocol*>(iNode);
        if (protocol != NULL)
        {
            if (!fDescendantToOpacity)
            {
                protocol->setOpacity(iGlobalOpacity);
            }
            else
            {
                TDescendantToOpacity::iterator it = fDescendantToOpacity->find(iNode);
                if (it != fDescendantToOpacity->end())
                {
                    const float childOpacity = it->second;
                    const float multipliedOpacity = (childOpacity * float(iGlobalOpacity)) / 255.f;
                    assert(multipliedOpacity >= 0);
                    assert(multipliedOpacity <= 255);
                    
                    protocol->setOpacity(GLubyte(multipliedOpacity));
                }
                else
                {
                    protocol->setOpacity(iGlobalOpacity);
                }                
            }
        }
    }
    
    CCArray* children = iNode->getChildren();
    const uint32_t count = (children != NULL) ? children->count() : 0;

    for (uint32_t i=0; i < count; ++i)
    {
        CCNode* child = (CCNode*) children->objectAtIndex(i);
        _modifyOpacity(child, iGlobalOpacity);
    }
}

void
CCSmartLayer::saveOpacities()
{
    fDescendantToOpacity.reset();
    _saveOpacities(this);    
}
    
void
CCSmartLayer::setOpacity(GLubyte opacity)
{
    if (fOpacity != opacity)
    {
        fOpacity = opacity;
        
        _registerUpdatingRenderTexture();
        
        if (fUseRenderTexture)
        {
            if (fRenderTexture != nullptr)
            {
                fRenderTexture->setOpacity(opacity);
            }
        }
        else
        {
            _modifyOpacity(this, opacity);
        }
    }
}
    
void
CCSmartLayer::setVisible(bool visible)
{
    const bool wasVisible = isVisible();
    
    SuperClass::setVisible(visible);
    
    const bool nowVisible = isVisible();
    
    if (wasVisible != nowVisible)
    {
        _registerUpdatingRenderTexture();
        
        if (nowVisible)
        {
            _discardNonDescendantNatives();
        }
        else
        {
            _restoreNonDescendantNatives();
        }
    }
}

void
CCSmartLayer::_registerUpdatingRenderTexture()
{
    if (fUseRenderTexture && (fOpacity != 0) && (fOpacity != 255) && isVisible())
    {
        RenderTextureManager::createInstance().registerLayer(this);
    }
    else
    {
        invalidateRenderTexture();
        
        RenderTextureManager* manager = RenderTextureManager::instance();
        if (manager != nullptr)
        {
            manager->unregisterLayer(this);
        }
    }
}
    
void
CCSmartLayer::useRenderTexture(bool iUse)
{
    if (iUse != fUseRenderTexture)
    {
        fUseRenderTexture = iUse;
        
        _registerUpdatingRenderTexture();
    }
}
    
static bool isRectEmpty(const CCRect& iRect)
{
    return (iRect.size.width == 0) || (iRect.size.height == 0);
}
    
static void _computeLocalBoundingBox(CCNode* iRoot, CCRect& oBox)
{
    oBox.setRect(0, 0, 0, 0);
    
    if (iRoot->isVisible())
    {
        CCRGBAProtocol* rgbaProtocol = dynamic_cast<CCRGBAProtocol*>(iRoot);
        if ((rgbaProtocol == nullptr) || (rgbaProtocol->getOpacity() != 0))
        {
            CCSmartLayer* smartLayer = dynamic_cast<CCSmartLayer*>(iRoot);
            if ((smartLayer == nullptr) || !smartLayer->isSelfDiscardedInRenderTexture())
            {
                oBox.setRect(0,0,iRoot->getContentSize().width, iRoot->getContentSize().height);
            }
            
            CCRect childBox;
            CCObject* child;
            CCARRAY_FOREACH(iRoot->getChildren(), child)
            {
                CCNode* childNode = (CCNode*) child;
                
                _computeLocalBoundingBox(childNode, childBox);
                
                if (!isRectEmpty(childBox))
                {
                    CCAffineTransform t = childNode->nodeToParentTransform();
                    childBox = CCRectApplyAffineTransform(childBox, t);
                    
                    if (isRectEmpty(oBox))
                    {
                        oBox = childBox;
                    }
                    else
                    {
                        // Union of oBox and child box
                        const float x1 = std::min(oBox.getMinX(), childBox.getMinX());
                        const float y1 = std::min(oBox.getMinY(), childBox.getMinY());
                        const float x2 = std::max(oBox.getMaxX(), childBox.getMaxX());
                        const float y2 = std::max(oBox.getMaxY(), childBox.getMaxY());
                        
                        oBox.setRect(x1, y1, x2 - x1, y2 - y1);
                    }
                }            
            }            
        }
    }
}
    
void
CCSmartLayer::_updateRenderTexture()
{
    assert(isUsingRenderTexture());
    
    if (fRenderTexture == nullptr)
    {
        if ((fOpacity != 0) && (fOpacity != 255))
        {
            CCRect renderBox;
            _computeLocalBoundingBox(this, renderBox);

            if ((renderBox.size.width > 0) && (renderBox.size.height > 0))
            {                
                renderBox.size.width = ceilf(renderBox.size.width);
                renderBox.size.height = ceilf(renderBox.size.height);
                
                fRenderOffset = renderBox.origin;
                
                CCObjectPtr<RenderTexture> renderTexture = RenderTexture::create(renderBox.size.width, renderBox.size.height);
                RenderTexture::GfxContext* ctx = renderTexture->beginLocalWithClear(0, 0, 0, 0, true /*Flip Y*/);
                
                kmGLTranslatef(-fRenderOffset.x, -fRenderOffset.y, 0);
                _visitStandard();
                
                renderTexture->endLocal(ctx);
                                
                fRenderTexture = renderTexture->getSprite();                
            }

            fRenderTexture->setOpacity(fOpacity);
            fRenderTexture->setColor(fColor);
        }
    }
}
    
void
CCSmartLayer::invalidateRenderTexture()
{
    if (fRenderTexture != nullptr)
    {
        fRenderTexture->removeAllChildrenWithCleanup(true);
        fRenderTexture = nullptr;        
    }
}
    
void
CCSmartLayer::setOpacityModifyRGB(bool bValue)
{
    DOW_NOT_IMPLEMENTED();
}
    
bool
CCSmartLayer::isOpacityModifyRGB(void)
{
    DOW_NOT_IMPLEMENTED();
    return false;
}
    
void
CCSmartLayer::setClipToBounds(bool iClipToBounds)
{
    fClipToBounds = iClipToBounds;
}
    
static bool
_ancestorUsingRenderTexture(CCNode* iNode)
{
    while (iNode != nullptr)
    {
        CCSmartLayer* layer = dynamic_cast<CCSmartLayer*>(iNode);
        if ((layer != nullptr) && layer->isUsingRenderTexture())
        {
            return true;
        }
        
        iNode = iNode->getParent();
    }
    
    return false;
}
    
void
CCSmartLayer::visit()
{    
    // quick return if not visible. children won't be drawn.
    if (!m_bVisible)
    {
        return;
    }

    kmGLPushMatrix();
    
    if (m_pGrid && m_pGrid->isActive())
    {
        m_pGrid->beforeDraw();
    }
    
    this->transform();
    
    const bool clipToBounds = fClipToBounds && !_ancestorUsingRenderTexture(getParent());
    bool wasClippingEnabled;
    GLint oldScissorBox[4];
    
    if (clipToBounds)
    {
        wasClippingEnabled = glIsEnabled(GL_SCISSOR_TEST);
        if (!wasClippingEnabled)
        {
            glEnable(GL_SCISSOR_TEST);
        }
        else
        {
            // save current values
            glGetIntegerv(GL_SCISSOR_BOX, oldScissorBox);
        }
        
        CCRect worldBox = CCRectApplyAffineTransform(boundingBox(), getParent()->nodeToWorldTransform());
        
        CCEGLView::sharedOpenGLView()->setScissorInPoints(worldBox.origin.x, worldBox.origin.y, worldBox.size.width, worldBox.size.height);
    }
    
    if (isUsingRenderTexture())
    {
        if (fRenderTexture != nullptr)
        {
            kmGLTranslatef(fRenderOffset.x, fRenderOffset.y, 0);
            fRenderTexture->draw();
        }
        else if (fOpacity != 0)
        {
            _visitStandard();
        }
    }
    else if (fOpacity != 0)
    {
        // No render texture, standard recursive rendering
        _visitStandard();
    }
    
    if (m_pGrid && m_pGrid->isActive())
    {
        m_pGrid->afterDraw(this);
    }

    if (clipToBounds)
    {
        if (wasClippingEnabled)
        {
            // Restore previous clipping
            glScissor(oldScissorBox[0], oldScissorBox[1], oldScissorBox[2], oldScissorBox[3]);
        }
        else
        {
            glDisable(GL_SCISSOR_TEST);
        }        
    }
    
    kmGLPopMatrix();
}
   
void
CCSmartLayer::_visitStandard()
{
    CCNode* pNode = NULL;
    unsigned int i = 0;
    
    if(m_pChildren && m_pChildren->count() > 0)
    {
        sortAllChildren();
        // draw children zOrder < 0
        ccArray *arrayData = m_pChildren->data;
        for( ; i < arrayData->num; i++ )
        {
            pNode = (CCNode*) arrayData->arr[i];
            
            if ( pNode && pNode->getZOrder() < 0 )
            {
                _visitChild(pNode);
            }
            else
            {
                break;
            }
        }
        // self draw
        this->draw();
        
        for( ; i < arrayData->num; i++ )
        {
            pNode = (CCNode*) arrayData->arr[i];
            if (pNode)
            {
                _visitChild(pNode);
            }
        }
    }
    else
    {
        this->draw();
    }
    
    // reset for next frame

    m_uOrderOfArrival = 0;   
}
    
void
CCSmartLayer::_visitChild(cocos2d::CCNode *iChild)
{
    iChild->visit();
}
   
void
CCSmartLayer::onEnter(void)
{
    SuperClass::onEnter();
    
    _discardNonDescendantNatives();
}
    
void
CCSmartLayer::onEnterTransitionDidFinish()
{
    SuperClass::onEnterTransitionDidFinish();

    _discardNonDescendantNatives();
}
    
void
CCSmartLayer::onExit()
{
    SuperClass::onExit();
    
    _restoreNonDescendantNatives();
}
    
void
CCSmartLayer::onExitTransitionDidStart()
{
    SuperClass::onExitTransitionDidStart();
    
    _restoreNonDescendantNatives();
}

void
CCSmartLayer::_discardNonDescendantNatives()
{
    if (fShouldDiscardNonDescendantNativeNodes && !fIsCurrentlyDiscardingNonDescendantNativeNodes)
    {
        fIsCurrentlyDiscardingNonDescendantNativeNodes = true;
        
        NotificationCenter::defaultNotificationCenter().postNotification(kShouldDiscardNonDescendantNativeNodesNotifID, (void*) this);
    }
}
    
void
CCSmartLayer::_restoreNonDescendantNatives()
{
    if (fShouldDiscardNonDescendantNativeNodes && fIsCurrentlyDiscardingNonDescendantNativeNodes)
    {
        fIsCurrentlyDiscardingNonDescendantNativeNodes = false;
        
        NotificationCenter::defaultNotificationCenter().postNotification(kShouldRestoreNonDescendantNativeNodesNotifID, (void*) this);
    }
    
}
    
NS_CC_END
