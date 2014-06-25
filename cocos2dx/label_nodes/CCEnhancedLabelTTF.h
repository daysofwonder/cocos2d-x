//
//  CCEnhancedLabelTTF.h
//
//  Created by Gérald Guyomard on 20/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCEnhancedLabelTTF__
#define __CCEnhancedLabelTTF__

#include <cocos2d.h>

#include "label_nodes/CCStylesProtocol.h"
#include "cocoa/CCObjectPtr.h"
#include "DOWFoundation/DOWFoundation.h"

NS_CC_BEGIN

    class CCEnhancedLabelTTF : public CCLabelTTF, public CCStylesProtocol
    {
    public:
        DOW_DECLARE_HIERARCHY(CCEnhancedLabelTTF, CCLabelTTF);
        
        CCEnhancedLabelTTF();
        static CCEnhancedLabelTTF* create();
        virtual ~CCEnhancedLabelTTF();
        
        virtual void setTexture(CCTexture2D *texture) override;
        
        // Shadow Management
        virtual ccColor4B shadowColor() const override { return fShadowColor; }
        virtual void setShadowColor(const ccColor4B& iShadowColor) override;
        virtual CCSize shadowOffset() const override { return fShadowOffset; }
        virtual void setShadowOffset(const CCSize& iOffset) override;
        
        // Outline management
        virtual ccColor4B outlineColor() const override { return fOutlineColor; }
        virtual void setOutlineColor(const ccColor4B& iColor) override;
        
        virtual float outlineThickness() const override { return fOutlineThickness; }
        virtual void setOutlineThickness(float iThickness) override;

        virtual void setLocalizedStringKey(const std::string& iKey);

    protected:

        virtual bool updateTexture() override;

        void _updateShadow();
        void _updateOutline();
        
        ccColor4B   fShadowColor;
        CCSize      fShadowOffset;
        CCObjectPtr<CCSprite> fShadow;
        
        
        ccColor4B   fOutlineColor;
        float       fOutlineThickness;
        CCObjectPtr<CCSpriteBatchNode> fOutline;
    };

NS_CC_END


#endif /* defined(__CCEnhancedLabelTTF__) */
