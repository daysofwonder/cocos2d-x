//
//  CCMultilineTextBoxImplIOS.cpp
//  SmallWorld2
//
//  Created by Gérald Guyomard on 05/03/13.
//
//

#include "GUI/NativeNodes/CCMultilineTextBox.h"
#include "../DOWFoundationImpl/Apple/NSString+STLExtensions.h"
#include "Cocos2DNativeView.h"
#include "CCNativeViewImplApple.h"
#include "GUI/NativeNodes/CCMultilineTextBoxImpl.h"

#include <WebKit/WebKit.h>

@class CCMultilineTextBoxImplOSX_TextViewDelegate;

NS_CC_EXT_BEGIN

    class CCMultilineTextBox;
    
    class CCMultilineTextBoxImplIOSX : public CCNativeViewImplApple, public CCMultilineTextBoxImpl
    {
    public:
        DOW_DECLARE_HIERARCHY(CCMultilineTextBoxImplIOSX, CCNativeViewImplApple);
        
        CCMultilineTextBoxImplIOSX();
        virtual ~CCMultilineTextBoxImplIOSX();
        
        CCMultilineTextBox* textBox() const;
        
        NSTextView* uiTextView() const;
        
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
        virtual void _updateNativeUIView() override;
        
#if CC_TARGET_OS_MAC
        std::string fFontName;
        float fFontSize = 0;
        
        void _adjustFontSize(bool iForceUpdate);
#endif
        
        CCMultilineTextBoxImplOSX_TextViewDelegate* fTextViewDelegate;
    };

NS_CC_EXT_END



@interface Cocos2dTextView : NSTextView<Cocos2DNativeView>
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

@interface CCMultilineTextBoxImplOSX_TextViewDelegate : NSObject<NSTextViewDelegate>
{
@public
    
@private
    cocos2d::extension::CCMultilineTextBoxImplIOSX* fImpl;
    __weak NSTextView*           fTextView;
}

-(id) initWithImpl:(cocos2d::extension::CCMultilineTextBoxImplIOSX*)iImpl textView:(NSTextView*)iTextView;

-(void) detach;

@end

@implementation CCMultilineTextBoxImplOSX_TextViewDelegate

-(id) initWithImpl:(cocos2d::extension::CCMultilineTextBoxImplIOSX*)iImpl textView:(NSTextView*)iTextView
{
    if (self = [self init])
    {
        // Weak
        fImpl = iImpl;
        fTextView = iTextView;
        
        [fTextView setDelegate: self];
    }
    
    return self;
}

-(void) dealloc
{
    [fTextView setDelegate: nil];
    
    [super dealloc];
}

-(void) detach
{
    fImpl = nullptr;
    [fTextView setDelegate: nil];
    fTextView = nil;
}

- (void)textDidBeginEditing:(NSNotification *)aNotification
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

- (void)textDidEndEditing:(NSNotification *)aNotification
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

