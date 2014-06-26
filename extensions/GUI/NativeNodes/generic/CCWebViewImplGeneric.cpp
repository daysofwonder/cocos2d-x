//
#include "CCWebView.h"
#include "CCWebViewImpl.h"
#include "CCNativeViewImpl.h"

namespace swapp
{
    
    class NoWebView : public CCNativeViewImpl , public CCWebViewImpl
    {
	public:
		NoWebView() {}
		virtual ~NoWebView() {}

        virtual bool init(CCNode* iNode) { return true; }
        virtual CCNode* owner() const { return nullptr; }
        virtual void onEnter() {}
        virtual void onEnterTransitionDidFinish() {}
        virtual void onExit(void) {}
        virtual void onExitTransitionDidStart() {}
        
        virtual void loadUrl(const Url& iUrl, bool iForceReload) {}
        virtual Url url() const { return Url(); }
        virtual void loadHTML(const std::string& iHTMLCode) {}
        
        virtual void setBackgroundColor(const ccColor3B& iColor) {}
        virtual void setNavigationFeatures(uint32_t iFeatures) {}
    };
    
    std::unique_ptr<CCNativeViewImpl>
    CCWebView::_createImpl()
    {
        return std::unique_ptr<CCNativeViewImpl>(new NoWebView());
    }
    
}

