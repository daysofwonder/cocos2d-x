//
//  CCStylesProtocol.h
//
//  Created by Gérald Guyomard on 20/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCStylesProtocol_h
#define __CCStylesProtocol_h

#include "platform/CCPlatformMacros.h"
#include "cocoa/CCGeometry.h"
#include "ccTypes.h"
#include <string>

NS_CC_BEGIN


class CCStylesProtocol
{
public:
    
    // Shadow Management
    virtual ccColor4B shadowColor() const = 0;
    virtual void setShadowColor(const ccColor4B& iShadowColor) = 0;
    virtual CCSize shadowOffset() const = 0;
    virtual void setShadowOffset(const CCSize& iOffset) = 0;
    
    // Outline management
    virtual ccColor4B outlineColor() const = 0;
    virtual void setOutlineColor(const ccColor4B& iColor) = 0;
    
    virtual float outlineThickness() const = 0;
    virtual void setOutlineThickness(float iThickness) = 0;
    
    // Some helpers to facilitate deserialization
    void setShadowColorFromPlainText(const std::string& iShadowColor);
    void setShadowOffsetFromPlainText(const std::string& iShadowOffset);
    void setOutlineColorFromPlainText(const std::string& iOutlineColor);
    
    virtual void setLocalizedStringKey(const std::string& iKey) = 0;

protected:
    ~CCStylesProtocol() {}
};

NS_CC_END


#endif // __CCStylesProtocol_h
