//
//  CCMultilineTextBoxImplIOS.h
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCMultilineTextBoxImplIOS__
#define __CCMultilineTextBoxImplIOS__

#include "GUI/NativeNodes/apple/CCNativeViewImplApple.h"
#include "CCMultilineTextBoxImpl.h"
#include <UIKit/UIKit.h>
#include "DOWFoundation/DOWFoundation.h"

@class CCMultilineTextBoxImplIOS_TextViewDelegate;

NS_CC_EXT_BEGIN

    class CCMultilineTextBox;
    
    class CCMultilineTextBoxImplIOS : public CCNativeViewImplApple, public CCMultilineTextBoxImpl
    {
    public:
        DOW_DECLARE_HIERARCHY(CCMultilineTextBoxImplIOS, CCNativeViewImplApple);
        
        CCMultilineTextBoxImplIOS();
        virtual ~CCMultilineTextBoxImplIOS();
                                        
        CCMultilineTextBox* textBox() const;
        
        UITextView* uiTextView() const;

        virtual std::string getText() const override;
        virtual void setText(const std::string& iText) override;
        
        virtual void setFont(const std::string& pName, float iSize) override;

        virtual void setBackgroundColor(const ccColor4B& iColor) override;
        virtual void setTextColor(const ccColor4B& iColor) override;

        virtual void retainFocus() override;
        virtual void releaseFocus() override;

    private:
        virtual AppleView* _createUIView() override;
        virtual void _onExit() override;

        CCMultilineTextBoxImplIOS_TextViewDelegate* fTextViewDelegate;
    };

NS_CC_EXT_END

#endif /* defined(__CCMultilineTextBoxImplIOS__) */
