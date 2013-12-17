/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2012 James Chen
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "CCEditBoxImplApple.h"

#if __APPLE__

#define kLabelZOrder  9999

#include "CCEditBox.h"
#import "EAGLView.h"

@class EditBoxImplApple;

#if TARGET_OS_IPHONE

    #define NativeTextFieldDelegate UITextFieldDelegate

    @interface CustomUITextField : UITextField<Cocos2DNativeView>
    {
    @protected
        EditBoxImplApple* impl_;
    }

    -(id) initWithFrame:(CGRect)frame impl:(EditBoxImplApple*)iImpl;

    @end


    @implementation CustomUITextField

        -(id) initWithFrame:(CGRect)frame impl:(EditBoxImplApple*)iImpl
        {
            if (self = [self initWithFrame:frame])
            {
                impl_ = iImpl;
            }
            
            return self;
        }

        -(cocos2d::CCNode*) cocos2dNode
        {
            cocos2d::extension::CCEditBoxImplApple* impl = (cocos2d::extension::CCEditBoxImplApple*) impl_.editBox;
            return impl->getCCEditBox();
        }

    @end

#else

    // Mac
    #define NativeTextFieldDelegate NSTextFieldDelegate

    static NSRect adjustedFrameToVerticallyCenterText(NSTextFieldCell* iCell, const NSRect& frame)
    {
        // super would normally draw text at the top of the cell
        NSInteger offset = floor((NSHeight(frame) -
                                  ([[iCell font] ascender] - [[iCell font] descender])) / 2);
        return NSInsetRect(frame, 0.0, offset);
    }

    @interface CustomTextCell : NSTextFieldCell
    @end

    @implementation CustomTextCell

    - (void)editWithFrame:(NSRect)aRect inView:(NSView *)controlView
                   editor:(NSText *)editor delegate:(id)delegate event:(NSEvent *)event
    {
        [super editWithFrame:adjustedFrameToVerticallyCenterText(self, aRect)
                      inView:controlView editor:editor delegate:delegate event:event];
    }

    - (void)selectWithFrame:(NSRect)aRect inView:(NSView *)controlView
                     editor:(NSText *)editor delegate:(id)delegate
                      start:(NSInteger)start length:(NSInteger)length
    {
        
        [super selectWithFrame:adjustedFrameToVerticallyCenterText(self, aRect)
                        inView:controlView editor:editor delegate:delegate
                         start:start length:length];
    }

    - (void)drawInteriorWithFrame:(NSRect)frame inView:(NSView *)view
    {
        [super drawInteriorWithFrame:
         adjustedFrameToVerticallyCenterText(self, frame) inView:view];
    }


    @end

    @interface CustomNSTextField : NSTextField
    {
    }

    @end

    @implementation CustomNSTextField

        +(void)load
        {
            [self setCellClass:[CustomTextCell class]];
        }

        - (void)setup
        {
            [self setBordered:NO];
            [self setHidden:YES];
            [self setWantsLayer:YES];
        }

    @end

#endif

@interface EditBoxImplApple()<NativeTextFieldDelegate>
@end

@implementation EditBoxImplApple

@synthesize textField = textField_;
@synthesize editState = editState_;
@synthesize editBox = editBox_;

-(id) initWithFrame:(CGRect)frameRect editBox: (cocos2d::extension::CCEditBoxImplApple*) editBox
{
    if (self = [super init])
    {
        editState_ = NO;
        self.editBox = editBox;
        
#if TARGET_OS_IPHONE
        textField_ = [[CustomUITextField alloc] initWithFrame: frameRect impl:self];
        
        [textField_ setTextColor:[UIColor whiteColor]];
        textField_.font = [UIFont systemFontOfSize:frameRect.size.height*2/3]; //TODO need to delete hard code here.
		textField_.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
        textField_.backgroundColor = [UIColor clearColor];
        textField_.borderStyle = UITextBorderStyleNone;
        textField_.hidden = true;
		textField_.returnKeyType = UIReturnKeyDefault;
        [textField_ addTarget:self action:@selector(textChanged) forControlEvents:UIControlEventEditingChanged];
#else
        // Mac
        textField_ = [[CustomNSTextField alloc] initWithFrame: frameRect];
        
        [textField_ setTextColor:[NSColor whiteColor]];
        textField_.font = [NSFont systemFontOfSize:frameRect.size.height*2/3]; //TODO need to delete hard code here.
        textField_.backgroundColor = [NSColor clearColor];
        //textField_.backgroundColor = [NSColor redColor];
        [(CustomNSTextField*)textField_ setup];
#endif
        
        textField_.delegate = self;
    }
    
    return self;
}

