//
//  CCWebView.cpp
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "GUI/NativeNodes/CCWebView.h"
#include "GUI/NativeNodes/CCWebViewImpl.h"

#include "layers_scenes_transitions_nodes/CCNativeViewImpl.h"

NS_CC_EXT_BEGIN

    CCWebView*
    CCWebView::create()
    {
        CCWebView* pRet = new CCWebView();
        if (pRet && pRet->init())
        {
            pRet->autorelease();
        }
        else
        {
            CC_SAFE_DELETE(pRet);
        }
        return pRet;
    }
    
    CCWebView::~CCWebView()
    {
    }
        
    CCWebViewImpl*
    CCWebView::_webViewImpl() const
    {
		CCNativeViewImpl* nativeImpl = _impl();
		CCWebViewImpl* impl = dynamic_cast<CCWebViewImpl*>(nativeImpl);
        assert(impl != nullptr);
        return impl;
    }
    
    void
    CCWebView::loadUrl(const Url& iUrl, bool iForceReload)
    {
        _webViewImpl()->loadUrl(iUrl, iForceReload);
    }
    
    Url
    CCWebView::url() const
    {
        return _webViewImpl()->url();
    }
    
    void
    CCWebView::loadHTML(const std::string& iHTMLCode)
    {
        _webViewImpl()->loadHTML(iHTMLCode);
    }
    
    CCWebView::CCWebView()
    : fDelegate(nullptr)
    {
    }
    
    void
    CCWebView::setBackgroundColor(const ccColor3B &iColor)
    {
        _webViewImpl()->setBackgroundColor(iColor);
    }

    void
    CCWebView::setNavigationFeatures(uint32_t iNavigationFeatures)
    {
        _webViewImpl()->setNavigationFeatures(iNavigationFeatures);
    }
    
    void
    CCWebView::setBackgroundColorFromPlainText(const std::string& iColorAsString)
    {        
        ccColor3B color = ccColor3B::decode(iColorAsString);
        setBackgroundColor(color);
    }
    
    void
    CCWebViewLoader::onHandlePropTypeString(CCNode * pNode, CCNode * pParent, const char* pPropertyName, const char * pString, CCBReader * pCCBReader)
    {
        CCWebView* webView = static_cast<CCWebView*>(pNode);
        assert(dynamic_cast<CCWebView*>(pNode) != nullptr);

        if (::strcmp(pPropertyName, "backgroundColor") == 0)
        {
            webView->setBackgroundColorFromPlainText(pString);
        }
        else
        {
            SuperClass::onHandlePropTypeString(pNode, pParent, pPropertyName, pString, pCCBReader);
        }
    }

NS_CC_EXT_END
