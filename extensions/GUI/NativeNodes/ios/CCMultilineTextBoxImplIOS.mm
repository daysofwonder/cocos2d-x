//
//  CCMultilineTextBoxImplIOS.cpp
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCMultilineTextBoxImplIOS.h"
#include "CCMultilineTextBox.h"
#include "../DOWFoundationImpl/Apple/NSString+STLExtensions.h"
#include "Cocos2DNativeView.h"

#ifdef CC_TARGET_OS_MAC
#define UITextView NSTextView
#define UITextViewDelegate NSObject
#endif

@interface Cocos2dTextView : UITextView<Cocos2DNativeView>
{
@protected
    cocos2d::extension::CCMultilineTextBox* fTextBox;
}

-(id) initWithFrame:(CGRect)frame owner:(cocos2d::extension::CCMultilineTextBox*)iTextBox;
-(cocos2d::CCNode*) cocos2dNode;

-(void) detach;

@end

@implementation Cocos2dTextView

-(id) initWithFrame:(CGRect)frame owner:(cocos2d::extension::CCMultilineTextBox*)iTextBox
{
    if (self = [self initWithFrame:frame])
    {
        fTextBox = iTextBox;
    }
    
    return self;
}

-(cocos2d::CCNode*) cocos2dNode
{
    return fTextBox;
}

-(void) detach
{
    fTextBox = nullptr;
}

@end

@interface CCMultilineTextBoxImplIOS_TextViewDelegate : NSObject<UITextViewDelegate>
{
@public
    
@private
    cocos2d::extension::CCMultilineTextBoxImplIOS* fImpl;
    __weak UITextView*           fTextView;
}

-(id) initWithImpl:(cocos2d::extension::CCMultilineTextBoxImplIOS*)iImpl textView:(UITextView*)iTextView;

-(void) detach;

@end

@implementation CCMultilineTextBoxImplIOS_TextViewDelegate

-(id) initWithImpl:(cocos2d::extension::CCMultilineTextBoxImplIOS*)iImpl textView:(UITextView*)iTextView
{
    if (self = [self init])
    {
        // Weak
        fImpl = iImpl;
        fTextView = iTextView;
        
        fTextView.delegate = self;
    }
    
    return self;
}

-(void) dealloc
{
    fTextView.delegate = nil;
    
    [super dealloc];
}

-(void) detach
{
    fImpl = nullptr;
    fTextView.delegate = nil;
    fTextView = nil;
}

-(void) textViewDidBeginEditing:(UITextView *)textView
{
    if (fImpl != nullptr)
    {
        auto* textBox = fImpl->textBox();
        if (textBox != nullptr)
        {
            auto* delegate = textBox->delegate();
            if (delegate != nullptr)
            {
                delegate->textBoxEditingDidBegin(textBox);
            }            
        }
    }
}

-(void) textViewDidEndEditing:(UITextView *)textView
{
    if (fImpl != nullptr)
    {
        auto* textBox = fImpl->textBox();
        if (textBox != nullptr)
        {
            auto* delegate = textBox->delegate();
            if (delegate != nullptr)
            {
                delegate->textBoxEditingDidEnd(textBox);
            }            
        }
    }
}

-(void) textViewDidChange:(UITextView *)textView
{
    if (fImpl != nullptr)
    {
        auto* textBox = fImpl->textBox();
        if (textBox != nullptr)
        {
            auto* delegate = textBox->delegate();
            if (delegate != nullptr)
            {
                std::string text = textBox->getText();
                delegate->textBoxTextChanged(textBox, text);
            }                    
        }
    }
}

@end

NS_CC_EXT_BEGIN

    std::unique_ptr<CCNativeViewImpl>
    CCMultilineTextBox::_createImpl()
    {
        std::unique_ptr<CCNativeViewImpl> impl(new CCMultilineTextBoxImplIOS);
        return impl;
    }
    
    CCMultilineTextBoxImplIOS::CCMultilineTextBoxImplIOS()
    : fTextViewDelegate(nil)
    {
    }
    
    CCMultilineTextBoxImplIOS::~CCMultilineTextBoxImplIOS()
    {
        auto* view = uiTextView();
        view.delegate = nil;
        [(Cocos2dTextView*) view detach];
        
        [fTextViewDelegate detach];
        [fTextViewDelegate release];
    }
    
    AppleView*
    CCMultilineTextBoxImplIOS::_createUIView()
    {
        CGRect r = CGRectMake(0, 0, 100, 100);
        
        Cocos2dTextView* textView = [[Cocos2dTextView alloc] initWithFrame:r owner:textBox()];
        
        fTextViewDelegate = [[CCMultilineTextBoxImplIOS_TextViewDelegate alloc] initWithImpl:this textView:textView];
        return textView;
    }
    
    UITextView*
    CCMultilineTextBoxImplIOS::uiTextView() const
    {
        AppleView* view = uiView();
        assert([view isKindOfClass:[UITextView class]]);
        return (UITextView*) view;
    }

    CCMultilineTextBox*
    CCMultilineTextBoxImplIOS::textBox() const
    {
        return static_cast<CCMultilineTextBox*>(owner());
    }

    std::string
    CCMultilineTextBoxImplIOS::getText() const
    {
#ifndef CC_TARGET_OS_MAC
        NSString* t = uiTextView().text;
        return [t stdString];
#else
      return "";
#endif
    }
    
    void
    CCMultilineTextBoxImplIOS::setText(const std::string& iText)
    {
#ifndef CC_TARGET_OS_MAC
        [uiTextView() setText:[NSString stringWithStdString:iText]];
#endif
    }
    
    void
    CCMultilineTextBoxImplIOS::setFont(const std::string& pName, float iSize)
    {
#ifndef CC_TARGET_OS_MAC
        UIFont* font = [UIFont fontWithName:[NSString stringWithStdString:pName] size:iSize];
        [uiTextView() setFont:font];
#endif
    }

    void
    CCMultilineTextBoxImplIOS::setBackgroundColor(const ccColor4B& iColor)
    {
#ifndef CC_TARGET_OS_MAC
        uiTextView().backgroundColor = [UIColor colorWithRed:float(iColor.r) / 255.f green:float(iColor.g) / 255.f blue:float(iColor.b) / 255.f alpha:float(iColor.a) / 255.f];
#endif
    }
    
    void
    CCMultilineTextBoxImplIOS::setTextColor(const ccColor4B& iColor)
    {
#ifndef CC_TARGET_OS_MAC
        uiTextView().textColor = [UIColor colorWithRed:float(iColor.r) / 255.f green:float(iColor.g) / 255.f blue:float(iColor.b) / 255.f alpha:float(iColor.a) / 255.f];
#endif
    }

    void
    CCMultilineTextBoxImplIOS::retainFocus()
    {
        [uiTextView() becomeFirstResponder];
    }
    
    void
    CCMultilineTextBoxImplIOS::releaseFocus()
    {
        [uiTextView() resignFirstResponder];
    }
    
    void
    CCMultilineTextBoxImplIOS::_onExit()
    {
        releaseFocus();

        SuperClass::_onExit();
    }

NS_CC_EXT_END