-(void) doAnimationWhenKeyboardMoveWithDuration:(float)duration distance:(float)distance
{
    id eglView = [EAGLView sharedEGLView];
    [eglView doAnimationWhenKeyboardMoveWithDuration:duration distance:distance];
}

-(void) setPosition:(NativePoint) pos
{
    NativeRect frame = [textField_ frame];
    frame.origin = pos;
    [textField_ setFrame:frame];
}

-(void) setContentSize:(NativeSize) size
{
    NativeRect frame = [textField_ frame];
    
#if !TARGET_OS_IPHONE
    // Mac
    const float s = cocos2d::CCDirector::sharedDirector()->getContentScaleFactor();
    
    size.width *= s;
    size.height *= s;
#endif
    
    frame.size = size;
    [textField_ setFrame:frame];
}

-(void) setVisible:(BOOL)iVisible
{
    [textField_ setHidden:!iVisible];
}

-(void) openKeyboard
{
    [[EAGLView sharedEGLView] addSubview:textField_];
    
#if TARGET_OS_IPHONE
    [textField_ becomeFirstResponder];
#else
    [textField_.window makeFirstResponder:textField_];
#endif
}

-(void) closeKeyboard
{
    [textField_ resignFirstResponder];
    [textField_ removeFromSuperview];
}

- (BOOL)textFieldShouldReturn:(NativeTextField *)sender
{
    if (sender == textField_)
    {
        [sender resignFirstResponder];
        
        editBox_->setText(editBox_->getText());
        
        cocos2d::extension::CCEditBoxDelegate* pDelegate = editBox_->getDelegate();
        if (pDelegate != NULL)
        {
            pDelegate->editBoxReturn(editBox_->getCCEditBox());
        }
        
        cocos2d::extension::CCEditBox*  pEditBox= editBox_->getCCEditBox();
        if (NULL != pEditBox && 0 != pEditBox->getScriptEditBoxHandler())
        {
            cocos2d::CCScriptEngineProtocol* pEngine = cocos2d::CCScriptEngineManager::sharedManager()->getScriptEngine();
            pEngine->executeEvent(pEditBox->getScriptEditBoxHandler(), "return",pEditBox);
        }
        
        return YES;
    }
    
    return NO;
}

#if TARGET_OS_IPHONE
-(void)animationSelector
{
    id eglView = [EAGLView sharedEGLView];
    [eglView doAnimationWhenAnotherEditBeClicked];
}
#endif

- (BOOL)textFieldShouldBeginEditing:(NativeTextField *)sender        // return NO to disallow editing.
{
    CCLOG("textFieldShouldBeginEditing...");
    editState_ = YES;
    
#if TARGET_OS_IPHONE
    id eglView = [EAGLView sharedEGLView];
    if ([eglView isKeyboardShown])
    {
        [self performSelector:@selector(animationSelector) withObject:nil afterDelay:0.0f];
    }
#endif // TARGET_OS_IPHONE
    
    cocos2d::extension::CCEditBoxDelegate* pDelegate = editBox_->getDelegate();
    if (pDelegate != NULL)
    {
        pDelegate->editBoxEditingDidBegin(editBox_->getCCEditBox());
    }
    
    cocos2d::extension::CCEditBox*  pEditBox= editBox_->getCCEditBox();
    if (NULL != pEditBox && 0 != pEditBox->getScriptEditBoxHandler())
    {
        cocos2d::CCScriptEngineProtocol* pEngine = cocos2d::CCScriptEngineManager::sharedManager()->getScriptEngine();
        pEngine->executeEvent(pEditBox->getScriptEditBoxHandler(), "began",pEditBox);
    }
    
    return YES;
}