- (void)textDidChange:(NSNotification *)aNotification
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
        std::unique_ptr<CCNativeViewImpl> impl(new CCMultilineTextBoxImplIOSX);
        return impl;
    }
    
    CCMultilineTextBoxImplIOSX::CCMultilineTextBoxImplIOSX()
    : fTextViewDelegate(nil)
    {
    }
    
    CCMultilineTextBoxImplIOSX::~CCMultilineTextBoxImplIOSX()
    {
        auto* view = uiTextView();
        view.delegate = nil;
        [(Cocos2dTextView*) view detach];
        
        [fTextViewDelegate detach];
        [fTextViewDelegate release];
    }
    
    AppleView*
    CCMultilineTextBoxImplIOSX::_createUIView()
    {
        CGRect r = CGRectMake(0, 0, 100, 100);
        
        Cocos2dTextView* textView = [[Cocos2dTextView alloc] initWithFrame:r owner:textBox()];
        [textView setWantsLayer:YES];
        [textView setDrawsBackground:YES];
        [textView setAllowsUndo:YES];
        
        NSFont* font = [textView font];
        fFontName = [NSString stdStringWithString:[font familyName]];
        fFontSize = [font pointSize];
        
        fTextViewDelegate = [[CCMultilineTextBoxImplOSX_TextViewDelegate alloc] initWithImpl:this textView:textView];
        return textView;
    }
    
    NSTextView*
    CCMultilineTextBoxImplIOSX::uiTextView() const
    {
        AppleView* view = uiView();
        assert([view isKindOfClass:[NSTextView class]]);
        return (NSTextView*) view;
    }

    CCMultilineTextBox*
    CCMultilineTextBoxImplIOSX::textBox() const
    {
        return static_cast<CCMultilineTextBox*>(owner());
    }

    std::string
    CCMultilineTextBoxImplIOSX::getText() const
    {
        NSTextView* textView = uiTextView();
        NSTextStorage* textStorage = [textView textStorage];
        NSMutableString* mutableString = [textStorage mutableString];
        const char *text = [mutableString UTF8String];
        return text;
    }
    
    void
    CCMultilineTextBoxImplIOSX::setText(const std::string& iText)
    {
        NSTextView* textView = uiTextView();
        NSTextStorage* textStorage = [textView textStorage];
        NSMutableString* mutableString = [textStorage mutableString];
        NSString *string = [NSString stringWithUTF8String: iText.c_str()];
        [mutableString setString: string];
    }
    
    void
    CCMultilineTextBoxImplIOSX::setFont(const std::string& pName, float iSize)
    {
#if CC_TARGET_OS_MAC
        fFontName = pName;
        fFontSize = iSize;
        _adjustFontSize(true);
#else
        UIFont* font = [UIFont fontWithName:[NSString stringWithStdString:pName] size:iSize];
        [uiTextView() setFont:font];
#endif
    }

    void
    CCMultilineTextBoxImplIOSX::setBackgroundColor(const ccColor4B& iColor)
    {
#if CC_TARGET_OS_MAC
        uiTextView().backgroundColor = [NSColor colorWithCalibratedRed:float(iColor.r) / 255.f green:float(iColor.g) / 255.f blue:float(iColor.b) / 255.f alpha:float(iColor.a) / 255.f];
#else
        uiTextView().backgroundColor = [UIColor colorWithRed:float(iColor.r) / 255.f green:float(iColor.g) / 255.f blue:float(iColor.b) / 255.f alpha:float(iColor.a) / 255.f];
#endif
    }
    
    void
    CCMultilineTextBoxImplIOSX::setTextColor(const ccColor4B& iColor)
    {
#if CC_TARGET_OS_MAC
        uiTextView().textColor = [NSColor colorWithCalibratedRed:float(iColor.r) / 255.f green:float(iColor.g) / 255.f blue:float(iColor.b) / 255.f alpha:float(iColor.a) / 255.f];
#else
        uiTextView().textColor = [UIColor colorWithRed:float(iColor.r) / 255.f green:float(iColor.g) / 255.f blue:float(iColor.b) / 255.f alpha:float(iColor.a) / 255.f];
#endif
    }

    void
    CCMultilineTextBoxImplIOSX::retainFocus()
    {
        #if CC_TARGET_OS_MAC
            auto* textView = uiTextView();
            [textView.window makeFirstResponder:textView];
        #else
            [uiTextView() becomeFirstResponder];
        #endif
    }
    
    void
    CCMultilineTextBoxImplIOSX::releaseFocus()
    {
#if CC_TARGET_OS_MAC
        auto* textView = uiTextView();
        [textView.window makeFirstResponder:[textView.window initialFirstResponder]]; // initialFirstResponder is glview
#else
        [uiTextView() resignFirstResponder];
#endif
    }
    
    void
    CCMultilineTextBoxImplIOSX::_onExit()
    {
        releaseFocus();

        SuperClass::_onExit();
    }
    
    void
    CCMultilineTextBoxImplIOSX::_updateNativeUIView()
    {
        SuperClass::_updateNativeUIView();
        
    #if CC_TARGET_OS_MAC
        auto* tView = uiTextView();
        if (!tView.isHidden)
        {
            _adjustFontSize(false);
        }
    #endif
    }
    
#if CC_TARGET_OS_MAC
    void
    CCMultilineTextBoxImplIOSX::_adjustFontSize(bool iForceUpdate)
    {
        // Take into account overall scaling
        auto* tBox = textBox();
        CCNode* parent = tBox->getParent();
        CCRect worldBox = tBox->boundingBox();
        
        if (parent != NULL)
        {
            worldBox = CCRectApplyAffineTransform(worldBox, parent->nodeToWorldTransform());
        }
        
        const float scale = worldBox.size.width / tBox->getContentSize().width;
        float fSize = float(fFontSize) * scale * CCEGLView::sharedOpenGLView()->getScaleX();
        fSize /= frameBufferScale();
        
        auto* tView = uiTextView();
        
        if (!iForceUpdate)
        {
            NSFont* font = [tView font];
            const float currentSize = [font pointSize];
            if (currentSize == fSize)
            {
                return;
            }
        }
        
        NSFont* font = [NSFont fontWithName:[NSString stringWithStdString:fFontName] size:fSize];        
        [tView setFont:font];
    }
#endif
    
NS_CC_EXT_END


