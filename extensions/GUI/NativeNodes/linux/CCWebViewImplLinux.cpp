//
#include "CCWebViewImplLinux.h"
#include "GUI/NativeNodes/CCWebView.h"
#include <boost/log/trivial.hpp>
#include <webkit/webkit.h>
#include "CCOperationPool.h"
#include "platform/linux/SW2EGLView.h"
#include "DOWFoundation/Library.h"

// to finish move with SW2EGLView...
using namespace swapp;

namespace
{

class FakeLib: public DOW::Library<FakeLib> {
};


gboolean exposeTransparent(GtkWidget *widget, GdkEventExpose *event, gpointer userdata)
{
	cairo_t *cr = gdk_cairo_create(widget->window);
	cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
	cairo_paint (cr);
	cairo_destroy(cr);
	return FALSE;
}

}

NS_CC_EXT_BEGIN


    WebViewLinux::WebViewLinux()
    {
    	fWebView =  GTK_WIDGET(g_object_ref(webkit_web_view_new()));
        gtk_object_sink(GTK_OBJECT(fWebView));
    	const std::string defaultStyleSheet = "file://"+FakeLib::instance().pathForResource("defaultStyleSheet.css");
        webkit_web_view_set_transparent(WEBKIT_WEB_VIEW(fWebView),true); // IMPORTANT
        webkit_web_view_set_view_mode(WEBKIT_WEB_VIEW(fWebView),WEBKIT_WEB_VIEW_VIEW_MODE_FLOATING);
        WebKitWebSettings *settings = webkit_web_settings_new ();
        g_object_set (G_OBJECT(settings), "user-stylesheet-uri", defaultStyleSheet.c_str(), NULL);
        webkit_web_view_set_settings (WEBKIT_WEB_VIEW(fWebView), settings);
        fExposeEvent=g_signal_connect(G_OBJECT(fWebView), "expose-event", G_CALLBACK(exposeTransparent), nullptr);
        fNavigation=g_signal_connect(fWebView,"navigation-policy-decision-requested",G_CALLBACK(navigationPolicyDecisionRequested) , this);
        fLoadStatus=g_signal_connect(fWebView,"notify::load-status", G_CALLBACK(notifyLoadStatus), this);
        fLoadError = g_signal_connect(fWebView,"load-error", G_CALLBACK(loadError), this);
    }

    WebViewLinux::~WebViewLinux()
    {
    	g_signal_handler_disconnect(fWebView , fExposeEvent);
    	g_signal_handler_disconnect(fWebView , fNavigation);
    	g_signal_handler_disconnect(fWebView , fLoadStatus);
    	g_signal_handler_disconnect(fWebView , fLoadError);
    	g_object_unref(fWebView);
    }

    void WebViewLinux::_onShow()
    {
    	BOOST_LOG_TRIVIAL(info) << "WebViewLinux::_onShow" << fUrl.urlAsString()  << std::endl;

    	if (!isAttached()) {
    		_startWebView();
    	}
    }

    void WebViewLinux::_onHide()
    {
    	BOOST_LOG_TRIVIAL(info) << "WebViewLinux::_onHide" << fUrl.urlAsString() << std::endl;

    	if (isAttached()) {
    		_stopWebView();
    	}
    }

    void WebViewLinux::onEnterTransitionDidFinish()
    {
    	BOOST_LOG_TRIVIAL(info) << "WebViewLinux::onEnterTransitionDidFinish" << std::endl;
		SW2EGLView::sharedOpenGLView()->postToGTK([this](){
			_startWebView();
    	});
    }

    void WebViewLinux::onExitTransitionDidStart()
    {
    	BOOST_LOG_TRIVIAL(info) << "WebViewLinux::onExitTransitionDidStart" << std::endl;
		SW2EGLView::sharedOpenGLView()->postToGTK([this](){
			if (isAttached()) {
				_stopWebView();
			}
    	});
   }


    gboolean
    WebViewLinux::navigationPolicyDecisionRequested (WebKitWebView *web_view,
            WebKitWebFrame            *frame,
            WebKitNetworkRequest      *request,
            WebKitWebNavigationAction *navigation_action,
            WebKitWebPolicyDecision   *policy_decision,
            gpointer                   user_data)
    {
        Url url;
        bool ok = url.createFromString(webkit_network_request_get_uri(request));
        assert(ok);

        const WebKitWebNavigationReason reason = webkit_web_navigation_action_get_reason(navigation_action);

        BOOST_LOG_TRIVIAL(info) << "navigationPolicyDecisionRequested " << url.urlAsString().c_str() << " why: " << reason;

        WebViewLinux* self = static_cast<WebViewLinux *>(user_data);
        assert(self);

        CCWebView::ENavigationType type;
        switch(reason) {
        case WEBKIT_WEB_NAVIGATION_REASON_LINK_CLICKED:
            type = CCWebView::ENavigationType::eLinkClicked;
            break;
        case WEBKIT_WEB_NAVIGATION_REASON_FORM_SUBMITTED:
            type = CCWebView::ENavigationType::eFormSubmitted;
            break;
        case WEBKIT_WEB_NAVIGATION_REASON_BACK_FORWARD:
            type = CCWebView::ENavigationType::eBackForward;
            break;
        case WEBKIT_WEB_NAVIGATION_REASON_RELOAD:
            type = CCWebView::ENavigationType::eReload;
            break;
        case WEBKIT_WEB_NAVIGATION_REASON_FORM_RESUBMITTED:
            type = CCWebView::ENavigationType::eFormResubmitted;
            break;
        case WEBKIT_WEB_NAVIGATION_REASON_OTHER:
            type = CCWebView::ENavigationType::eOther;
            break;
        default:
            assert(false);
            return false;

        }
        const bool shouldStart = self->sendShouldStartLoadWithUrl(url, type);
        if (shouldStart) {
            webkit_web_policy_decision_use(policy_decision);
        } else {
            webkit_web_policy_decision_ignore(policy_decision);
        }

        return true;
    }



    void
    WebViewLinux::notifyLoadStatus(GObject* object, GParamSpec* pspec, gpointer user_data)
    {
        WebViewLinux* self = static_cast<WebViewLinux *>(user_data);
        assert(self);
        WebKitWebView *web_view = WEBKIT_WEB_VIEW(object);
        const WebKitLoadStatus status = webkit_web_view_get_load_status(web_view);
        const gchar *uri = webkit_web_view_get_uri(web_view);
        if (!uri) {
            uri = "N/A";
        }
        switch (status) {
        case WEBKIT_LOAD_PROVISIONAL:
            BOOST_LOG_TRIVIAL(info) << "Load provisional: " << uri;
            self->sendDidStartLoad();
            break;
        case WEBKIT_LOAD_COMMITTED:
            BOOST_LOG_TRIVIAL(info) << "Load commited: " << uri;
            break;
        case WEBKIT_LOAD_FIRST_VISUALLY_NON_EMPTY_LAYOUT:
            BOOST_LOG_TRIVIAL(info) << "Load first layout: " << uri;
            break;
        case WEBKIT_LOAD_FINISHED:
        {
            BOOST_LOG_TRIVIAL(info) << "Load finished: " << uri;
            Url newUrl;
            if (newUrl.createFromString(uri) || newUrl.createFromSystemPath(uri)) {
            	self->fUrl = newUrl;
            } else {
                BOOST_LOG_TRIVIAL(error) << "Cannot parse: " << uri;
            }
            self->sendDidFinishLoad();
        }
            break;
        case WEBKIT_LOAD_FAILED:
            BOOST_LOG_TRIVIAL(error) << "Load failed: " << uri;
            //self->sendDidFailLoadWithException(DOW::Exception("WEBKIT_LOAD_FAILED")); NO NEED WILL BE DONE BY WebViewLinux::loadError
        	break;
        default:
            assert(false);
        }
    }

    gboolean
    WebViewLinux::loadError (WebKitWebView  *web_view,
                             WebKitWebFrame *web_frame,
                             gchar          *uri,
                             GError         *web_error,
                             gpointer        user_data)
    {
        WebViewLinux* self = static_cast<WebViewLinux *>(user_data);
        assert(self);
        self->sendDidFailLoadWithException(DOW::Exception(web_error->message));
        return true;
    }

    void
    WebViewLinux::_startWebView()
    {
    	BOOST_LOG_TRIVIAL(info) << "WebViewLinux::_startWebView" << std::endl;
    	assert(!isAttached());
    	attachView(fWebView);
    	_nativeSetPage();
    }

    void
    WebViewLinux::_stopWebView()
    {
    	BOOST_LOG_TRIVIAL(info) << "WebViewLinux::_stopWebView" << std::endl;
    	assert(isAttached());
        detachView();
    }

    void
    WebViewLinux::loadUrl(const Url& iUrl, bool iForceReload)
    {
        BOOST_LOG_TRIVIAL(info) << "WebViewLinux::loadUrl " << iUrl.urlAsString();
        fUrl = iUrl;
        fHtml.clear();
        _nativeSetPage();
    }

    Url WebViewLinux::url() const
    {
        return fUrl;
    }
    void WebViewLinux::loadHTML(const std::string& iHTMLCode)
    {
        fUrl = Url();
        fHtml = iHTMLCode;
        _nativeSetPage();
    }

    void WebViewLinux::_nativeSetPage()
    {
    	if (isAttached()) {
			if (!fHtml.empty()) {
				webkit_web_view_load_string(WEBKIT_WEB_VIEW(fWebView),fHtml.c_str(),nullptr,nullptr,"");
			} else if (fUrl.isValid()) {
				webkit_web_view_load_uri(WEBKIT_WEB_VIEW(fWebView), fUrl.urlAsString().c_str());
			}
    	}
    }

    void WebViewLinux::setBackgroundColor(const ccColor3B& iColor)
    {
        fBackgroundColor = iColor;
    }
    void WebViewLinux::setNavigationFeatures(uint32_t iFeatures)
    {
        fFeatures=iFeatures;
    }

    CCWebView*
    WebViewLinux::webView()
    {
        cocos2d::CCNode* owner = this->owner();
        if (owner) {
            CCWebView* webView = dynamic_cast<CCWebView *>(owner);
            if (webView) {
                return webView;
            }
        }
        return nullptr;

    }

    CCWebViewDelegate* WebViewLinux::delegate()
    {
        CCWebView* webView = this->webView();
        if (webView) {
            CCWebViewDelegate* delegate = webView->delegate();
            if (delegate) {
                return delegate;
            }
        }
        return nullptr;
    }

    bool
    WebViewLinux::sendShouldStartLoadWithUrl(const Url& iUrl, CCWebView::ENavigationType iType)
    {
        CCWebViewDelegate* delegate = this->delegate();
        if (delegate) {
            return delegate->webViewShouldStartLoadWithUrl(webView(),iUrl,iType);
        }
        return true;
    }
    void
    WebViewLinux::sendDidStartLoad()
    {
        CCWebViewDelegate* delegate = this->delegate();
        if (delegate) {
            delegate->webViewDidStartLoad(webView());
        }
    }
    void
    WebViewLinux::sendDidFinishLoad()
    {
        CCWebViewDelegate* delegate = this->delegate();
        if (delegate) {
            delegate->webViewDidFinishLoad(webView());
        }
    }
    void
    WebViewLinux::sendDidFailLoadWithException(const Exception& iException)
    {
        CCWebViewDelegate* delegate = this->delegate();
        if (delegate) {
            delegate->webViewDidFailLoadWithException(webView(),iException);
        }

    }


    std::unique_ptr<CCNativeViewImpl>
    CCWebView::_createImpl()
    {
        return std::unique_ptr<CCNativeViewImpl>(new WebViewLinux());
    }

NS_CC_EXT_END

