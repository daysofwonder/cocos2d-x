//
#include "CCMultilineTextBoxImplLinux.h"
#include "linux/SW2EGLView.h"
#include "DOWFoundation/NotificationCenter.h"
#include <webkit/webkit.h>
#include <boost/log/trivial.hpp>
#include <cocos2d.h>

using namespace cocos2d;
using namespace cocos2d::extension;

// to finish move with SW2EGLView...
using namespace swapp;

namespace
{
const char *kTextAreaElementID="dow_text_area";

gboolean expose(GtkWidget *widget, GdkEventExpose *event, gpointer userdata)
{
	   cairo_t *cr = gdk_cairo_create(widget->window);
	    cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
	    cairo_paint (cr);
	    cairo_destroy(cr);
	    return FALSE;
}

std::string glToCss(const ccColor4B& glColor) {
	std::stringstream ss;
	float alpha = glColor.a;
	int red = glColor.r;
	int green = glColor.g;
	int blue = glColor.b;
	alpha /=255;
	ss << "rgba(" << red << "," << green << "," << blue << "," << alpha << ")";
	return ss.str();
}
}


NS_CC_EXT_BEGIN


CCMultilineTextBoxImplLinux::CCMultilineTextBoxImplLinux() {
	fOwner=nullptr;
	fWebView =  nullptr;
	fTextArea = nullptr;
	fBackgroundColor.r = 0;
	fBackgroundColor.g = 0;
	fBackgroundColor.b = 0;
	fBackgroundColor.a = 0;
	fTextColor.r = 0;
	fTextColor.g = 0;
	fTextColor.b = 0;
	fTextColor.a = 255;
}

CCMultilineTextBoxImplLinux::~CCMultilineTextBoxImplLinux() {
	assert(!fWebView);
	assert(!fTextArea);
}

bool CCMultilineTextBoxImplLinux::init(CCNode* iNode) {
	fOwner = dynamic_cast<CCMultilineTextBox *>(iNode);
	assert(fOwner);
	CCNativeViewImplLinux::init(fOwner);
	return true;
}

CCNode* CCMultilineTextBoxImplLinux::owner() const {
	return fOwner;
}

void CCMultilineTextBoxImplLinux::onEnter()
{
	CCNativeViewImplLinux::onEnter();
}

void CCMultilineTextBoxImplLinux::onEnterTransitionDidFinish()
{
	//_attachBox();
}

void CCMultilineTextBoxImplLinux::onExit(void)
{
	CCNativeViewImplLinux::onExit();
}

void CCMultilineTextBoxImplLinux::onExitTransitionDidStart()
{
	//_detachBox();
}

std::string CCMultilineTextBoxImplLinux::getText() const {
	_nativeGetText();
	return fText;
}

void CCMultilineTextBoxImplLinux::setText(const std::string& iText)
{
	fText=iText;
	_nativeSetText();
}

void CCMultilineTextBoxImplLinux::setFont(const std::string& pName, float iSize)
{
	fFontName = pName;
	fFontSize = iSize;
	_nativeSetFont();
}

void CCMultilineTextBoxImplLinux::setBackgroundColor(const ccColor4B& iColor)
{
	fBackgroundColor=iColor;
	_nativeSetBackgroundColor();
}

void CCMultilineTextBoxImplLinux::setTextColor(const ccColor4B& iColor)
{
	fTextColor=iColor;
	_nativeSetTextColor();
}

void CCMultilineTextBoxImplLinux::retainFocus()
{
	//_attachBox();
}

void CCMultilineTextBoxImplLinux::releaseFocus()
{
	//_detachBox();
}

void CCMultilineTextBoxImplLinux::_nativeGetText() const
{
    WebKitDOMHTMLTextAreaElement* textArea = WEBKIT_DOM_HTML_TEXT_AREA_ELEMENT(fTextArea);
    if (textArea) {
    	fText = webkit_dom_html_text_area_element_get_value(textArea);
    }
}

void CCMultilineTextBoxImplLinux::_nativeSetText()
{
    WebKitDOMHTMLTextAreaElement* textArea = WEBKIT_DOM_HTML_TEXT_AREA_ELEMENT(fTextArea);
    if (textArea) {
    	webkit_dom_html_text_area_element_set_value(textArea, fText.c_str());
    }
}

