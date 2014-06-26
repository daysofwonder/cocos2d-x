//
#include "CCEditBoxImplLinux.h"
#include <cassert>
#include "platform/linux/SW2EGLView.h"
#include <boost/log/trivial.hpp>
#include <webkit/webkit.h>
#include <sstream>
#include <gdk/gdkkeysyms.h>

// to finish move with SW2EGLView...
using namespace swapp;

namespace cocos2d {
namespace extension {

CCEditBoxImpl* __createSystemEditBox(CCEditBox* pEditBox) {
	return new cocos2d::extension::CCEditBoxImplLinux(pEditBox);
}

}
}

namespace
{
const char *kInputElementID="dow_input";


gboolean expose(GtkWidget *widget, GdkEventExpose *event, gpointer userdata)
{
//    BOOST_LOG_TRIVIAL(info) << "EXPOSE EDIT BOX WEBVIEW" << std::endl;

	   cairo_t *cr = gdk_cairo_create(widget->window);
	    cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
	    cairo_paint (cr);
	    cairo_destroy(cr);
	    return FALSE;
}

}

NS_CC_EXT_BEGIN

CCEditBoxImplLinux::CCEditBoxImplLinux(cocos2d::extension::CCEditBox* pEditText) :
		cocos2d::extension::CCEditBoxImpl(pEditText) {
	fEntry=nullptr;
    fLabel = nullptr;
    fFontSize = 20;
    fInputFlag = cocos2d::extension::kEditBoxInputFlagNone;
}

CCEditBoxImplLinux::~CCEditBoxImplLinux() {
	assert(!fEntry);
	if (fLabel) {
		CC_SAFE_RELEASE(fLabel);
		fLabel=nullptr;
	}
}

bool CCEditBoxImplLinux::initWithSize(const cocos2d::CCSize& size) {
	fLabel = cocos2d::CCLabelTTF::create("", "", size.height-12);
	_positionLabel();
	m_pEditBox->addChild(fLabel);
	assert(fLabel);
	CCNativeViewImplLinux::init(m_pEditBox);
	return true;
}

void CCEditBoxImplLinux::setFont(const char* pFontName, int fontSize) {
	if (fLabel != nullptr) {
		fLabel->setFontName(pFontName);
		fLabel->setFontSize(fontSize);
	}

	fFontName = pFontName;
	fFontSize = fontSize;
	_nativeSetFont();
}

void CCEditBoxImplLinux::_nativeSetFont()
{
    WebKitDOMHTMLInputElement* input = WEBKIT_DOM_HTML_INPUT_ELEMENT(_getInputElement());
    if (input) {
    	assert(WEBKIT_DOM_IS_HTML_INPUT_ELEMENT(input));
    	WebKitDOMCSSStyleDeclaration* style = webkit_dom_element_get_style(WEBKIT_DOM_ELEMENT(input));
        assert(style);
        std::stringstream ss;

#if 0
        const float fontSize = fFontSize*cocos2d::CCDirector::sharedDirector()->getContentScaleFactor();
#else
        const float scaleFactor = SW2EGLView::sharedOpenGLView()->getScaleX() /*/ getScreenDensity()*/;
        // Take into account overall scaling
        cocos2d::CCNode* parent = m_pEditBox->getParent();
        cocos2d::CCRect worldBox = m_pEditBox->boundingBox();

        if (parent != NULL)
        {
            worldBox = CCRectApplyAffineTransform(worldBox, parent->nodeToWorldTransform());
        }

        const float scale = worldBox.size.width / m_pEditBox->getContentSize().width;
        const float fontSize = float(fFontSize) * scale * scaleFactor;
#endif


        ss << fontSize << "px";
        const std::string value = ss.str();
        GError* error = nullptr;
        webkit_dom_css_style_declaration_set_property(style,"font-size",value.c_str(),"",&error);
        if (error) {
            BOOST_LOG_TRIVIAL(error) << "cannot set input style font size to " << value << " : " << error->message << std::endl;
        	g_error_free (error);
        }

        webkit_dom_css_style_declaration_set_property(style,"font-family",fFontName.c_str(),"",&error);
        if (error) {
            BOOST_LOG_TRIVIAL(error) << "cannot set input style font family to " << fFontName << " : " << error->message << std::endl;
        	g_error_free (error);
        }

    }
}

void CCEditBoxImplLinux::setFontColor(const cocos2d::ccColor3B& color) {
	fLabel->setColor(color);
}

