//
//  CCEnhancedControlButton.cpp
//
//  Created by Gérald Guyomard on 20/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCEnhancedControlButton.h"
#include "label_nodes/CCEnhancedLabelTTF.h"
#include "label_nodes/CCStylesProtocol.h"

#include "DOWFoundation/StringUtils.h"
#include "DOWFoundation/Exception.h"


#include "misc_nodes/CCTooltip.h"

NS_CC_EXT_BEGIN

    CCEnhancedControlButton*
    CCEnhancedControlButton::create()
    {
        CCEnhancedControlButton* pRet = new CCEnhancedControlButton();
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
    
    bool
    CCEnhancedControlButton::init(void)
    {
        CCObjectPtr<CCEnhancedLabelTTF> label;
        label << new CCEnhancedLabelTTF;
        if(!label->initWithString("", "Helvetica", 12))
        {
            return false;
        }

        label->setHorizontalAlignment(kCCTextAlignmentCenter);
        label->setVerticalAlignment(kCCVerticalTextAlignmentCenter);
        
        return this->initWithLabelAndBackgroundSprite(label, CCScale9Sprite::create());
    }
    
    CCEnhancedControlButton::CCEnhancedControlButton()
    : fTitleWrappingEnabled(false), fAlphaHitTestEnabled(false)
    {
    
    }
    
    CCEnhancedControlButton::~CCEnhancedControlButton()
    {
        if (fTooltip != nullptr)
        {
            fTooltip->terminate();
        }
    }
    
    void
    CCEnhancedControlButton::setTitleTTFForState(const char * fntFile, CCControlState state)
    {
        CCString * title = this->getTitleForState(state);
        if (!title)
        {
            title = CCString::create("");
        }

        CCObjectPtr<CCEnhancedLabelTTF> label;
        label << new CCEnhancedLabelTTF;
        
        CCSize zeroDim;
        label->initWithString(title->getCString(), fntFile, 12, zeroDim, kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter);
        
        this->setTitleLabelForState(label, state);
    }
    
    template <class TTitle> void
    CCEnhancedControlButton::_iterateTitles(const std::function<bool (const TTitle*)>& iCallback) const
    {
        CCDictionary* dict = getTitleLabelDispatchTable();
        if (dict != nullptr)
        {
            CCArray* keys = dict->allKeys();
            CCObject* key;
            CCARRAY_FOREACH(keys, key)
            {
                CCInteger* state = (CCInteger*) key;
                
                CCNode* node = (CCNode*) dict->objectForKey(state->getValue());
                assert(node != nullptr);
                
                const TTitle* title = dynamic_cast<const TTitle*>(node);
                if (title != nullptr)
                {
                    if (iCallback(title))
                    {
                        break;
                    }
                }
            }
        }
    }

    template <class TTitle> void
    CCEnhancedControlButton::_iterateTitles(const std::function<bool (TTitle*)>& iCallback)
    {
        CCDictionary* dict = getTitleLabelDispatchTable();
        if (dict != nullptr)
        {
            CCArray* keys = dict->allKeys();
            CCObject* key;
            CCARRAY_FOREACH(keys, key)
            {
                CCInteger* state = (CCInteger*) key;
                
                CCNode* node = (CCNode*) dict->objectForKey(state->getValue());
                assert(node != nullptr);
                
                TTitle* title = dynamic_cast<TTitle*>(node);
                if (title != nullptr)
                {
                    if (iCallback(title))
                    {
                        break;
                    }
                }
            }
        }
    }
    
    void
    CCEnhancedControlButton::layoutTitleLabel()
    {
        static const CCRect kNoInsetsRect;
        
        if (!isTitleWrappingEnabled() && fTitleInsets.equals(kNoInsetsRect))
        {
            CCControlButton::layoutTitleLabel();
        }
        else
        {
            CCNode* label = getTitleLabel();
            if (label != NULL)
            {
                CCRect area;
                area.size = getContentSize();
                
                area.size.width -= fTitleInsets.getMaxX();
                area.size.height -= fTitleInsets.getMaxY();
                
                area.origin = fTitleInsets.origin;
                
                label->setPosition(CCPoint(area.getMidX(), area.getMidY()));
                
                CCLabelTTF* labelTTF = dynamic_cast<CCLabelTTF*>(getTitleLabel());
                if (labelTTF != NULL)
                {
                    labelTTF->setDimensions(area.size);
                }
            }            
        }
    }

	CCSize
    CCEnhancedControlButton::computeContentSize(const CCRect& iBackgroundRect, const CCRect& iTitleRect) const
    {
        if (isTitleWrappingEnabled())
        {
            return iBackgroundRect.size;
        }
        else
        {
            return CCControlButton::computeContentSize(iBackgroundRect, iTitleRect);
        }
    }

    void
    CCEnhancedControlButton::setTitleWrappingEnabled(bool iEnabled)
    {
        if (fTitleWrappingEnabled != iEnabled)
        {
            fTitleWrappingEnabled = iEnabled;
            
            needsLayout();
        }
    }
    
    void
    CCEnhancedControlButton::setTitleHorizontalAlignment(CCTextAlignment iAlignment)
    {
        _iterateTitles<CCLabelTTF>([iAlignment](CCLabelTTF* iLabel)
                                         {
                                             iLabel->setHorizontalAlignment(iAlignment);
                                             return true;
                                         });

    }
    
    void
    CCEnhancedControlButton::setTitleVerticalAlignment(CCVerticalTextAlignment iAlignment)
    {
        _iterateTitles<CCLabelTTF>([iAlignment](CCLabelTTF* iLabel)
                                   {
                                       iLabel->setVerticalAlignment(iAlignment);
                                       return true;
                                   });        
    }

    ccColor4B
    CCEnhancedControlButton::titleShadowColor() const
    {
        ccColor4B c = ccc4(0, 0, 0, 0);

        _iterateTitles<CCStylesProtocol>([&c](const CCStylesProtocol* iLabel)
                             {
                                 c = iLabel->shadowColor();
                                 return true;
                             });
        
        return c;
    }
    
    void
    CCEnhancedControlButton::setTitleShadowColor(const ccColor4B& iShadowColor)
    {
        _iterateTitles<CCStylesProtocol>([&iShadowColor](CCStylesProtocol* iLabel)
                             {
                                 iLabel->setShadowColor(iShadowColor);
                                 return false;
                             });

    }
    
    CCSize
    CCEnhancedControlButton::titleShadowOffset() const
    {
        CCSize offset;
        
        _iterateTitles<CCStylesProtocol>([&offset](const CCStylesProtocol* iLabel)
                             {
                                 offset = iLabel->shadowOffset();
                                 return true;
                             });
        
        return offset;
    }
    
    void
    CCEnhancedControlButton::setTitleShadowOffset(const CCSize& iOffset)
    {
        _iterateTitles<CCStylesProtocol>([&iOffset](CCStylesProtocol* iLabel)
                             {
                                 iLabel->setShadowOffset(iOffset);
                                 return false;
                             });
    }
    
    ccColor4B
    CCEnhancedControlButton::titleOutlineColor() const
    {
        ccColor4B c = ccc4(0, 0, 0, 0);
        
        _iterateTitles<CCStylesProtocol>([&c](const CCStylesProtocol* iLabel)
                             {
                                 c = iLabel->outlineColor();
                                 return true;
                             });
        
        return c;

    }
    
    void
    CCEnhancedControlButton::setTitleOutlineColor(const ccColor4B& iColor)
    {
        _iterateTitles<CCStylesProtocol>([&iColor](CCStylesProtocol* iLabel)
                             {
                                 iLabel->setOutlineColor(iColor);
                                 return false;
                             });

    }
    
    float
    CCEnhancedControlButton::titleOutlineThickness() const
    {
        float t = 0;
        
        _iterateTitles<CCStylesProtocol>([&t](const CCStylesProtocol* iLabel)
                             {
                                 t = iLabel->outlineThickness();
                                 return true;
                             });
        
        return t;

    }
    
    void
    CCEnhancedControlButton::setTitleOutlineThickness(float iThickness)
    {
        _iterateTitles<CCStylesProtocol>([iThickness](CCStylesProtocol* iLabel)
                             {
                                 iLabel->setOutlineThickness(iThickness);
                                 return false;
                             });
    }

    void
    CCEnhancedControlButton::setTitleMinimumFontSize(float iMinFontSize)
    {
        setTitleWrappingEnabled(iMinFontSize > 0);
        
        _iterateTitles<CCEnhancedLabelTTF>([iMinFontSize](CCEnhancedLabelTTF* iLabel)
                                         {
                                             iLabel->setMinimumFontSize(iMinFontSize);
                                             return false;
                                         });        
    }
    
    void
    CCEnhancedControlButton::setTitleLocalizedStringKey(const std::string& iKey)
    {        
        _iterateTitles<CCStylesProtocol>([&iKey](CCStylesProtocol* iLabel)
                             {
                                 iLabel->setLocalizedStringKey(iKey);
                                 return false;
                             });

        // Update also the string for any state
         CCString* value = CCLocalizedString(iKey);
        if (value != nullptr)
        {
            CCDictionary* dict = getTitleDispatchTable();
            if (dict != nullptr)
            {
                CCArray* keys = dict->allKeys();
                CCObject* key;
                CCARRAY_FOREACH(keys, key)
                {
                    CCInteger* k = (CCInteger*) key;
                    dict->setObject(value, k->getValue());
                }
            }            
        }
    }
    
    void
    CCEnhancedControlButton::setTitleShadowColorFromPlainText(const std::string& iShadowColor)
    {
        _iterateTitles<CCStylesProtocol>([&iShadowColor](CCStylesProtocol* iLabel)
                             {
                                 iLabel->setShadowColorFromPlainText(iShadowColor);
                                 return false;
                             });
    }
    
    void
    CCEnhancedControlButton::setTitleShadowOffsetFromPlainText(const std::string& iShadowOffset)
    {
        _iterateTitles<CCStylesProtocol>([&iShadowOffset](CCStylesProtocol* iLabel)
                             {
                                 iLabel->setShadowOffsetFromPlainText(iShadowOffset);
                                 return false;
                             });
    }
    
    void
    CCEnhancedControlButton::setTitleOutlineColorFromPlainText(const std::string& iOutlineColor)
    {
        _iterateTitles<CCStylesProtocol>([&iOutlineColor](CCStylesProtocol* iLabel)
                             {
                                 iLabel->setOutlineColorFromPlainText(iOutlineColor);
                                 return false;
                             });
      
    }
    
    void
    CCEnhancedControlButton::setTitleInsets(const CCRect& iTitleInsets)
    {
        if (!fTitleInsets.equals(iTitleInsets))
        {
            fTitleInsets = iTitleInsets;
            
            needsLayout();
        }
    }

    void
    CCEnhancedControlButton::setTitleInsetsFromPlainText(const std::string& iInsetsAsString)
    {        
        CCRect insets = CCRect::decode(iInsetsAsString);
        setTitleInsets(insets);
    }
    
    void
    CCEnhancedControlButton::setAlphaHitTestEnabled(bool iEnabled)
    {
        if (iEnabled != fAlphaHitTestEnabled)
        {
            fAlphaHitTestEnabled = iEnabled;
            
            if (!fAlphaHitTestEnabled)
            {
                fHitMap.reset();
            }
        }
    }
    
    class CCEnhancedControlButton::HitMap
    {
    public:
        HitMap(CCNode* iNode);
        
        bool isHit(const CCPoint& iPt) const;
        
    private:
        int32_t                 fWidth;
        int32_t                 fHeight;
        std::vector<uint8_t>    fData;
    };
    
    CCEnhancedControlButton::HitMap::HitMap(CCNode* iNode)
    {
        CCSize size = iNode->getContentSize();

        CCObjectPtr<RenderTexture> renderTexture = RenderTexture::create(size.width, size.height);
        
        auto* ctx = renderTexture->beginLocalWithClear(0, 0, 0, 0);
        {
            iNode->visit();
        }
        renderTexture->endLocal(ctx);
        
        // Extract the ARGB32 image
        CCObjectPtr<CCImage> img;
        img << renderTexture->newCCImage();
        
        fWidth = img->getWidth();
        fHeight = img->getHeight();
        
        // Only keep the alpha component
        const size_t nbBytes = fWidth * fHeight;
        fData.resize(nbBytes);
        
        assert(img->getBitsPerComponent() == 8);
        
        uint8_t* pSrc = img->getData();
        uint8_t* pDest = &fData[0];
        
        for (int32_t y=0; y < fHeight; ++y)
        {
            for (int32_t x=0; x < fWidth; ++x)
            {
                // RGBA
                *(pDest++) = pSrc[3];
                pSrc += sizeof(uint32_t);
            }
        }
    }
    
    bool
    CCEnhancedControlButton::HitMap::isHit(const CCPoint& iPt) const
    {
        const int32_t kScaleFactor = CC_CONTENT_SCALE_FACTOR();
        const int32_t x = iPt.x * kScaleFactor;
        if ((x < 0) || (x >= fWidth))
        {
            return false;
        }
        
        const int32_t y = fHeight - (iPt.y * kScaleFactor) - 1;
        if ((y < 0) || (y >= fHeight))
        {
            return false;
        }

        size_t index = (y * fWidth) + x;
        const uint8_t alpha = fData[index];
        
        return alpha >= 128;
    }
        
    bool
    CCEnhancedControlButton::isTouchInside(CCTouch *touch)
    {
        // Test against bounding box
        if (!CCControlButton::isTouchInside(touch))
        {
            return false;
        }
        
        if (isAlphaHitTestEnabled())
        {
            if (fHitMap == nullptr)
            {
                fHitMap.reset(new HitMap(this));
            }
            
            CCPoint pt = convertToNodeSpace(touch->getLocation());
            return fHitMap->isHit(pt);
        }
        
        return true;
    }
    
    bool
    CCEnhancedControlButton::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
    {
        CCTooltip::hideCurrentTooltip();
        if (fTooltip != nullptr)
        {
            fTooltip->terminate();
        }
        
        return CCControlButton::ccTouchBegan(pTouch, pEvent);
    }
    
    bool
    CCEnhancedControlButton::canHandleMouseOver()
    {
        return isEnabled();
    }
    
    void
    CCEnhancedControlButton::onMouseEnter(const CCPoint& iWorldMouseLocation)
    {
        if (fTooltip != nullptr)
        {
            fTooltip->onMouseEnter(iWorldMouseLocation);
        }
    }
    
    void
    CCEnhancedControlButton::onMouseOver(const CCPoint& iWorldMouseLocation)
    {
        if (fTooltip != nullptr)
        {
            fTooltip->onMouseOver(iWorldMouseLocation);
        }
    }
    
    void
    CCEnhancedControlButton::onMouseExit(const CCPoint& iWorldMouseLocation)
    {
        if (fTooltip != nullptr)
        {
            fTooltip->onMouseExit(iWorldMouseLocation);
        }
    }
    
    void
    CCEnhancedControlButton::setTooltipText(const std::string& iText)
    {
        if (fTooltip != nullptr)
        {
            fTooltip->terminate();
        }
        
        fTooltip = CCTextTooltip::create(iText);
    }
    
    void
    CCEnhancedControlButton::setTooltipLocalizedTextKey(const std::string& iKey)
    {
        if (fTooltip != nullptr)
        {
            fTooltip->terminate();
        }
        
        fTooltip = CCTextTooltip::createWithLocalizedTextKey(iKey);
    }
    
    void
    CCEnhancedControlButtonLoader::onHandlePropTypeInteger(CCNode * pNode, CCNode * pParent, const char* pPropertyName, int pInt, CCBReader * pCCBReader)
    {
        CCEnhancedControlButton* button = static_cast<CCEnhancedControlButton*>(pNode);
        assert(dynamic_cast<CCEnhancedControlButton*>(pNode) != nullptr);
        
        if (::strcmp(pPropertyName, "titleHorizontalAlignment") == 0)
        {
            CCTextAlignment alignment;
            if (pInt < 0)
            {
                alignment = kCCTextAlignmentLeft;
            }
            else if (pInt == 0)
            {
                alignment = kCCTextAlignmentCenter;
            }
            else
            {
                // >>
                alignment = kCCTextAlignmentRight;
            }
            
            button->setTitleHorizontalAlignment(alignment);
        }
        else if (::strcmp(pPropertyName, "titleVerticalAlignment") == 0)
        {
            CCVerticalTextAlignment alignment;
            if (pInt < 0)
            {
                alignment = kCCVerticalTextAlignmentBottom;
            }
            else if (pInt == 0)
            {
                alignment = kCCVerticalTextAlignmentCenter;
            }
            else
            {
                // >>
                alignment = kCCVerticalTextAlignmentTop;
            }
            
            button->setTitleVerticalAlignment(alignment);
        }
        else
        {
            SuperClass::onHandlePropTypeInteger(pNode, pParent, pPropertyName, pInt, pCCBReader);
        }        
    }
    
    void
    CCEnhancedControlButtonLoader::onHandlePropTypeFloat(CCNode * pNode, CCNode * pParent, const char* pPropertyName, float pFloat, CCBReader * pCCBReader)
    {
        CCEnhancedControlButton* button = static_cast<CCEnhancedControlButton*>(pNode);
        assert(dynamic_cast<CCEnhancedControlButton*>(pNode) != nullptr);
        
        if (::strcmp(pPropertyName, "titleOutlineThickness") == 0)
        {
            button->setTitleOutlineThickness(pFloat);
        }
        else if (::strcmp(pPropertyName, "titleMinimumFontSize") == 0)
        {
            button->setTitleMinimumFontSize(pFloat);
        }
        else
        {
            SuperClass::onHandlePropTypeFloat(pNode, pParent, pPropertyName, pFloat, pCCBReader);
        }
    }
    
    void
    CCEnhancedControlButtonLoader::onHandlePropTypeString(CCNode * pNode, CCNode * pParent, const char* pPropertyName, const char * pString, CCBReader * pCCBReader)
    {
        CCEnhancedControlButton* button = static_cast<CCEnhancedControlButton*>(pNode);
        assert(dynamic_cast<CCEnhancedControlButton*>(pNode) != nullptr);
        
        if (::strcmp(pPropertyName, "titleShadowOffset") == 0)
        {
            button->setTitleShadowOffsetFromPlainText(pString);
        }
        else if (::strcmp(pPropertyName, "titleShadowColor") == 0)
        {
            button->setTitleShadowColorFromPlainText(pString);
        }
        else if (::strcmp(pPropertyName, "titleOutlineColor") == 0)
        {
            button->setTitleOutlineColorFromPlainText(pString);
        }
        else if (::strcmp(pPropertyName, "titleLocalizedStringKey") == 0)
        {
            button->setTitleLocalizedStringKey(pString);
        }
        else if (::strcmp(pPropertyName, "titleInsets") == 0)
        {
            button->setTitleInsetsFromPlainText(pString);
        }
        else if (::strcmp(pPropertyName, "tooltipText") == 0)
        {
            button->setTooltipText(pString);
        }
        else if (::strcmp(pPropertyName, "tooltipLocalizedStringKey") == 0)
        {
            button->setTooltipLocalizedTextKey(pString);
        }
        else
        {
            SuperClass::onHandlePropTypeString(pNode, pParent, pPropertyName, pString, pCCBReader);
        }
    }
    
    void
    CCEnhancedControlButtonLoader::onHandlePropTypeCheck(CCNode * pNode, CCNode * pParent, const char* pPropertyName, bool pCheck, CCBReader * pCCBReader)
    {
        CCEnhancedControlButton* button = static_cast<CCEnhancedControlButton*>(pNode);
        assert(dynamic_cast<CCEnhancedControlButton*>(pNode) != nullptr);
        
        if (::strcmp(pPropertyName, "titleWrappingEnabled") == 0)
        {
            button->setTitleWrappingEnabled(pCheck);
        }
        else if (::strcmp(pPropertyName, "alphaHitTestEnabled") == 0)
        {
            button->setAlphaHitTestEnabled(pCheck);
        }
        else
        {
            SuperClass::onHandlePropTypeCheck(pNode, pParent, pPropertyName, pCheck, pCCBReader);
        }
    }

NS_CC_EXT_END
