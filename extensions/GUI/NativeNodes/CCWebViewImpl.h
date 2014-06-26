//
//  CCWebViewImpl.h
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCWebViewImpl__
#define __CCWebViewImpl__

#include "DOWFoundation/Url.h"
#include "ccTypes.h"
#include <cstdint>
#include "ExtensionMacros.h"

NS_CC_EXT_BEGIN

    class CCWebView;
    class CCWebViewImpl
    {
    public:
        virtual ~CCWebViewImpl() {}

        virtual void loadUrl(const DOW::Url& iUrl, bool iForceReload) = 0;
        virtual DOW::Url url() const = 0;
        virtual void loadHTML(const std::string& iHTMLCode) = 0;

        virtual void setBackgroundColor(const cocos2d::ccColor3B& iColor) = 0;
        virtual void setNavigationFeatures(uint32_t iFeatures) = 0;
    };

NS_CC_EXT_END

#endif /* defined(__CCWebViewImpl__) */