- (BOOL)textFieldShouldEndEditing:(NativeTextField*)sender
{
    CCLOG("textFieldShouldEndEditing...");
    editState_ = NO;
    editBox_->setText(editBox_->getText());
    
    cocos2d::extension::CCEditBoxDelegate* pDelegate = editBox_->getDelegate();
    if (pDelegate != NULL)
    {
        pDelegate->editBoxEditingDidEnd(editBox_->getCCEditBox());
    }
    
    cocos2d::extension::CCEditBox*  pEditBox= editBox_->getCCEditBox();
    if (NULL != pEditBox && 0 != pEditBox->getScriptEditBoxHandler())
    {
        cocos2d::CCScriptEngineProtocol* pEngine = cocos2d::CCScriptEngineManager::sharedManager()->getScriptEngine();
        pEngine->executeEvent(pEditBox->getScriptEditBoxHandler(), "ended",pEditBox);
    }
	
	if (editBox_ != nil)
	{
		editBox_->onEndEditing();
	}
    
    return YES;
}

#if !TARGET_OS_IPHONE

// Mac
- (BOOL)control:(NSControl *)control textShouldBeginEditing:(NSText *)fieldEditor
{
    return [self textFieldShouldBeginEditing:(NativeTextField*) control];
}

- (BOOL)control:(NSControl *)control textShouldEndEditing:(NSText *)fieldEditor
{
    return [self textFieldShouldEndEditing:(NativeTextField*) control];
}

- (void)controlTextDidChange:(NSNotification *)aNotification
{
    [self textChanged];
}

-(void)controlTextDidEndEditing:(NSNotification *)notification
{
    // See if it was due to a return
    const int v = [[[notification userInfo] objectForKey:@"NSTextMovement"] intValue];
    
    if ( v == NSReturnTextMovement )
    {
        [self textFieldShouldReturn:textField_];
    }
    else
    {
        [self textFieldShouldEndEditing:textField_];
        [self closeKeyboard];
    }
}

#endif

/**
 * Delegate method called before the text has been changed.
 * @param textField The text field containing the text.
 * @param range The range of characters to be replaced.
 * @param string The replacement string.
 * @return YES if the specified text range should be replaced; otherwise, NO to keep the old text.
 */
