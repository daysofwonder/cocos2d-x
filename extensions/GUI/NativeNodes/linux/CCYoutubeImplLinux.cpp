//
#include "CCYoutubeImplLinux.h"
#include "GUI/NativeNodes/CCYoutubeView.h"
#include "linux/SW2EGLView.h"
#include <boost/log/trivial.hpp>

// to finish move with SW2EGLView...
using namespace swapp;

namespace{

    gboolean
    loadError (WebKitWebView  *web_view,
                             WebKitWebFrame *web_frame,
                             gchar          *uri,
                             GError         *web_error,
                             gpointer        user_data)
    {
        cocos2d::extension::YoutubeLinux* self = static_cast<cocos2d::extension::YoutubeLinux *>(user_data);
        assert(self);
        //self->sendDidFailLoadWithException(DOW::Exception(web_error->message));
        BOOST_LOG_TRIVIAL(error) << "YoutubeLinux::loadError: " << web_error->message;

        return true;
    }

    gboolean
    navigationPolicyDecisionRequested (WebKitWebView *web_view,
            WebKitWebFrame            *frame,
            WebKitNetworkRequest      *request,
            WebKitWebNavigationAction *navigation_action,
            WebKitWebPolicyDecision   *policy_decision,
            gpointer                   user_data)
    {
        const gchar* rawURI = webkit_network_request_get_uri(request);
        const WebKitWebNavigationReason reason = webkit_web_navigation_action_get_reason(navigation_action);

        BOOST_LOG_TRIVIAL(info) << "navigationPolicyDecisionRequested " << rawURI << " why: " << reason;

        cocos2d::extension::YoutubeLinux* self = static_cast<cocos2d::extension::YoutubeLinux *>(user_data);
        assert(self);

        //webkit_web_policy_decision_use(policy_decision);
        webkit_web_policy_decision_ignore(policy_decision);
        return true;
    }

    void
    notifyLoadStatus(GObject* object, GParamSpec* pspec, gpointer user_data)
    {
        cocos2d::extension::YoutubeLinux* self = static_cast<cocos2d::extension::YoutubeLinux *>(user_data);
        assert(self);
        WebKitWebView *web_view = WEBKIT_WEB_VIEW(object);
        const WebKitLoadStatus status = webkit_web_view_get_load_status(web_view);
        const gchar *uri = webkit_web_view_get_uri(web_view);
        if (!uri) {
            uri = "N/A";
        }
        switch (status) {
        case WEBKIT_LOAD_PROVISIONAL:
            BOOST_LOG_TRIVIAL(info) << "Youtube Load provisional: " << uri;
            break;
        case WEBKIT_LOAD_COMMITTED:
            BOOST_LOG_TRIVIAL(info) << "Youtube Load commited: " << uri;
            break;
        case WEBKIT_LOAD_FIRST_VISUALLY_NON_EMPTY_LAYOUT:
            BOOST_LOG_TRIVIAL(info) << "Youtube Load first layout: " << uri;
            break;
        case WEBKIT_LOAD_FINISHED:
            BOOST_LOG_TRIVIAL(info) << "Youtube Load finished: " << uri;
            break;
        case WEBKIT_LOAD_FAILED:
            BOOST_LOG_TRIVIAL(error) << "Youtube Load failed: " << uri;
        	break;
        default:
            assert(false);
        }
    }
}


NS_CC_EXT_BEGIN

    YoutubeLinux::YoutubeLinux()
    {
        fWebKitView = WEBKIT_WEB_VIEW(webkit_web_view_new());
    }

    YoutubeLinux::~YoutubeLinux()
    {
        gtk_widget_destroy(GTK_WIDGET(fWebKitView));
    }

    void YoutubeLinux::onEnterTransitionDidFinish()
    {
        SW2EGLView::sharedOpenGLView()->postToGTK([this]() {
            attachView(GTK_WIDGET(fWebKitView));
            //gtk_container_add(GTK_CONTAINER(containee()), GTK_WIDGET(fWebKitView));
            //gtk_widget_grab_focus(GTK_WIDGET(fWebKitView));
            //gtk_widget_show (GTK_WIDGET(fWebKitView));

            g_signal_connect(fWebKitView,"load-error", G_CALLBACK(loadError), this);
            g_signal_connect(fWebKitView,"notify::load-status", G_CALLBACK(notifyLoadStatus), this);
            g_signal_connect(fWebKitView,"navigation-policy-decision-requested",G_CALLBACK(navigationPolicyDecisionRequested) , this);
        });

    }

    void YoutubeLinux::onExitTransitionDidStart()
    {
        SW2EGLView::sharedOpenGLView()->postToGTK([this]() {
            detachView();
        });

    }

    void
    YoutubeLinux::playMovie(const std::string& iMovieID)
    {
        const std::string youtubeUrlPrefix("http://www.youtube.com/embed/");
        //const std::string youtubeUrlPostfix("?autoplay=1&fs=1&hd=1&fmt=22&cc_load_policy=1&cc=1&rel=0");
        const std::string youtubeUrlPostfix("?rel=0");
#if 1
        const std::string urlStr = youtubeUrlPrefix + iMovieID + youtubeUrlPostfix;
#else
        const std::string urlStr = "http://www.youtube.com/html5";
#endif
        webkit_web_view_load_uri(fWebKitView, urlStr.c_str());
    }

    std::unique_ptr<CCNativeViewImpl>
    CCYoutubeView::_createImpl()
    {
        return std::unique_ptr<CCNativeViewImpl>(new YoutubeLinux());
    }

NS_CC_EXT_END


