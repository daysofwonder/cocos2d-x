//
//  CCWebViewImplIOS.h
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCWebViewImplIOS__
#define __CCWebViewImplIOS__

#include "GUI/NativeNodes/apple/CCNativeViewImplApple.h"
#include "CCWebViewImpl.h"
#include <UIKit/UIKit.h>
#include "DOWFoundation/DOWFoundation.h"

@class CCWebViewImplIOS_WebViewDelegate;

NS_CC_EXT_BEGIN

    class CCWebView;
    
    class CCWebViewImplIOS : public CCNativeViewImplApple, public CCWebViewImpl
    {
    public:
        DOW_DECLARE_HIERARCHY(CCWebViewImplIOS, CCNativeViewImplApple);
        
        CCWebViewImplIOS();
        virtual ~CCWebViewImplIOS();
                                
        virtual void loadUrl(const DOW::Url& iUrl, bool iForceReload) override;
        virtual DOW::Url url() const override;

        virtual void loadHTML(const std::string& iHTMLCode) override;

        virtual void setBackgroundColor(const ccColor3B& iColor) override;
        virtual void setNavigationFeatures(uint32_t iFeatures) override;
        
        CCWebView* webView() const;
        
        UIWebView* uiWebView() const;
        
    private:
        virtual AppleView* _createUIView() override;
        CCWebViewImplIOS_WebViewDelegate* fWebViewDelegate;
    };

NS_CC_EXT_END

#endif /* defined(__CCWebViewImplIOS__) */
