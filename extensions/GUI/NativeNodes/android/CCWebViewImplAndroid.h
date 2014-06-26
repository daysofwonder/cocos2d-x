//
//  CCWebViewImplAndroid.h
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCWebViewImplAndroid__
#define __CCWebViewImplAndroid__

#include "GUI/NativeNodes/android/CCNativeViewImplAndroid.h"
#include "GUI/NativeNodes/CCWebViewImpl.h"

NS_CC_EXT_BEGIN

    class CCWebView;
    
    class CCWebViewImplAndroid : public CCNativeViewImplAndroid, public CCWebViewImpl
    {
    public:
        DOW_DECLARE_HIERARCHY(CCWebViewImplAndroid, CCNativeViewImplAndroid);
        
        CCWebViewImplAndroid();
        virtual ~CCWebViewImplAndroid();
                                
        virtual void loadUrl(const Url& iUrl, bool iForceReload) override;
        virtual Url url() const override;

        virtual void loadHTML(const std::string& iHTMLCode) override;

        virtual void setBackgroundColor(const ccColor3B& iColor) override;
        virtual void setNavigationFeatures(uint32_t iFeatures) override;
        
        CCWebView* webView() const;
        
        // Called by Java
        void onPageStarted();
        void onPageFinished();
        void shouldOverrideUrlLoading(jstring iUrl);
        void onReceivedError(jint iErrorCode, jstring iDescription, jstring iFailingUrl);

    private:
        virtual std::string _nativeViewControllerFullName() override;
    };

NS_CC_EXT_END

#endif /* defined(__CCWebViewImplAndroid__) */

