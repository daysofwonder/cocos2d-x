//
//  CCEnhancedLabelTTF.cpp
//
//  Created by Gérald Guyomard on 20/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "label_nodes/CCEnhancedLabelTTF.h"
#include "ccTypes.h"

#include "cocoa/CCObjectPtr.h"


NS_CC_BEGIN

    CCEnhancedLabelTTF*
    CCEnhancedLabelTTF::create()
    {
        CCEnhancedLabelTTF* pRet = new CCEnhancedLabelTTF();
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

    CCEnhancedLabelTTF::CCEnhancedLabelTTF()
    : fShadowColor(ccc4(0, 0, 0, 0)), fOutlineColor(ccc4(0, 0, 0, 0)), fOutlineThickness(0)
    {}

    CCEnhancedLabelTTF::~CCEnhancedLabelTTF()
    {}

    void
    CCEnhancedLabelTTF::setLocalizedStringKey(const std::string& iKey)
    {
        const std::string* value = CCStringPool::sharedLocalizedStringPool().stdStringForKeyIfFound(iKey);

        if (value != nullptr)
        {
            setString(value->c_str());
        }
        else
        {
            std::string missingValue = "$" + iKey;
            setString(missingValue.c_str());
        }
    }

    void
    CCEnhancedLabelTTF::setTexture(CCTexture2D* iTexture)
    {
        const bool textureChanged = (iTexture != getTexture());

        SuperClass::setTexture(iTexture);

        if (textureChanged)
        {
            _updateShadow();
            _updateOutline();
        }
    }

    void
    CCEnhancedLabelTTF::setShadowColor(const ccColor4B& iShadowColor)
    {
        if (iShadowColor != fShadowColor)
        {
            fShadowColor = iShadowColor;
            _updateShadow();
        }
    }

    void
    CCEnhancedLabelTTF::setShadowOffset(const CCSize& iOffset)
    {
        if (iOffset != fShadowOffset)
        {
            fShadowOffset = iOffset;
            _updateShadow();
        }
    }

    // Outline management
    void
    CCEnhancedLabelTTF::setOutlineColor(const ccColor4B& iColor)
    {
        if (iColor != fOutlineColor)
        {
            fOutlineColor = iColor;
            _updateOutline();
        }
    }

    void
    CCEnhancedLabelTTF::setOutlineThickness(float iThickness)
    {
        if (fOutlineThickness != iThickness)
        {
            fOutlineThickness = iThickness;
            _updateOutline();
        }
    }

    
    bool
    CCEnhancedLabelTTF::updateTexture()
    {
        if (!SuperClass::updateTexture())
        {
            return false;
        }
        
        _updateShadow();
        _updateOutline();
        
        return true;
    }

    void
    CCEnhancedLabelTTF::_updateOutline()
    {
        if (fOutline != nullptr)
        {
            fOutline->removeFromParentAndCleanup(true);
            fOutline = nullptr;
        }

        CCTexture2D* texture = getTexture();
        if ((texture != nullptr) && !m_string.empty() && (fOutlineColor.a != 0) && (fOutlineThickness > 0))
        {
            fOutline = CCSpriteBatchNode::createWithTexture(texture);
            /*fOutline->ignoreAnchorPointForPosition(false);
            fOutline->setAnchorPoint(CCPoint(0.5f, 0.5f));
            fOutline->setPosition(CCSizeMake(m_obContentSize.width / 2, m_obContentSize.height / 2));*/
            fOutline->setScale(1.f / fGlobalScale);
            addChild(fOutline, -1);

            ccColor3B c = ccc3(fOutlineColor.r, fOutlineColor.g, fOutlineColor.b);

            const float thickness = fOutlineThickness * fGlobalScale;
            
            // Create 4 shadows
            for (float x=-thickness; x <= thickness; x += thickness)
            {
                for (float y=-thickness; y <= thickness; y += thickness)
                {
                    CCSprite* sprite = CCSprite::createWithTexture(texture, m_obRect);
                    sprite->setColor(c);
                    sprite->setOpacity(fOutlineColor.a);
                    sprite->ignoreAnchorPointForPosition(true);
                    sprite->setPosition(CCPoint(x, y));

                    fOutline->addChild(sprite);
                }
            }
        }
    }

    void
    CCEnhancedLabelTTF::_updateShadow()
    {
        if (fShadow != nullptr)
        {
            fShadow->removeFromParentAndCleanup(true);
            fShadow = nullptr;
        }

        CCTexture2D* texture = getTexture();
        if ((texture != nullptr) && !m_string.empty() && (fShadowColor.a != 0) && ((fShadowOffset.width != 0) || (fShadowOffset.width != 0)))
        {
            fShadow = CCSprite::createWithTexture(texture, m_obRect);
            fShadow->ignoreAnchorPointForPosition(false);
            fShadow->setAnchorPoint(CCPoint(0, 0));
            fShadow->setScale(1.f / fGlobalScale);
            
            addChild(fShadow, -2);

            fShadow->setPosition(CCPoint(fShadowOffset.width, fShadowOffset.height));

            ccColor3B c = ccc3(fShadowColor.r, fShadowColor.g, fShadowColor.b);
            fShadow->setColor(c);
            fShadow->setOpacity(fShadowColor.a);
            
            // for debug
#if 0
            CCLayerColor* color = CCLayerColor::create(ccc4(255, 0, 0, 255), m_obRect.size.width, m_obRect.size.height);
            color->ignoreAnchorPointForPosition(true);
            fShadow->addChild(color, -1);
#endif
        }
    }

NS_CC_END