void CCEditBoxImplLinux::setPlaceholderFont(const char* pFontName, int fontSize) {
}
void CCEditBoxImplLinux::setPlaceholderFontColor(const cocos2d::ccColor3B& color) {
}
void CCEditBoxImplLinux::setInputMode(cocos2d::extension::EditBoxInputMode inputMode) {
}

void CCEditBoxImplLinux::setInputFlag(cocos2d::extension::EditBoxInputFlag inputFlag) {
	fInputFlag = inputFlag;
}

void CCEditBoxImplLinux::setMaxLength(int maxLength) {
}
int CCEditBoxImplLinux::getMaxLength() {
}
void CCEditBoxImplLinux::setReturnType(cocos2d::extension::KeyboardReturnType returnType) {
}

bool CCEditBoxImplLinux::isEditing() {
	return fEntry;
}

void CCEditBoxImplLinux::setText(const char* pText) {
	fText = pText;
	_nativeSetText();

	assert(fLabel);
	fLabel->setString(pText);
}

const char* CCEditBoxImplLinux::getText(void) {
	_nativeGetText(fText);
	return fText.c_str();
}

void CCEditBoxImplLinux::setPlaceHolder(const char* pText) {
}
void CCEditBoxImplLinux::setPosition(const cocos2d::CCPoint& pos) {
}
void CCEditBoxImplLinux::setVisible(bool visible) {
}
void CCEditBoxImplLinux::setContentSize(const cocos2d::CCSize& size) {
}
void CCEditBoxImplLinux::setAnchorPoint(const cocos2d::CCPoint& anchorPoint) {
}
void CCEditBoxImplLinux::visit(void) {
}
void CCEditBoxImplLinux::doAnimationWhenKeyboardMove(float duration, float distance) {
}
void CCEditBoxImplLinux::openKeyboard() {
	if(!fEntry) {
		_beginEdit();
	}
}
void CCEditBoxImplLinux::closeKeyboard() {
	if (fEntry) {
		_endEdit();
	}
}
void CCEditBoxImplLinux::onEnter(void) {
}

// DoW Additions
void CCEditBoxImplLinux::onEnterTransitionDidFinish() {
//dead not called
}

void CCEditBoxImplLinux::onExitTransitionDidStart() {
//dead not called
}

void CCEditBoxImplLinux::setAutoCorrectionMode(cocos2d::extension::EditBoxAutoCorrectionMode mode) {
}
bool CCEditBoxImplLinux::clearsOnBeginEditing() const {
}
void CCEditBoxImplLinux::setClearsOnBeginEditing(bool iEnable) {
}
void CCEditBoxImplLinux::setClearButtonMode(cocos2d::extension::EditBoxClearButtonMode iMode) {
}
void CCEditBoxImplLinux::needsLayout() {
	_positionLabel();
}

cocos2d::CCLabelTTF* CCEditBoxImplLinux::getLabel() const {
	return fLabel;
}

void CCEditBoxImplLinux::setLabel(cocos2d::CCLabelTTF* iLabel) {
#if 0
	if (fLabel) {
		fLabel->removeFromParentAndCleanup(true);
		CC_SAFE_RELEASE(fLabel);
		fLabel=nullptr;
	}
	fLabel = iLabel;
	CC_SAFE_RETAIN(fLabel);
#else
	const int kLabelZOrder = 9999;

	if (fLabel != iLabel)
	{
		if (fLabel != NULL)
		{
			fLabel->removeFromParentAndCleanup(true);
			CC_SAFE_RELEASE(fLabel);
		}

		fLabel = iLabel;
		CC_SAFE_RETAIN(fLabel);

		if (fLabel != NULL)
		{
			fLabel->removeFromParentAndCleanup(true);

			fLabel->setVisible(false);



			m_pEditBox->addChild(fLabel, kLabelZOrder);

			// Update the native text field accordingly
			setFont(fLabel->getFontName(), fLabel->getFontSize());
			setFontColor(fLabel->getDisplayedColor());

			fLabel->setVisible(true); // JFS: true by default, we might want to be smarter WHEN required

		}
	}
#endif
}

//******

void CCEditBoxImplLinux::_beginEdit()
{
	fLabel->setVisible(false);
	_attachEntry();
}

void CCEditBoxImplLinux::_endEdit()
{
	assert(fLabel);
    assert(fEntry);

    _nativeGetText(fText);
	_detachEntry();

	std::string text;
    if (fInputFlag == cocos2d::extension::kEditBoxInputFlagPassword) {
        for(int i=0; i < fText.length(); i++)
    	text.append("*");
    } else {
    	text = fText.c_str();
    }
	fLabel->setString(text.c_str());
	fLabel->setVisible(true);
}