- (BOOL)textField:(NativeTextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string
{
    if (editBox_->getMaxLength() < 0)
    {
        return YES;
    }
    
#if TARGET_OS_IPHONE
    NSUInteger oldLength = [textField.text length];
#else
    NSUInteger oldLength = [[textField stringValue] length];
#endif
    
    NSUInteger replacementLength = [string length];
    NSUInteger rangeLength = range.length;
    
    NSUInteger newLength = oldLength - rangeLength + replacementLength;
    
    return newLength <= editBox_->getMaxLength();
}

/**
 * Called each time when the text field's text has changed.
 */
- (void) textChanged
{
    // NSLog(@"text is %@", self.textField.text);
    cocos2d::extension::CCEditBoxDelegate* pDelegate = editBox_->getDelegate();
    if (pDelegate != NULL)
    {
        pDelegate->editBoxTextChanged(editBox_->getCCEditBox(), editBox_->getText());
    }
    
    cocos2d::extension::CCEditBox*  pEditBox= editBox_->getCCEditBox();
    if (NULL != pEditBox && 0 != pEditBox->getScriptEditBoxHandler())
    {
        cocos2d::CCScriptEngineProtocol* pEngine = cocos2d::CCScriptEngineManager::sharedManager()->getScriptEngine();
        pEngine->executeEvent(pEditBox->getScriptEditBoxHandler(), "changed",pEditBox);
    }
}

@end


NS_CC_EXT_BEGIN

CCEditBoxImpl* __createSystemEditBox(CCEditBox* pEditBox)
{
    return new CCEditBoxImplApple(pEditBox);
}

CCEditBoxImplApple::CCEditBoxImplApple(CCEditBox* pEditText)
: CCEditBoxImpl(pEditText)
, m_pLabel(NULL)
, m_pLabelPlaceHolder(NULL)
, m_systemControl(NULL)
, m_obAnchorPoint(ccp(0.5f, 0.5f))
, m_nMaxTextLength(-1)
{
#if TARGET_OS_IPHONE
    m_bInRetinaMode = [[EAGLView sharedEGLView] contentScaleFactor] == 2.0f ? true : false;
#else
    //! TODO: Retina on Mac
    //! m_bInRetinaMode = [[EAGLView sharedEGLView] contentScaleFactor] == 2.0f ? true : false;
    m_bInRetinaMode = false;
#endif
}

CCEditBoxImplApple::~CCEditBoxImplApple()
{
    [m_systemControl release];
}

void CCEditBoxImplApple::doAnimationWhenKeyboardMove(float duration, float distance)
{
    if ([m_systemControl isEditState] || distance < 0.0f)
    {
        [m_systemControl doAnimationWhenKeyboardMoveWithDuration:duration distance:distance];
    }
}

bool CCEditBoxImplApple::initWithSize(const CCSize& size)
{
    do 
    {
        CCEGLViewProtocol* eglView = CCEGLView::sharedOpenGLView();

        CGRect rect = CGRectMake(0, 0, size.width * eglView->getScaleX(),size.height * eglView->getScaleY());

        if (m_bInRetinaMode)
        {
            rect.size.width /= 2.0f;
            rect.size.height /= 2.0f;
        }
        
        m_systemControl = [[EditBoxImplApple alloc] initWithFrame:rect editBox:this];
        if (!m_systemControl) break;
        
		initInactiveLabels(size);
        setContentSize(size);
		
        return true;
    }while (0);
    
    return false;
}

void CCEditBoxImplApple::initInactiveLabels(const CCSize& size)
{
	const char* pDefaultFontName = [[m_systemControl.textField.font fontName] UTF8String];

	CCLabelTTF* label = CCLabelTTF::create("", "", 0.0f);
    label->setColor(ccWHITE);

    setLabel(label);
	
    m_pLabelPlaceHolder = CCLabelTTF::create("", "", 0.0f);
	// align the text vertically center
    m_pLabelPlaceHolder->setAnchorPoint(ccp(0, 0.5f));
    m_pLabelPlaceHolder->setColor(ccGRAY);
    m_pEditBox->addChild(m_pLabelPlaceHolder, kLabelZOrder);
    
    setFont(pDefaultFontName, size.height*2/3);
    setPlaceholderFont(pDefaultFontName, size.height*2/3);
}

static void _setupLabel(CCLabelTTF* iLabel, const CCRect& iInnerBounds)
{
    iLabel->ignoreAnchorPointForPosition(false);
    iLabel->setAnchorPoint(ccp(0, 0.5f));
    
    iLabel->setHorizontalAlignment(kCCTextAlignmentLeft);
    iLabel->setVerticalAlignment(kCCVerticalTextAlignmentCenter);
    
    iLabel->setPosition(ccp(iInnerBounds.origin.x, iInnerBounds.getMidY()));
    iLabel->setDimensions(iInnerBounds.size);
}

void CCEditBoxImplApple::placeInactiveLabels() {
    
    CCRect inputBox = m_pEditBox->inputLocalBounds();
    
    _setupLabel(m_pLabel, inputBox);
    _setupLabel(m_pLabelPlaceHolder, inputBox);
}

void CCEditBoxImplApple::setInactiveText(const char* pText)
{
#if TARGET_OS_IPHONE
	if (m_systemControl.textField.secureTextEntry == YES)
#else
    if ([m_systemControl.textField isKindOfClass:[NSSecureTextField class]])
#endif
	{
		std::string passwordString;
		for(int i = 0; i < strlen(pText); ++i)
			passwordString.append("\u25CF");
		m_pLabel->setString(passwordString.c_str());
	}
	else
    {
		m_pLabel->setString(getText());
	}
    
	// Clip the text width to fit to the text box
	float fMaxWidth = m_pEditBox->getContentSize().width;
	CCRect clippingRect = m_pLabel->getTextureRect();
	if(clippingRect.size.width > fMaxWidth) {
		clippingRect.size.width = fMaxWidth;
		m_pLabel->setTextureRect(clippingRect);
	}
}

void CCEditBoxImplApple::updateFontOfNativeTextField()
{
    const char* fontName = m_pLabel->getFontName();
    const float fontSize = m_pLabel->getFontSize();
    
    bool isValidFontName = true;
	if(fontName == NULL || strlen(fontName) == 0) {
        isValidFontName = false;
    }
    
    NSString * fntName = [NSString stringWithUTF8String:fontName];
    
#if TARGET_OS_IPHONE
    float retinaFactor = m_bInRetinaMode ? 2.0f : 1.0f;
    float scaleFactor = CCEGLView::sharedOpenGLView()->getScaleX();
    UIFont *textFont = nil;
    if (isValidFontName) {
        textFont = [UIFont fontWithName:fntName size:fontSize * scaleFactor / retinaFactor];
    }
    
    if (!isValidFontName || textFont == nil){
        textFont = [UIFont systemFontOfSize:fontSize * scaleFactor / retinaFactor];
    }
    
#else
    // Mac
    
    // Take into account overall scaling
    CCNode* parent = m_pEditBox->getParent();
    CCRect worldBox = m_pEditBox->boundingBox();
    
    if (parent != NULL)
    {
        worldBox = CCRectApplyAffineTransform(worldBox, parent->nodeToWorldTransform());
    }
    
    const float scale = worldBox.size.width / m_pEditBox->getContentSize().width;
    const float fSize = float(fontSize) * scale * CC_CONTENT_SCALE_FACTOR();
    NSFont *textFont = [NSFont fontWithName:fntName size:fSize];
#endif
    
    if(textFont != nil) {
        [m_systemControl.textField setFont:textFont];
    }
}

void CCEditBoxImplApple::setFont(const char* pFontName, int fontSize)
{
    if (m_pLabel != NULL)
    {
        m_pLabel->setFontName(pFontName);
        m_pLabel->setFontSize(fontSize);        
    }
    
    if (m_pLabelPlaceHolder != NULL)
    {
        m_pLabelPlaceHolder->setFontName(pFontName);
        m_pLabelPlaceHolder->setFontSize(fontSize);        
    }
    
    updateFontOfNativeTextField();
}

void CCEditBoxImplApple::setFontColor(const ccColor3B& color)
{
#if TARGET_OS_IPHONE
    m_systemControl.textField.textColor = [UIColor colorWithRed:color.r / 255.0f green:color.g / 255.0f blue:color.b / 255.0f alpha:1.0f];
#else
    m_systemControl.textField.textColor = [NSColor colorWithCalibratedRed:color.r / 255.0f green:color.g / 255.0f blue:color.b / 255.0f alpha:1.0f];
    //m_systemControl.textField.textColor = [NSColor redColor];
#endif
    
	m_pLabel->setColor(color);
}

void CCEditBoxImplApple::setPlaceholderFont(const char* pFontName, int fontSize)
{
	// TODO need to be implemented.
}

void CCEditBoxImplApple::setPlaceholderFontColor(const ccColor3B& color)
{
	m_pLabelPlaceHolder->setColor(color);
}

void CCEditBoxImplApple::setInputMode(EditBoxInputMode inputMode)
{
#if TARGET_OS_IPHONE
    switch (inputMode)
    {
        case kEditBoxInputModeEmailAddr:
            m_systemControl.textField.keyboardType = UIKeyboardTypeEmailAddress;
            break;
        case kEditBoxInputModeNumeric:
            m_systemControl.textField.keyboardType = UIKeyboardTypeNumbersAndPunctuation;
            break;
        case kEditBoxInputModePhoneNumber:
            m_systemControl.textField.keyboardType = UIKeyboardTypePhonePad;
            break;
        case kEditBoxInputModeUrl:
            m_systemControl.textField.keyboardType = UIKeyboardTypeURL;
            break;
        case kEditBoxInputModeDecimal:
            m_systemControl.textField.keyboardType = UIKeyboardTypeDecimalPad;
            break;
        case kEditBoxInputModeSingleLine:
            m_systemControl.textField.keyboardType = UIKeyboardTypeDefault;
            break;
        default:
            m_systemControl.textField.keyboardType = UIKeyboardTypeDefault;
            break;
    }
#else
    // Mac
    // TODO
    
#endif
}

void CCEditBoxImplApple::setMaxLength(int maxLength)
{
    m_nMaxTextLength = maxLength;
}

int CCEditBoxImplApple::getMaxLength()
{
    return m_nMaxTextLength;
}

void CCEditBoxImplApple::setInputFlag(EditBoxInputFlag inputFlag)
{
#if TARGET_OS_IPHONE
    switch (inputFlag)
    {
        case kEditBoxInputFlagPassword:
            m_systemControl.textField.secureTextEntry = YES;
            break;
        case kEditBoxInputFlagInitialCapsWord:
            m_systemControl.textField.autocapitalizationType = UITextAutocapitalizationTypeWords;
            break;
        case kEditBoxInputFlagInitialCapsSentence:
            m_systemControl.textField.autocapitalizationType = UITextAutocapitalizationTypeSentences;
            break;
        case kEditBoxInputFlagInitialCapsAllCharacters:
            m_systemControl.textField.autocapitalizationType = UITextAutocapitalizationTypeAllCharacters;
            break;
        case kEditBoxInputFlagSensitive:
            m_systemControl.textField.autocorrectionType = UITextAutocorrectionTypeNo;
            break;
        default:
            break;
    }
#else
    // Mac
    // TODO
#endif
}

void CCEditBoxImplApple::setReturnType(KeyboardReturnType returnType)
{
#if TARGET_OS_IPHONE
    switch (returnType) {
        case kKeyboardReturnTypeDefault:
            m_systemControl.textField.returnKeyType = UIReturnKeyDefault;
            break;
        case kKeyboardReturnTypeDone:
            m_systemControl.textField.returnKeyType = UIReturnKeyDone;
            break;
        case kKeyboardReturnTypeSend:
            m_systemControl.textField.returnKeyType = UIReturnKeySend;
            break;
        case kKeyboardReturnTypeSearch:
            m_systemControl.textField.returnKeyType = UIReturnKeySearch;
            break;
        case kKeyboardReturnTypeGo:
            m_systemControl.textField.returnKeyType = UIReturnKeyGo;
            break;
        default:
            m_systemControl.textField.returnKeyType = UIReturnKeyDefault;
            break;
    }
#else
    // Mac TODO
#endif
}

bool CCEditBoxImplApple::isEditing()
{
    return [m_systemControl isEditState] ? true : false;
}

void CCEditBoxImplApple::setText(const char* pText)
{
#if TARGET_OS_IPHONE
    m_systemControl.textField.text = [NSString stringWithUTF8String:pText];
#else
    // Mac
    m_systemControl.textField.stringValue = [NSString stringWithUTF8String:pText];
#endif
    
	if([m_systemControl.textField isHidden]) {
		setInactiveText(pText);
		if(strlen(pText) == 0)
		{
			m_pLabel->setVisible(false);
			m_pLabelPlaceHolder->setVisible(true);
		}
		else
		{
			m_pLabel->setVisible(true);
			m_pLabelPlaceHolder->setVisible(false);
		}
	}
}

const char*  CCEditBoxImplApple::getText(void)
{
#if TARGET_OS_IPHONE
    return [m_systemControl.textField.text UTF8String];
#else
    // Mac
    return [m_systemControl.textField.stringValue UTF8String];
#endif
}

void CCEditBoxImplApple::setPlaceHolder(const char* pText)
{
#if TARGET_OS_IPHONE
    m_systemControl.textField.placeholder = [NSString stringWithUTF8String:pText];
#else
    // Mac
    [[m_systemControl.textField cell] setPlaceholderString:[NSString stringWithUTF8String:pText]];
#endif
    
	m_pLabelPlaceHolder->setString(pText);
}

NativePoint CCEditBoxImplApple::convertDesignCoordToScreenCoord(const CCPoint& designCoord) const
{
    CCEGLViewProtocol* eglView = CCEGLView::sharedOpenGLView();
    
    CCPoint visiblePos = ccp(designCoord.x * eglView->getScaleX(), designCoord.y * eglView->getScaleY());
    CCPoint screenGLPos = ccpAdd(visiblePos, eglView->getViewPortRect().origin);
    
#if TARGET_OS_IPHONE
    float viewH = (float)[[EAGLView sharedEGLView] getHeight];
    
    CGPoint screenPos = CGPointMake(screenGLPos.x, viewH - screenGLPos.y);
#else
    NativeRect frame = [m_systemControl.textField frame];
    CGFloat height = frame.size.height;
    
    NSPoint screenPos = NSMakePoint(screenGLPos.x, screenGLPos.y - height);
#endif
    
    if (m_bInRetinaMode)
    {
        screenPos.x = screenPos.x / 2.0f;
        screenPos.y = screenPos.y / 2.0f;
    }
    //CCLOG("[EditBox] pos x = %f, y = %f", screenGLPos.x, screenGLPos.y); JFS: STOP the insanity
    return screenPos;
}

void CCEditBoxImplApple::setPosition(const CCPoint& pos)
{
	m_obPosition = pos;
    needsLayout();
}

void CCEditBoxImplApple::setVisible(bool visible)
{
//    m_systemControl.textField.hidden = !visible;
}

void CCEditBoxImplApple::setContentSize(const CCSize& size)
{
    m_tContentSize = size;

    needsLayout();
}

void CCEditBoxImplApple::setAnchorPoint(const CCPoint& anchorPoint)
{
    CCLOG("[Edit text] anchor point = (%f, %f)", anchorPoint.x, anchorPoint.y);
	m_obAnchorPoint = anchorPoint;
	setPosition(m_obPosition);
}

void CCEditBoxImplApple::visit(void)
{
    // Update at every frame, to track possible
    // position changes in ancestors
    adjustTextFieldPosition();
}

void CCEditBoxImplApple::onEnter(void)
{
    adjustTextFieldPosition();
    const char* pText = getText();
    if (pText) {
        setInactiveText(pText);
    }
}

void CCEditBoxImplApple::adjustTextFieldPosition()
{
	CCRect localBox = m_pEditBox->inputLocalBounds();
	CCRect rect = CCRectMake(localBox.origin.x, localBox.origin.y, localBox.size.width, localBox.size.height);
    rect = CCRectApplyAffineTransform(rect, m_pEditBox->nodeToWorldTransform());
	
	CCPoint designCoord = ccp(rect.origin.x, rect.origin.y + rect.size.height);
    [m_systemControl setPosition:convertDesignCoordToScreenCoord(designCoord)];
    
    [m_systemControl setContentSize:CGSizeMake(rect.size.width, rect.size.height)];
}

void CCEditBoxImplApple::openKeyboard()
{
	m_pLabel->setVisible(false);
	m_pLabelPlaceHolder->setVisible(false);

    updateFontOfNativeTextField();
    
	m_systemControl.textField.hidden = NO;
    [m_systemControl openKeyboard];
}

void CCEditBoxImplApple::closeKeyboard()
{
    [m_systemControl closeKeyboard];
}

void CCEditBoxImplApple::onEndEditing()
{
#if TARGET_OS_IPHONE
    [m_systemControl.textField endEditing:YES];
#else
    // Mac TODO
#endif
    
	m_systemControl.textField.hidden = YES;
	if(strlen(getText()) == 0)
	{
		m_pLabel->setVisible(false);
		m_pLabelPlaceHolder->setVisible(true);
	}
	else
	{
		m_pLabel->setVisible(true);
		m_pLabelPlaceHolder->setVisible(false);
		setInactiveText(getText());
	}
}

// DoW Additions

void CCEditBoxImplApple::onEnterTransitionDidFinish()
{
    const BOOL v = getCCEditBox()->isVisible();
    [m_systemControl setVisible:v];
}

void CCEditBoxImplApple::onExitTransitionDidStart()
{
    [m_systemControl setVisible:NO];
}

void CCEditBoxImplApple::setAutoCorrectionMode(EditBoxAutoCorrectionMode mode)
{
#if TARGET_OS_IPHONE
    switch (mode)
    {
        case KEditBoxAutoCorrectionDefault:
            m_systemControl.textField.autocorrectionType = UITextAutocorrectionTypeDefault;
            break;
            
        case KEditBoxAutoCorrectionNo:
            m_systemControl.textField.autocorrectionType = UITextAutocorrectionTypeNo;
            break;
            
        case KEditBoxAutoCorrectionYes:
            m_systemControl.textField.autocorrectionType = UITextAutocorrectionTypeYes;
            break;
            
        default:
            break;
    }
#else
    // Mac TODO
#endif
}

bool CCEditBoxImplApple::clearsOnBeginEditing() const
{
#if TARGET_OS_IPHONE
    return m_systemControl.textField.clearsOnBeginEditing;
#else
    // Mac TODO
    return false;
#endif
}

void
CCEditBoxImplApple::setClearsOnBeginEditing(bool iEnable)
{
#if TARGET_OS_IPHONE
    m_systemControl.textField.clearsOnBeginEditing = iEnable;
#else
    // Mac TODO
#endif
}

void
CCEditBoxImplApple::setClearButtonMode(EditBoxClearButtonMode iMode)
{
#if TARGET_OS_IPHONE
    UITextFieldViewMode mode;
    
    switch (iMode)
    {
        case kEditBoxClearButtonEditing:
        {
            mode = UITextFieldViewModeWhileEditing;
            break;
        }
            
        case kEditBoxClearButtonUnlessEditing:
        {
            mode = UITextFieldViewModeUnlessEditing;
            break;
        }
            
        case kEditBoxClearButtonAlways:
        {
            mode = UITextFieldViewModeAlways;
            break;
        }
            
        case kEditBoxClearButtonNever:
        default:
        {
            mode = UITextFieldViewModeNever;
            break;
        }
    }
    
    m_systemControl.textField.clearButtonMode = mode;
#else
    // Mac TODO
#endif
}

CCLabelTTF*
CCEditBoxImplApple::getLabel() const
{
    return m_pLabel;
}

void
CCEditBoxImplApple::setLabel(CCLabelTTF* iLabel)
{
    if (m_pLabel != iLabel)
    {
        if (m_pLabel != NULL)
        {
            m_pLabel->removeFromParentAndCleanup(true);
            CC_SAFE_RELEASE(m_pLabel);
        }
        
        m_pLabel = iLabel;
        CC_SAFE_RETAIN(m_pLabel);
        
        if (m_pLabel != NULL)
        {
            m_pLabel->removeFromParentAndCleanup(true);
            
            m_pLabel->setVisible(false);
            m_pEditBox->addChild(m_pLabel, kLabelZOrder);
            
            // Update the native text field accordingly
            setFont(m_pLabel->getFontName(), m_pLabel->getFontSize());
            setFontColor(m_pLabel->getDisplayedColor());
        }
    }
}

void CCEditBoxImplApple::needsLayout()
{
    placeInactiveLabels();
    adjustTextFieldPosition();
}

NS_CC_EXT_END

#endif /* __APPLE__ */