void CCMultilineTextBoxImplLinux::_nativeSetFont()
{
    WebKitDOMHTMLTextAreaElement* textArea = WEBKIT_DOM_HTML_TEXT_AREA_ELEMENT(fTextArea);
    if (textArea) {
    	WebKitDOMCSSStyleDeclaration* style = webkit_dom_element_get_style(WEBKIT_DOM_ELEMENT(textArea));
        assert(style);

        CCNode* parent = fOwner->getParent();
         CCRect worldBox = fOwner->boundingBox();

         if (parent != NULL)
         {
             worldBox = CCRectApplyAffineTransform(worldBox, parent->nodeToWorldTransform());
         }

         const float scale = worldBox.size.width / fOwner->getContentSize().width;
         float fontSize = float(fFontSize) * scale * CCEGLView::sharedOpenGLView()->getScaleX();
         //fSize /= frameBufferScale();

        std::stringstream ss;
        ss << fontSize << "px";
        const std::string value = ss.str();
        webkit_dom_css_style_declaration_set_property(style,"font-size",value.c_str(),"",nullptr);

        webkit_dom_css_style_declaration_set_property(style,"font-family",fFontName.c_str(),"",nullptr);
    }
}

void CCMultilineTextBoxImplLinux::_nativeSetTextColor()
{
    WebKitDOMHTMLTextAreaElement* textArea = WEBKIT_DOM_HTML_TEXT_AREA_ELEMENT(fTextArea);
    if (textArea) {
    	WebKitDOMCSSStyleDeclaration* style = webkit_dom_element_get_style(WEBKIT_DOM_ELEMENT(textArea));
        assert(style);
        const std::string value = glToCss(fTextColor);
        GError* error = nullptr;
        webkit_dom_css_style_declaration_set_property(style,"color",value.c_str(),"",&error);
        if (error) {
            BOOST_LOG_TRIVIAL(error) << "cannot set text area color to " << value << " : " << error->message << std::endl;
        	g_error_free (error);
        }
   }
}

void CCMultilineTextBoxImplLinux::_nativeSetBackgroundColor()
{
    WebKitDOMHTMLTextAreaElement* textArea = WEBKIT_DOM_HTML_TEXT_AREA_ELEMENT(fTextArea);
    if (textArea) {
    	WebKitDOMCSSStyleDeclaration* style = webkit_dom_element_get_style(WEBKIT_DOM_ELEMENT(textArea));
        assert(style);
        const std::string value = glToCss(fBackgroundColor);
        GError* error = nullptr;
        webkit_dom_css_style_declaration_set_property(style,"background-color",value.c_str(),"",nullptr);
        if (error) {
            BOOST_LOG_TRIVIAL(error) << "cannot set text area background-color to " << value << " : " << error->message << std::endl;
        	g_error_free (error);
        }
    }
}

gboolean CCMultilineTextBoxImplLinux::_onKeyPress(GtkWidget* target, GObject* event, gpointer opaque)
{
	WebKitDOMEvent* myEvent = WEBKIT_DOM_EVENT(event);
	CCMultilineTextBoxImplLinux* self = static_cast<CCMultilineTextBoxImplLinux*>(opaque);
	self->_nativeGetText();
	self->_sendTextChanged(self->fText);

	return TRUE;
}

gboolean CCMultilineTextBoxImplLinux::_onKeyRelease(GtkWidget* target, GdkEventKey* event, gpointer opaque)
{
    static const char returnChar('\n');

    CCMultilineTextBoxImplLinux* self = static_cast<CCMultilineTextBoxImplLinux*>(opaque);
    self->_nativeGetText();
    if(self->fText.length() > 0 && (self->fText.at(self->fText.length()-1) == returnChar)) {
        self->_sendTextChanged(self->fText);
        self->_sendEditingDidEnd();
    }
	return TRUE;
}