GObject* CCEditBoxImplLinux::_getInputElement()
{
	if (!fEntry) {
		return nullptr;
	}
    WebKitWebView* webview = WEBKIT_WEB_VIEW(fEntry);
    assert(webview);


    WebKitDOMDocument* document = webkit_web_view_get_dom_document(webview);

    if (!document) {
        BOOST_LOG_TRIVIAL(info) << "_getInputElement No HTML Doc" << std::endl;
        return nullptr;
    }

#if 0
    WebKitDOMHTMLElement *body = webkit_dom_document_get_body(document);
    {
    gchar* outerBody = webkit_dom_html_element_get_outer_html(body);
    BOOST_LOG_TRIVIAL(info) << "_getInputElement outerBody: " << outerBody << std::endl;
    }
#endif

    assert(document);
    WebKitDOMElement* element =  webkit_dom_document_get_element_by_id(document,kInputElementID);
    if (!element) {
    	return nullptr;
    }
    assert(element);
    WebKitDOMHTMLInputElement* input = WEBKIT_DOM_HTML_INPUT_ELEMENT(element);
    assert(input);
    return G_OBJECT(input);
}

void CCEditBoxImplLinux::_nativeGetText(std::string& oText)
{
    WebKitDOMHTMLInputElement* input = WEBKIT_DOM_HTML_INPUT_ELEMENT(_getInputElement());
    if (input) {
		gchar *value =webkit_dom_html_input_element_get_value(input);
		assert(value);
		oText = value;
    }
}

void CCEditBoxImplLinux::_nativeSetText()
{
    WebKitDOMHTMLInputElement* input = WEBKIT_DOM_HTML_INPUT_ELEMENT(_getInputElement());
    if (input) {
    	webkit_dom_html_input_element_set_value(WEBKIT_DOM_HTML_INPUT_ELEMENT(input),fText.c_str());
    }
}

void CCEditBoxImplLinux::_loadFinished(GtkWidget *view, const char *uri, gpointer user_data)
{
	CCEditBoxImplLinux* self = static_cast<CCEditBoxImplLinux *>(user_data);
	self->_nativeSetFont();
	self->_nativeSetText();
}


GtkWidget* CCEditBoxImplLinux::_createWebView()
{
    WebKitWebView* webview = WEBKIT_WEB_VIEW(webkit_web_view_new());
    assert(webview);
    webkit_web_view_set_transparent(webview,true); // IMPORTANT
    g_signal_connect(G_OBJECT(webview), "expose-event", G_CALLBACK(expose), NULL);
    g_signal_connect(GTK_WIDGET(webview), "load-finished", G_CALLBACK(_loadFinished), this);

    std::stringstream ss;
    ss << "<HTML>";
    ss << "<HEAD>";
    ss << "<STYLE type=\"text/css\">";
//    ss << "html {background-color: rgba(0,0,255,0.3);}";
//    ss << "body {background-color: rgba(0,255,0,0.3);}";
    ss << "</STYLE>";
    ss << "</HEAD>";
    ss << "<BODY style=\"margin: 0; padding: 0;\">";
    ss << "<INPUT id=\"" << kInputElementID << "\" autofocus ";
    if (fInputFlag == cocos2d::extension::kEditBoxInputFlagPassword) {
    	ss << " type=\"password\" ";
    } else {
    	ss << " type=\"text\" ";
    }
    ss << "style=\"background-color: transparent; width: 100%; height: 100%; border: none; margin: 0; padding: 0;\">";
    ss << "</BODY>";
    ss << "</HTML>";

    webkit_web_view_load_string(webview,ss.str().c_str(),nullptr,nullptr,"");
    return GTK_WIDGET(webview);
}

void CCEditBoxImplLinux::_attachEntry()
{
    BOOST_LOG_TRIVIAL(info) << "CCEditBoxImplLinux::_attachEntry " << std::endl;


    assert(!fEntry);

    const cocos2d::CCRect viewRect = _calcViewRect();

    fEntry = _createWebView();
    assert(fEntry);
    gtk_widget_set_events(fEntry, GDK_FOCUS_CHANGE_MASK);

    attachView(fEntry);

    fOnFocusIn=g_signal_connect(fEntry, "focus-in-event", G_CALLBACK (onFocusIn), this);
    fOnFocusOut=g_signal_connect(fEntry, "focus-out-event", G_CALLBACK (onFocusOut), this);
    fOnKeyReleased=g_signal_connect(fEntry, "key-release-event", G_CALLBACK(onKeyReleased), this);

    gtk_widget_grab_focus(fEntry);
}

