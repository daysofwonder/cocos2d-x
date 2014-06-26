//
//  CCWebViewImplOSX.h
//
//  Created by Gérald Guyomard on 05/03/13.
//
//

#ifndef __CCWebViewImplOSX__
#define __CCWebViewImplOSX__

#include "GUI/NativeNodes/apple/CCNativeViewImplApple.h"
#include "GUI/NativeNodes/CCWebViewImpl.h"
#include <WebKit/WebKit.h>
#include "DOWFoundation/DOWFoundation.h"

@class WebViewDelegate;

NS_CC_EXT_BEGIN

    class CCWebView;
    
    class CCWebViewImplOSX : public CCNativeViewImplApple, public CCWebViewImpl
    {
    public:
        DOW_DECLARE_HIERARCHY(CCWebViewImplOSX, CCNativeViewImplApple);
        
        CCWebViewImplOSX();
        virtual ~CCWebViewImplOSX();
                                
        virtual void loadUrl(const DOW::Url& iUrl, bool iForceReload) override;
        virtual DOW::Url url() const override;

        virtual void loadHTML(const std::string& iHTMLCode) override;

        virtual void setBackgroundColor(const ccColor3B& iColor) override;
        virtual void setNavigationFeatures(uint32_t iFeatures) override;
        
        CCWebView* webView() const;
        
        WebView* uiWebView() const;
                
    private:
        virtual NSView* _createUIView() override;
        WebViewDelegate* fWebViewDelegate;
    };

NS_CC_EXT_END

#endif /* defined(__CCWebViewImplOSX__) */
