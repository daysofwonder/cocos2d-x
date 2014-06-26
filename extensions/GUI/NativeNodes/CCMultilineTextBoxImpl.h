//
//  CCMultilineTextBoxImpl.h
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCMultilineTextBoxImpl__
#define __CCMultilineTextBoxImpl__

#include "ExtensionMacros.h"
#include "DOWFoundation/Url.h"
#include "ccTypes.h"

NS_CC_EXT_BEGIN

using namespace cocos2d;

    class CCMultilineTextBox;
    class CCMultilineTextBoxImpl
    {
    public:
        virtual ~CCMultilineTextBoxImpl() {}
        
        virtual std::string getText() const = 0;
        virtual void setText(const std::string& iText) = 0;
        
        virtual void setFont(const std::string& pName, float iSize) = 0;
        
        virtual void setBackgroundColor(const ccColor4B& iColor) = 0;
        virtual void setTextColor(const ccColor4B& iColor) = 0;

        virtual void retainFocus() = 0;
        virtual void releaseFocus() = 0;
    };

NS_CC_EXT_END

#endif /* defined(__CCMultilineTextBoxImpl__) */
