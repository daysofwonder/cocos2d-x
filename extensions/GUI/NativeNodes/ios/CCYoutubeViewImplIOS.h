//
//  CCYoutubeViewImplIOS.h
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCYoutubeViewImplIOS__
#define __CCYoutubeViewImplIOS__

#include "GUI/NativeNodes/apple/CCNativeViewImplApple.h"
#include "CCYoutubeViewImpl.h"
#include <UIKit/UIKit.h>
#include "DOWFoundation/DOWFoundation.h"

@class CCYoutubeViewImplIOS_WebViewDelegate;

NS_CC_EXT_BEGIN

    class CCYoutubeView;
    
    class CCYoutubeViewImplIOS : public CCNativeViewImplApple, public CCYoutubeViewImpl
    {
    public:
        DOW_DECLARE_HIERARCHY(CCYoutubeViewImplIOS, CCNativeViewImplApple);
        
        CCYoutubeViewImplIOS();
        virtual ~CCYoutubeViewImplIOS();
                                        
        CCYoutubeView* youtubeView() const;
        
        UIWebView* uiWebView() const;
        
        virtual void playMovie(const std::string& iMovieID) override;

        void setPlaying();
        
    private:
        virtual AppleView* _createUIView() override;
        virtual void _updateNativeUIView() override;

        CCYoutubeViewImplIOS_WebViewDelegate* fWebViewDelegate;
        bool fPlaying;
    };

NS_CC_EXT_END

#endif /* defined(__CCYoutubeViewImplIOS__) */
