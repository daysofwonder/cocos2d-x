//
//  CCWebViewImplAndroid.cpp
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "GUI/NativeNodes/android/CCWebViewImplAndroid.h"
#include "GUI/NativeNodes/CCWebView.h"
#include "CCOperationPool.h"

// Native Implementations
 extern "C"
{
	using namespace cocos2d;
	using namespace cocos2d::extension;

    /**
    * this method is called by java code
    */
    JNIEXPORT void JNICALL Java_com_daysofwonder_smallworld2_WebViewController_onPageStarted(JNIEnv*  env, jobject thiz, jlong iOwner)
    {
    	CCWebViewImplAndroid* impl = reinterpret_cast<CCWebViewImplAndroid*>(iOwner);
    	impl->onPageStarted();
    }

    JNIEXPORT void JNICALL Java_com_daysofwonder_smallworld2_WebViewController_onPageFinished(JNIEnv*  env, jobject thiz, jlong iOwner)
    {
    	CCWebViewImplAndroid* impl = reinterpret_cast<CCWebViewImplAndroid*>(iOwner);
    	impl->onPageFinished();
    }

    JNIEXPORT void JNICALL Java_com_daysofwonder_smallworld2_WebViewController_shouldOverrideUrlLoading(JNIEnv*  env, jobject thiz, jlong iOwner, jstring iUrl)
    {
    	CCWebViewImplAndroid* impl = reinterpret_cast<CCWebViewImplAndroid*>(iOwner);
    	impl->shouldOverrideUrlLoading(iUrl);
    }

    // onReceivedError(long iOwner, int errorCode, String iDescription, String iFailingUrl);
    JNIEXPORT void JNICALL Java_com_daysofwonder_smallworld2_WebViewController_onReceivedError(JNIEnv*  env, jobject thiz, jlong iOwner, jint iErrorCode, jstring iDescription, jstring iFailingUrl)
    {
    	CCWebViewImplAndroid* impl = reinterpret_cast<CCWebViewImplAndroid*>(iOwner);
    	impl->onReceivedError(iErrorCode, iDescription, iFailingUrl);
    }
}

NS_CC_EXT_BEGIN

    std::unique_ptr<CCNativeViewImpl>
    CCWebView::_createImpl()
    {
        std::unique_ptr<CCNativeViewImpl> impl(new CCWebViewImplAndroid);
        return impl;
    }
    
    CCWebViewImplAndroid::CCWebViewImplAndroid()
    {
    }
    
    CCWebViewImplAndroid::~CCWebViewImplAndroid()
    {
    }
    
    static const std::string kWebViewControllerJavaClassName = "com/daysofwonder/smallworld2/WebViewController";

    std::string
    CCWebViewImplAndroid::_nativeViewControllerFullName()
    {
        return kWebViewControllerJavaClassName;
    }
 
    CCWebView*
    CCWebViewImplAndroid::webView() const
    {
        return static_cast<CCWebView*>(owner());
    }
    
    void
    CCWebViewImplAndroid::loadUrl(const Url& iUrl, bool iForceReload)
    {
    	auto controller = nativeViewController();
    	assert(controller != nullptr);

    	auto& manager = JNIManager::instance();

        auto method = manager.getMethodInfo(kWebViewControllerJavaClassName, "loadUrl", "(Ljava/lang/String;)V");

        if (method != nullptr)
        {
        	const auto urlAsString = iUrl.urlAsString();
            auto str = manager.string2jstring(urlAsString);
             jstring s = str->javaObject();

             method->callVoidMethod(controller, s);
        }
    }
    
    void
    CCWebViewImplAndroid::loadHTML(const std::string &iHTMLCode)
    {
    	auto controller = nativeViewController();
    	assert(controller != nullptr);

    	auto& manager = JNIManager::instance();

        auto method = manager.getMethodInfo(kWebViewControllerJavaClassName, "loadHTML", "(Ljava/lang/String;)V");

        if (method != nullptr)
        {
            auto str = manager.string2jstring(iHTMLCode);
             jstring s = str->javaObject();

             method->callVoidMethod(controller, s);
        }
    }
    
    Url
    CCWebViewImplAndroid::url() const
    {
       	auto controller = nativeViewController();
		assert(controller != nullptr);

		auto& manager = JNIManager::instance();

		auto method = manager.getMethodInfo(kWebViewControllerJavaClassName, "getUrl", "()Ljava/lang/String;");

        Url url;
		if (method != nullptr)
		{
			 auto str = method->callTypedObjectMethod<jstring>(controller);
			 const std::string urlAsString = manager.jstring2string(str->javaObject());

			 url.createFromString(urlAsString);
		}

        return url;
    }
    
    void
    CCWebViewImplAndroid::setBackgroundColor(const ccColor3B& iColor)
    {
    }

    void
    CCWebViewImplAndroid::setNavigationFeatures(uint32_t iFeatures)
    {
    }

    // Callback by delegation
    void
    CCWebViewImplAndroid::onPageStarted()
    {
    	CCOperationPool::instance()->executeInMainThread(
    		[this]()
    		{
    			auto* w = webView();
    			auto* delegate = w->delegate();
    			if (delegate != nullptr)
    			{
    				delegate->webViewDidStartLoad(w);
    			}
    		});
    }

    void
    CCWebViewImplAndroid::onPageFinished()
    {
    	CCOperationPool::instance()->executeInMainThread(
    		[this]()
    		{
    			auto* w = webView();
    			auto* delegate = w->delegate();
    			if (delegate != nullptr)
    			{
    				delegate->webViewDidFinishLoad(w);
    			}
    		});
    }

    void
    CCWebViewImplAndroid::shouldOverrideUrlLoading(jstring iUrl)
    {
    	std::string urlAsString = JNIManager::instance().jstring2string(iUrl);
    	Url url;
    	url.createFromString(urlAsString);

       	CCOperationPool::instance()->executeInMainThread(
        		[this, url]()
        		{
        			auto* w = webView();

        			auto* delegate = w->delegate();
        			if ((delegate == nullptr) || delegate->webViewShouldStartLoadWithUrl(w, url, CCWebView::ENavigationType::eLinkClicked))
        			{
        				loadUrl(url, false);
        			}
        		});
    }

    void
    CCWebViewImplAndroid::onReceivedError(jint iErrorCode, jstring iDescription, jstring iFailingUrl)
    {
		auto& manager = JNIManager::instance();
		const std::string description = manager.jstring2string(iDescription);
		const std::string failingUrl = manager.jstring2string(iFailingUrl);

       	CCOperationPool::instance()->executeInMainThread(
        		[this, iErrorCode, description, failingUrl]()
        		{
        			auto* w = webView();

        			auto* delegate = w->delegate();
        			if (delegate != nullptr)
        			{
        				Exception ex("error when loading <%s> (%d, %s)", failingUrl.c_str(), (int) iErrorCode, description.c_str());
        				delegate->webViewDidFailLoadWithException(w, ex);
        			}
        		});
    }

NS_CC_EXT_END