void CCMultilineTextBoxImplLinux::_loadFinished(GtkWidget *view, const char *uri, gpointer user_data)
{
	CCMultilineTextBoxImplLinux* self = static_cast<CCMultilineTextBoxImplLinux *>(user_data);

    assert(view);
    WebKitWebView* webview = WEBKIT_WEB_VIEW(view);
    assert(WEBKIT_IS_WEB_VIEW(webview));
    WebKitDOMDocument* document = webkit_web_view_get_dom_document(webview);
    assert(document);
    WebKitDOMElement* element =  webkit_dom_document_get_element_by_id(document,kTextAreaElementID);
    assert(element);
    WebKitDOMHTMLTextAreaElement* textArea = WEBKIT_DOM_HTML_TEXT_AREA_ELEMENT(element);

    assert(WEBKIT_DOM_IS_EVENT_TARGET(textArea));
    assert(WEBKIT_DOM_IS_HTML_TEXT_AREA_ELEMENT(textArea));


	self->fTextArea = G_OBJECT(textArea);
	self->_nativeSetTextColor();
	self->_nativeSetBackgroundColor();
	self->_nativeSetFont();
	self->_nativeSetText();

	WebKitDOMEventTarget* target = WEBKIT_DOM_EVENT_TARGET(webkit_dom_node_get_parent_element(WEBKIT_DOM_NODE(textArea)));
    webkit_dom_event_target_add_event_listener(target,"keypress" ,GCallback(_onKeyPress) , false , self);
    webkit_dom_event_target_add_event_listener(target,"keyup" ,GCallback(_onKeyRelease) , false , self);

#if 1
    {
    WebKitDOMHTMLElement *body = webkit_dom_document_get_body(document);
    gchar* outerBody = webkit_dom_html_element_get_outer_html(body);
    BOOST_LOG_TRIVIAL(info) << "CCMultilineTextBoxImplLinux::_loadFinished: " << outerBody << std::endl;
    }
#endif

    self->attachView(GTK_WIDGET(webview)); // webview ownership transfered to GTK
    self->fWebView = GTK_WIDGET(webview);
    gtk_widget_grab_focus(GTK_WIDGET(webview));
    self->_sendEditingDidBegin();
}

void CCMultilineTextBoxImplLinux::_createWebView()
{
    WebKitWebView* webview = WEBKIT_WEB_VIEW(webkit_web_view_new());
    assert(webview);
    webkit_web_view_set_transparent(webview,true); // IMPORTANT
    g_signal_connect(GTK_WIDGET(webview), "load-finished", G_CALLBACK(_loadFinished), this);
    g_signal_connect(G_OBJECT(webview), "expose-event", G_CALLBACK(expose), NULL);

    std::stringstream ss;
    ss << "<HTML>";
    ss << "<HEAD>";
    ss << "</HEAD>";
    ss << "<BODY style=\"margin: 0; padding: 0;\">";
    ss << "<TEXTAREA id=\"" << kTextAreaElementID << "\" autofocus style=\"width: 100%; height: 100%; ";
    ss << "border: none; margin: 0; padding: 0; resize: none; overflow: hidden; \">";
    ss << "</TEXTAREA>";
    ss << "</BODY>";
    ss << "</HTML>";

    webkit_web_view_load_string(webview,ss.str().c_str(),nullptr,nullptr,"");
    //fWebView = GTK_WIDGET(webview);
}

void CCMultilineTextBoxImplLinux::_onShow()
{
	_attachBox();
}

void CCMultilineTextBoxImplLinux::_onHide()
{
	_detachBox();
}

void CCMultilineTextBoxImplLinux::_attachBox()
{
	assert(!fWebView);
	_createWebView();
	//assert(fWebView);
    //attachView(fWebView); // webview ownership transfered to GTK

}

void CCMultilineTextBoxImplLinux::_detachBox()
{
	assert(fWebView);

	WebKitDOMEventTarget* target = WEBKIT_DOM_EVENT_TARGET(webkit_dom_node_get_parent_element(WEBKIT_DOM_NODE(fTextArea)));
	webkit_dom_event_target_remove_event_listener(target,"keypress" ,GCallback(_onKeyPress) , false);
	webkit_dom_event_target_remove_event_listener(target,"keyup" ,GCallback(_onKeyRelease) , false);
	_sendEditingDidEnd();

    detachView();
    fWebView=nullptr; // view released by GTK in above call, pointer is now invalid
    fTextArea = nullptr;
}

CCMultilineTextBoxDelegate* CCMultilineTextBoxImplLinux::_delegate()
{
	if (fOwner) {
		return fOwner->delegate();
	}
	return nullptr;
}
void CCMultilineTextBoxImplLinux::_sendEditingDidBegin()
{
	CCMultilineTextBoxDelegate* delegate = _delegate();
	if (delegate) {
		delegate->textBoxEditingDidBegin(fOwner);
	}
}

void CCMultilineTextBoxImplLinux::_sendEditingDidEnd()
{
	CCMultilineTextBoxDelegate* delegate = _delegate();
	if (delegate) {
		delegate->textBoxEditingDidEnd(fOwner);
	}
}

void CCMultilineTextBoxImplLinux::_sendTextChanged(const std::string& text)
{
	CCMultilineTextBoxDelegate* delegate = _delegate();
	if (delegate) {
		delegate->textBoxTextChanged(fOwner,text);
    }
}

std::unique_ptr<CCNativeViewImpl> CCMultilineTextBox::_createImpl() {
	return std::unique_ptr<CCNativeViewImpl>(new CCMultilineTextBoxImplLinux());
}

NS_CC_EXT_END


