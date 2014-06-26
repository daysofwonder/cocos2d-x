//
//  CCLayout.cpp
//  SmallWorld2
//
//  Created by Gérald Guyomard on 10/10/12.
//
//

#include "CCLayout.h"
#include "CCBReader/CCNode+CCBRelativePositioning.h"
#include "DOWFoundation/StringUtils.h"

#include <algorithm>
#undef min
#undef max

NS_CC_BEGIN

using namespace DOW;

    CCLayout::PositionAttributes::PositionAttributes()
    : fType(kCCBPositionTypeRelativeBottomLeft)
    {}
    
    CCLayout::SizeAttributes::SizeAttributes()
    : fType(kCCBSizeTypeAbsolute)
    {}
    
    CCLayout::Attributes::Attributes()
    {}
    
    CCLayout::CCLayout()
	: fAutoScaleMode(eNoAutoScale), fSiblingTagToFitScale(-1), fPackedFlags(0)
    {

    }
        
    CCLayout::~CCLayout()
    {
    }
        
    CCLayout*
    CCLayout::create()
    {
        CCLayout* pRet = new CCLayout();
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
    CCLayout::setAutoScaleMode(EAutoScaleMode iMode)
    {
        if (iMode != fAutoScaleMode)
        {
            fAutoScaleMode = iMode;
            needsLayout();
        }
    }
    
    void
    CCLayout::setSiblingTagToFitScale(int iSiblingTag)
    {
        if (fSiblingTagToFitScale != iSiblingTag)
        {
            fSiblingTagToFitScale = iSiblingTag;
            needsLayout();
        }
    }
    
    void
    CCLayout::setResizeToFitParentBounds(bool iResize)
    {
        if (iResize != fResizeToFitParentBounds)
        {
            fResizeToFitParentBounds = iResize;
            needsLayout();
        }
    }
    
    CCLayout::EAutoScaleMode
    CCLayout::childAutoScaleMode(int iChildTag) const
    {
        const auto it = fChildTagToAutoScaleMode.find(iChildTag);
        return (it != fChildTagToAutoScaleMode.end()) ? it->second : eNoAutoScale;
    }
    
    void
    CCLayout::setChildAutoScaleMode(EAutoScaleMode iMode, int iChildTag)
    {
        const auto oldAutoScaleMode = childAutoScaleMode(iChildTag);
        if (oldAutoScaleMode != iMode)
        {
            if (iMode == eNoAutoScale)
            {
                fChildTagToAutoScaleMode.erase(iChildTag);
            }
            else
            {
                fChildTagToAutoScaleMode[iChildTag] = iMode;
            }
            
            needsLayout();
        }
    }
    
    void
    CCLayout::needsLayout()
    {
        fContainerSize.setSize(0, 0);
    }
    
    static void _applyAutoScale(CCNode* iNode, CCLayout::EAutoScaleMode iMode, const CCSize& iContainerSize)
    {
        if (iMode != CCLayout::eNoAutoScale)
        {
            const CCSize& nodeSize = iNode->getContentSize();
            
            const float fX = iContainerSize.width / nodeSize.width;
            const float fY = iContainerSize.height / nodeSize.height;
            
            const float scale = iMode == (iMode == CCLayout::eAutoScaleToRevealAll) ? std::min(fX, fY) : std::max(fX, fY);
            iNode->setScale(scale);
        }
    }
    
    void
    CCLayout::_layoutNode(CCNode* iNode)
    {
        const auto* attr = _attributes(iNode);
        if (attr != nullptr)
        {
            CCNode* parent = iNode->getParent();
            const CCSize& containerSize = parent->getContentSize();
            
			if ((iNode != this) || !fContentSizeLocked)
			{
				const auto& sizeAttr = attr->fSizeAttrs;
				if (sizeAttr != nullptr)
				{
					const CCSize newSize = getAbsoluteSize(sizeAttr->fSize, sizeAttr->fType, containerSize);
					iNode->setContentSize(newSize);
				}
			}
                        
            const auto& posAttr = attr->fPosAttrs;
            if (posAttr != nullptr)
            {
                const CCPoint newPos = getAbsolutePosition(posAttr->fPosition, posAttr->fType, containerSize, nullptr);
                iNode->setPosition(newPos);
            }
        }
    }
    
    void
    CCLayout::_layout()
    {
        assert(!fLayoutInProgress);
        fLayoutInProgress = true;
        
        _layoutNode(this);
        
        CCNode* parent = getParent();
        
        if (fSiblingTagToFitScale >= 0)
        {
            CCNode* sibling = parent->getChildByTag(fSiblingTagToFitScale);
            if (sibling != nullptr)
            {
                const float s = sibling->getScale();
                setScale(s);
            }
        }
        else
        {
            _applyAutoScale(this, fAutoScaleMode, parent->getContentSize());            
        }
        
        if (fResizeToFitParentBounds)
        {
            const CCSize& parentSize = parent->getContentSize();
            CCRect box = boundingBox();
            
            const float sX = parentSize.width / box.size.width;
            const float sY = parentSize.height / box.size.height;
            
            CCSize contentSize = getContentSize();
            contentSize.width *= sX;
            contentSize.height *= sY;
            
            setContentSize(contentSize);
        }
        
        if(m_pChildren && m_pChildren->count() > 0)
        {
            const CCSize& containerSize = getContentSize();
            ccArray *arrayData = m_pChildren->data;
            for(int i = 0 ; i < arrayData->num; i++ )
            {
                CCNode* child = (CCNode*) arrayData->arr[i];
                _layoutNode(child);
                _applyAutoScale(child, childAutoScaleMode(child->getTag()), containerSize);
            }
        }
        
        fLayoutInProgress = false;
    }


    void
    CCLayout::setPositionAttributes(const CCPoint& iPosition, CCBPositionType nType)
    {
        _setPositionAttributes(this, iPosition, nType);
    }
    
    void
    CCLayout::setSizeAttributes(const CCSize& iSize, CCBSizeType nType)
    {
        _setSizeAttributes(this, iSize, nType);
    }

    void
    CCLayout::onChildPositionParsed(CCNode * pNode, CCNode* pChild, const CCPoint& pos, CCBPositionType ntype, const CCSize& containerSize, const char* pPropertyName)
    {
        assert(pNode == this);
        if (dynamic_cast<CCLayout*>(pChild) == 0)
        {
            // Not a layout
            _setPositionAttributes(pChild, pos, ntype);
        }
    }
    
    void
    CCLayout::onChildSizeParsed(CCNode * pNode, CCNode* pChild, const CCSize& size, CCBSizeType ntype, const CCSize& containerSize)
    {
        assert(pNode == this);
        if (dynamic_cast<CCLayout*>(pChild) == 0)
        {
            // Not a layout
            _setSizeAttributes(pChild, size, ntype);
        }
    }
    
    void
    CCLayout::_layoutIfNeeded()
    {
        CCNode* container = getParent();
        if (container != nullptr)
        {
            const CCSize& containerSize = container->getContentSize();
            if (containerSize != fContainerSize)
            {
                fContainerSize = containerSize;
                _layout();
            }
        }
    }
    
    void
    CCLayout::setContentSize(const CCSize& iNewSize)
    {
        if (fLayoutInProgress)
        {
            SuperClass::setContentSize(iNewSize);
        }
        else
        {
			// We're called from outside
			fContentSizeLocked = true;

            const bool sizeChanged = !iNewSize.equals(getContentSize());
            
            SuperClass::setContentSize(iNewSize);
            
            if (sizeChanged)
            {
                needsLayout();
            }
            
            _layoutIfNeeded();

			fContentSizeLocked = false;
        }
    }
    
    void
    CCLayout::visit()
    {
        _layoutIfNeeded();
        
        SuperClass::visit();
    }
    
    void
    CCLayout::onEnter()
    {
        SuperClass::onEnter();
        
        _registerUpdate();
    }
    
    void
    CCLayout::onExit(void)
    {
        SuperClass::onExit();
        
        _unregisterUpdate();
    }

    void
    CCLayout::update(float dT)
    {
        SuperClass::update(dT);
        
        _layoutIfNeeded();
    }
    
    void
    CCLayout::_registerUpdate()
    {
        CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(this, -1000, false);
    }
    
    void
    CCLayout::_unregisterUpdate()
    {
        CCDirector::sharedDirector()->getScheduler()->unscheduleUpdateForTarget(this);
    }

	void
	CCLayout::setChildPositionAttributes(CCNode* iChild, const CCPoint& iPosition, CCBPositionType ntype)
	{
		assert(iChild->getParent() == this);

		_setPositionAttributes(iChild, iPosition, ntype);
	}

	void
	CCLayout::setChildSizeAttributes(CCNode* iChild, const CCSize& iSize, CCBSizeType ntype)
	{
		assert(iChild->getParent() == this);

		_setSizeAttributes(iChild, iSize, ntype);
	}

    void
    CCLayout::_setPositionAttributes(CCNode* iNode, const CCPoint& iPosition, CCBPositionType nType)
    {
        auto& attrs = _createPositionAttributes(iNode);
        if ((attrs.fPosition != iPosition) || (attrs.fType != nType))
        {
            attrs.fPosition = iPosition;
            attrs.fType = nType;
            
            needsLayout();
        }
    }
    
    void
    CCLayout::_setSizeAttributes(CCNode* iNode, const CCSize& iSize, CCBSizeType nType)
    {
        if ((iSize.width == 0) && (iSize.height == 0))
        {
            return;
        }
        
        auto& attrs = _createSizeAttributes(iNode);
        if ((attrs.fSize != iSize) || (attrs.fType != nType))
        {
            attrs.fSize = iSize;
            attrs.fType = nType;
            
            needsLayout();
        }        
    }
    

    
    CCLayout::PositionAttributes*
    CCLayout::_positionAttributes(const CCNode* iNode)
    {
        auto* attr = _attributes(iNode);
        return (attr != nullptr) ? attr->fPosAttrs.get() : nullptr;
    }
    
    CCLayout::PositionAttributes&
    CCLayout::_createPositionAttributes(const CCNode* iNode)
    {
        auto& attrs = _createAttributes(iNode);
        if (attrs.fPosAttrs == nullptr)
        {
            attrs.fPosAttrs.reset(new PositionAttributes);
        }
        
        return *attrs.fPosAttrs;
    }

    
    CCLayout::SizeAttributes*
    CCLayout::_sizeAttributes(const CCNode* iNode)
    {
        auto* attr = _attributes(iNode);
        return (attr != nullptr) ? attr->fSizeAttrs.get() : nullptr;

    }
    
    CCLayout::SizeAttributes&
    CCLayout::_createSizeAttributes(const CCNode* iNode)
    {
        auto& attrs = _createAttributes(iNode);
        if (attrs.fSizeAttrs == nullptr)
        {
            attrs.fSizeAttrs.reset(new SizeAttributes);
        }
        
        return *attrs.fSizeAttrs;
        
    }
    
    CCLayout::Attributes*
    CCLayout::_attributes(const CCNode* iNode)
    {
        const auto it = fNodeToAttributes.find(iNode);
        if (it != fNodeToAttributes.end())
        {
            return it->second.get();
        }
        else
        {
            return nullptr;
        }
    }
    
    CCLayout::Attributes&
    CCLayout::_createAttributes(const CCNode* iNode)
    {
        auto& attrs = fNodeToAttributes[iNode];
        if (attrs == nullptr)
        {
            attrs.reset(new Attributes);
        }
        
        return *attrs;
    }

NS_CC_END

NS_CC_EXT_BEGIN

void
CCLayoutLoader::onHandlePropTypeInteger(CCNode * pNode, CCNode * pParent, const char* pPropertyName, int pInteger, CCBReader * pCCBReader)
{
    CCLayout* node = static_cast<CCLayout*>(pNode);
    assert(dynamic_cast<CCLayout*>(pNode) != nullptr);
    
    if (::strcmp(pPropertyName, "siblingTagToFitScale") == 0)
    {
        node->setSiblingTagToFitScale(pInteger);
    }
    else
    {
        SuperClass::onHandlePropTypeInteger(pNode, pParent, pPropertyName, pInteger, pCCBReader);
    }
}

void
CCLayoutLoader::onHandlePropTypeCheck(CCNode * pNode, CCNode * pParent, const char* pPropertyName, bool pCheck, CCBReader * pCCBReader)
{
    CCLayout* node = static_cast<CCLayout*>(pNode);
    assert(dynamic_cast<CCLayout*>(pNode) != nullptr);
    
    if (::strcmp(pPropertyName, "resizeToFitParentBounds") == 0)
    {
        node->setResizeToFitParentBounds(pCheck);
    }
    else
    {
        SuperClass::onHandlePropTypeCheck(pNode, pParent, pPropertyName, pCheck, pCCBReader);
    }
    
}

static const std::string kAutoScaleModePrefix = "autoScaleMode";
static const std::string kAutoScaleRevealAll = "autoScaleToRevealAll";
static const std::string kAutoScaleWithNoBorder = "autoScaleWithNoBorder";

void
CCLayoutLoader::onHandlePropTypeString(CCNode * pNode, CCNode * pParent, const char* pPropertyName, const char * pString, CCBReader * pCCBReader)
{
    CCLayout* node = static_cast<CCLayout*>(pNode);
    assert(dynamic_cast<CCLayout*>(pNode) != nullptr);
    
    const std::string propertyName = pPropertyName;
    
    if (StringUtils::startsWith(propertyName, kAutoScaleModePrefix))
    {
        const std::string value = pString;
        
        CCLayout::EAutoScaleMode mode = CCLayout::eNoAutoScale;
        
        if (value == kAutoScaleRevealAll)
        {
            mode = CCLayout::eAutoScaleToRevealAll;
        }
        else if (value == kAutoScaleWithNoBorder)
        {
            mode = CCLayout::eAutoScaleWithNoBorder;
        }
        
        // Determining target
        const auto pos = propertyName.find(':');
        if (pos == std::string::npos)
        {
            node->setAutoScaleMode(mode);
        }
        else
        {
            std::string tagAsString = propertyName.substr(pos + 1);
            const int tag = StringUtils::readInt32(tagAsString);
            node->setChildAutoScaleMode(mode, tag);
        }
    }
    else
    {
        SuperClass::onHandlePropTypeString(pNode, pParent, pPropertyName, pString, pCCBReader);
    }
}

CCPoint
CCLayoutLoader::processPropTypePosition(CCNode* pNode, CCNode* pParent, const CCPoint& pos, int ntype, const CCSize& containerSize, const char* pPropertyName)
{
    CCLayout* layout = dynamic_cast<CCLayout*>(pNode);
    assert(layout != nullptr);
    
    layout->setPositionAttributes(pos, (CCBPositionType) ntype);
    
    return SuperClass::processPropTypePosition(pNode, pParent, pos, ntype, containerSize, pPropertyName);
}

CCSize
CCLayoutLoader::processPropTypeSize(CCNode* pNode, CCNode* pParent, const CCSize& size, int ntype, const CCSize& containerSize)
{
    CCLayout* layout = dynamic_cast<CCLayout*>(pNode);
    assert(layout != nullptr);
    
    layout->setSizeAttributes(size, (CCBSizeType) ntype);
    
    return SuperClass::processPropTypeSize(pNode, pParent, size, ntype, containerSize);
}

NS_CC_EXT_END
