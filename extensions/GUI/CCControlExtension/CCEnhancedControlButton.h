//
//  CCEnhancedControlButton.h
//
//  Created by Gérald Guyomard on 20/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCEnhancedControlButton__
#define __CCEnhancedControlButton__

#include <cocos2d.h>

#include "GUI/CCControlExtension/CCControlButton.h"
#include "CCBReader/CCControlButtonLoader.h"
#include "cocoa/CCObjectPtr.h"
#include <functional>
#include <memory>
#include "DOWFoundation/DOWFoundation.h"

NS_CC_BEGIN
    class CCEnhancedLabelTTF;
    class CCTooltip;
NS_CC_END

NS_CC_EXT_BEGIN
    
    class CCEnhancedControlButton : public CCControlButton
    {
    public:
        static CCEnhancedControlButton* create();
        virtual ~CCEnhancedControlButton();
        
        virtual bool init(void) override;
        virtual void setTitleTTFForState(const char * fntFile, CCControlState state) override;

        virtual void layoutTitleLabel() override;
        virtual CCSize computeContentSize(const CCRect& iBackgroundRect, const CCRect& iTitleRect) const override;

        virtual bool isTouchInside(CCTouch * touch) override;
        
        // Shadow Management
        ccColor4B titleShadowColor() const;
        void setTitleShadowColor(const ccColor4B& iShadowColor);
        CCSize titleShadowOffset() const;
        void setTitleShadowOffset(const CCSize& iOffset);
        
        // Outline management
        ccColor4B titleOutlineColor() const;
        void setTitleOutlineColor(const ccColor4B& iColor);
        
        float titleOutlineThickness() const;
        void setTitleOutlineThickness(float iThickness);
        
        // Alignments
        void setTitleHorizontalAlignment(CCTextAlignment iAlignment);
        void setTitleVerticalAlignment(CCVerticalTextAlignment iAlignment);
        
        // Size constraint Management
        void setTitleMinimumFontSize(float iMinFontSize);
        
        // Some helpers to facilitate deserialization
        void setTitleShadowColorFromPlainText(const std::string& iShadowColor);
        void setTitleShadowOffsetFromPlainText(const std::string& iShadowOffset);
        void setTitleOutlineColorFromPlainText(const std::string& iOutlineColor);
        
        void setTitleLocalizedStringKey(const std::string& iKey);
        
        const CCRect& titleInsets() const { return fTitleInsets; }
        void setTitleInsets(const CCRect& iTitleInsets);
        void setTitleInsetsFromPlainText(const std::string& iInsetsAsString);
        
        bool isTitleWrappingEnabled() const { return fTitleWrappingEnabled; }
        void setTitleWrappingEnabled(bool iEnabled);
        
        bool isAlphaHitTestEnabled() const { return fAlphaHitTestEnabled; }
        void setAlphaHitTestEnabled(bool iEnabled);
        
        virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent) override;
        
        // Rollover system
        virtual bool canHandleMouseOver() override;
        virtual void onMouseEnter(const CCPoint& iWorldMouseLocation) override;
        virtual void onMouseOver(const CCPoint& iWorldMouseLocation) override;
        virtual void onMouseExit(const CCPoint& iWorldMouseLocation) override;
        
        void setTooltipText(const std::string& iText);
        void setTooltipLocalizedTextKey(const std::string& iKey);
        
    protected:
        CCEnhancedControlButton();
        
    private:
        template <class TTitle> void _iterateTitles(const std::function<bool (const TTitle*)>&) const;
        template <class TTitle> void _iterateTitles(const std::function<bool (TTitle*)>&);
        
        CCRect  fTitleInsets;
        
        uint32_t fTitleWrappingEnabled : 1;
        uint32_t fAlphaHitTestEnabled : 1;
        
        class HitMap;
        std::unique_ptr<HitMap> fHitMap;
        
        CCObjectPtr<CCTooltip> fTooltip;
    };
    
    class CCEnhancedControlButtonLoader : public CCControlButtonLoader
    {
    public:
        DOW_DECLARE_HIERARCHY(CCEnhancedControlButtonLoader, CCControlButtonLoader);
        
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCEnhancedControlButtonLoader, loader);
        
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CCEnhancedControlButton);
        
        virtual void onHandlePropTypeInteger(CCNode * pNode, CCNode * pParent, const char* pPropertyName, int pInt, CCBReader * pCCBReader) override;
        virtual void onHandlePropTypeCheck(CCNode * pNode, CCNode * pParent, const char* pPropertyName, bool pCheck, CCBReader * pCCBReader) override;
        virtual void onHandlePropTypeFloat(CCNode * pNode, CCNode * pParent, const char* pPropertyName, float pFloat, CCBReader * pCCBReader) override;
        virtual void onHandlePropTypeString(CCNode * pNode, CCNode * pParent, const char* pPropertyName, const char * pString, CCBReader * pCCBReader) override;

    };

NS_CC_EXT_END

#endif /* defined(__CCEnhancedControlButton__) */
