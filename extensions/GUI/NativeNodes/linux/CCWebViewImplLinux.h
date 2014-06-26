//
#include "GUI/NativeNodes/CCWebViewImpl.h"
#include "CCNativeViewImplLinux.h"
#include <webkit/webkit.h>
#include "GUI/NativeNodes/CCWebView.h"

NS_CC_EXT_BEGIN

    class WebViewLinux : public CCNativeViewImplLinux , public CCWebViewImpl
    {
	public:
		WebViewLinux();
		virtual ~WebViewLinux();

        virtual void onEnterTransitionDidFinish() override;
        virtual void onExitTransitionDidStart() override;

        virtual void loadUrl(const DOW::Url& iUrl, bool iForceReload);
        virtual DOW::Url url() const;
        virtual void loadHTML(const std::string& iHTMLCode);

        virtual void setBackgroundColor(const cocos2d::ccColor3B& iColor);
        virtual void setNavigationFeatures(uint32_t iFeatures);

	protected:
		virtual void _onShow() override final;
		virtual void _onHide() override final;

    private:
        DOW::Url fUrl;
        std::string fHtml;
        cocos2d::ccColor3B fBackgroundColor;
        uint32_t fFeatures;
        GtkWidget* fWebView;

        gulong fLoadError;
        gulong fLoadStatus;
        gulong fExposeEvent;
        gulong fNavigation;

        void _startWebView();
        void _stopWebView();
        void _nativeSetPage();

        CCWebView* webView();
        CCWebViewDelegate* delegate();
        bool sendShouldStartLoadWithUrl(const Url& iUrl, CCWebView::ENavigationType iType);
        void sendDidStartLoad();
        void sendDidFinishLoad();
        void sendDidFailLoadWithException(const Exception& iException);

        static gboolean navigationPolicyDecisionRequested (WebKitWebView *web_view,
               WebKitWebFrame            *frame,
               WebKitNetworkRequest      *request,
               WebKitWebNavigationAction *navigation_action,
               WebKitWebPolicyDecision   *policy_decision,
               gpointer                   user_data);

        static void notifyLoadStatus(GObject* object, GParamSpec* pspec, gpointer user_data);
        static gboolean loadError (WebKitWebView  *web_view,
               WebKitWebFrame *web_frame,
               gchar          *uri,
               GError         *web_error,
               gpointer        user_data);
    };

NS_CC_EXT_END

