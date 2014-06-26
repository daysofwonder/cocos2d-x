//
//  CCMultilineTextBoxImplAndroid.h
//
//  Created by Gerald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCMultilineTextBoxImplAndroid__
#define __CCMultilineTextBoxImplAndroid__

#include "GUI/NativeNodes/android/CCNativeViewImplAndroid.h"
#include "GUI/NativeNodes/CCMultilineTextBoxImpl.h"

NS_CC_EXT_BEGIN

    class CCMultilineTextBox;
    
    class CCMultilineTextBoxImplAndroid : public CCNativeViewImplAndroid, public CCMultilineTextBoxImpl
    {
    public:
        DOW_DECLARE_HIERARCHY(CCMultilineTextBoxImplAndroid, CCNativeViewImplAndroid);
        
        CCMultilineTextBoxImplAndroid();
        virtual ~CCMultilineTextBoxImplAndroid();
        
        CCMultilineTextBox* textBox() const;

        virtual std::string getText() const override;
        virtual void setText(const std::string& iText) override;

        virtual void setFont(const std::string& pName, float iSize) override;

        virtual void setBackgroundColor(const ccColor4B& iColor) override;
        virtual void setTextColor(const ccColor4B& iColor) override;

        virtual void retainFocus() override;
        virtual void releaseFocus() override;

        // Call from java
        void textBoxDidStartEditing();
        void textBoxDidChange(jstring iText);
        void textBoxDidEndEditing();

    private:
        virtual std::string _nativeViewControllerFullName() override;
    };

NS_CC_EXT_END


#endif /* defined(__CCMultilineTextBoxImplAndroid__) */