void CCEditBoxImplLinux::_detachEntry()
{
    BOOST_LOG_TRIVIAL(info) << "CCEditBoxImplLinux::_detachEntry " << std::endl;

    assert(fEntry);

    gulong handler_id;
    g_signal_handler_disconnect(fEntry,fOnFocusIn);
    g_signal_handler_disconnect(fEntry,fOnFocusOut);
    g_signal_handler_disconnect(fEntry,fOnKeyReleased);

    _nativeGetText(fText);
    detachView();
    gtk_widget_destroy(fEntry);
    fEntry = nullptr;
}

cocos2d::CCRect CCEditBoxImplLinux::_calcViewRect()
{
	cocos2d::extension::CCEditBox* owner = getCCEditBox();
    assert(owner);
    cocos2d::CCNode* parent = owner->getParent();

    //cocos2d::CCRect r = owner->boundingBox();
    cocos2d::CCRect r = owner->inputLocalBounds();

    r = CCRectApplyAffineTransform(r, parent->nodeToWorldTransform());

    // flip Y
    const float kHeight = SW2EGLView::sharedOpenGLView()->getHeight();
    const float flipY = kHeight - (r.origin.y + r.size.height);
    r.origin.y = flipY;


    cocos2d::CCEGLViewProtocol* eglView = cocos2d::CCEGLView::sharedOpenGLView();
    const float scaleX = eglView->getScaleX();
    const float scaleY = eglView->getScaleY();

    BOOST_LOG_TRIVIAL(info) << "CCEditBoxImplLinux::_calcViewRect scale: " << scaleX << "," << scaleY  << std::endl;

    r.origin.x *= scaleX;
    r.origin.y *= scaleY;
    r.size.width *= scaleX;
    r.size.height *= scaleY;

    return r;
}

cocos2d::CCRect
CCEditBoxImplLinux::_getContentArea()
{
    return  m_pEditBox->inputLocalBounds();
}

void CCEditBoxImplLinux::_positionLabel()
{
	cocos2d::CCRect iInnerBounds = m_pEditBox->inputLocalBounds();

	fLabel->ignoreAnchorPointForPosition(false);
	fLabel->setAnchorPoint(ccp(0, 0.5f));

	fLabel->setHorizontalAlignment(cocos2d::kCCTextAlignmentLeft);
	fLabel->setVerticalAlignment(cocos2d::kCCVerticalTextAlignmentCenter);

	fLabel->setPosition(ccp(iInnerBounds.origin.x, iInnerBounds.getMidY()));
	fLabel->setDimensions(iInnerBounds.size);

}

gboolean CCEditBoxImplLinux::onFocusIn(GtkWidget* widget,GdkEventFocus* event,gpointer user_data)
{
    BOOST_LOG_TRIVIAL(info) << "CCEditBoxImplLinux::onFocusIn " << event->type  << std::endl;

	CCEditBoxImplLinux* self = static_cast<CCEditBoxImplLinux*>(user_data);
	if (self->m_pDelegate) {
		self->m_pDelegate->editBoxEditingDidBegin(self->m_pEditBox);
	}
	return false;
}

gboolean CCEditBoxImplLinux::onFocusOut(GtkWidget* widget,GdkEventFocus* event,gpointer user_data)
{
    BOOST_LOG_TRIVIAL(info) << "CCEditBoxImplLinux::onFocusOut " << event->type  << std::endl;

	CCEditBoxImplLinux* self = static_cast<CCEditBoxImplLinux*>(user_data);
	self->_endEdit();
	if (self->m_pDelegate) {
		self->m_pDelegate->editBoxEditingDidEnd(self->m_pEditBox);
	}
	return false;
}

gboolean CCEditBoxImplLinux::onKeyReleased(GtkWidget* widget,GdkEventKey* event,gpointer user_data)
{
    BOOST_LOG_TRIVIAL(info) << "CCEditBoxImplLinux::onKeyReleased " << event->type  << std::endl;

    CCEditBoxImplLinux* self = static_cast<CCEditBoxImplLinux*>(user_data);
    if(strncmp(event->string,"\r", 1) == 0) {
        self->_endEdit();
        if (self->m_pDelegate) {
            self->m_pDelegate->editBoxReturn(self->m_pEditBox);
            self->m_pDelegate->editBoxEditingDidEnd(self->m_pEditBox);
        }
        return true;
    } else {
        if (self->m_pDelegate) {
            self->getText(); // update self->fText
            self->m_pDelegate->editBoxTextChanged(self->m_pEditBox, self->fText);
        }
    }


	return false;
}

NS_CC_EXT_END

