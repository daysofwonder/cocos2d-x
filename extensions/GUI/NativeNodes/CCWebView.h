//
//  CCWebView.h
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCWebView__
#define __CCWebView__

#include "ExtensionMacros.h"
#include "layers_scenes_transitions_nodes/CCNativeSmartLayer.h"
#include "DOWFoundation/Url.h"
#include "DOWFoundation/Exception.h"
#include "CCBReader/CCSmartLayerLoader.h"

NS_CC_EXT_BEGIN

    using namespace DOW;
    
    class CCWebViewImpl;
    class CCWebViewDelegate;

    class CCWebView : public CCNativeSmartLayer
    {
    public:
        DOW_DECLARE_HIERARCHY(CCWebView, CCNativeSmartLayer);
        static CCWebView* create();
        virtual ~CCWebView();

        void loadUrl(const Url& iUrl, bool iForceReload = false);
        Url url() const;

        void loadHTML(const std::string& iHTMLCode);

        CCWebViewDelegate* delegate() const { return fDelegate; }
        void setDelegate(CCWebViewDelegate* iDelegate) { fDelegate = iDelegate; }

        void setBackgroundColor(const ccColor3B& iColor);
        void setBackgroundColorFromPlainText(const std::string& iColorAsString);

        enum FNavigationFeatures
        {
            fScrollingEnabled = 1 << 0,
            fZoomingEnabled = 1 << 1,
            fBouncingEnabled = 1 << 2,

            fDefaultNavigationFeatures = fScrollingEnabled | fZoomingEnabled | fBouncingEnabled
        };

        void setNavigationFeatures(uint32_t iNavigationFeatures);

        // See CCWebViewDelegate
        enum class ENavigationType
        {
            eLinkClicked,
            eFormSubmitted,
            eBackForward,
            eReload,
            eFormResubmitted,
            eOther
        };

    protected:
        CCWebView();

        virtual std::unique_ptr<CCNativeViewImpl> _createImpl() override;
        CCWebViewImpl* _webViewImpl() const;

        CCWebViewDelegate* fDelegate;
    };

    class CCWebViewDelegate
    {
    public:
        virtual ~CCWebViewDelegate() {}

        virtual bool webViewShouldStartLoadWithUrl(CCWebView* iWebView, const Url& iUrl, CCWebView::ENavigationType iType) { return true; }
        virtual void webViewDidStartLoad(CCWebView* iWebView) {}
        virtual void webViewDidFinishLoad(CCWebView *iWebView) {}
        virtual void webViewDidFailLoadWithException(CCWebView* iWebView, const Exception& iException) {}

    };

    using namespace cocos2d;
    using namespace cocos2d::extension;
    
    class CCWebViewLoader : public CCSmartLayerLoader
    {
    public:
        DOW_DECLARE_HIERARCHY(CCWebViewLoader, CCSmartLayerLoader);

        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCWebViewLoader, loader);
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CCWebView);

        virtual void onHandlePropTypeString(CCNode * pNode, CCNode * pParent, const char* pPropertyName, const char * pString, CCBReader * pCCBReader) override;
    };

NS_CC_EXT_END

#endif /* defined(__CCWebView__) */
