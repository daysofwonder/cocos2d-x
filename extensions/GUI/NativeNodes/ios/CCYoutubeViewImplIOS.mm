//
//  CCYoutubeViewImplIOS.cpp
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCYoutubeViewImplIOS.h"
#include "CCYoutubeView.h"
#include "../DOWFoundationImpl/Apple/NSURL+DOWUrl.h"
#include "../DOWFoundationImpl/Apple/NSString+STLExtensions.h"


@interface CCYoutubeViewImplIOS_WebViewDelegate : NSObject<UIWebViewDelegate>
{
@public
    
@private
    cocos2d::extension::CCYoutubeViewImplIOS* fImpl;
    __weak UIWebView*           fWebView;
}

-(id) initWithImpl:(cocos2d::extension::CCYoutubeViewImplIOS*)iImpl webView:(UIWebView*)iWebView;

-(void) detach;

@end

@implementation CCYoutubeViewImplIOS_WebViewDelegate

-(id) initWithImpl:(cocos2d::extension::CCYoutubeViewImplIOS*)iImpl webView:(UIWebView*)iWebView
{
    if (self = [self init])
    {
        // Weak
        fImpl = iImpl;
        fWebView = iWebView;
        fWebView.delegate = self;
    }
    
    return self;
}

-(void) dealloc
{
   fWebView.delegate = nil;
    [super dealloc];
}

-(void) webViewDidFinishLoad:(UIWebView *)webView
{
    if (fImpl != nullptr)
    {
        fImpl->setPlaying();        
    }
}

-(void) detach
{
    fImpl = nullptr;
    fWebView.delegate = nil;
    fWebView = nil;
}

@end

NS_CC_EXT_BEGIN

    std::unique_ptr<CCNativeViewImpl>
    CCYoutubeView::_createImpl()
    {
        std::unique_ptr<CCNativeViewImpl> impl(new CCYoutubeViewImplIOS);
        return impl;
    }
    
    CCYoutubeViewImplIOS::CCYoutubeViewImplIOS()
    : fWebViewDelegate(nil), fPlaying(false)
    {
    }
    
    CCYoutubeViewImplIOS::~CCYoutubeViewImplIOS()
    {
        uiWebView().delegate = nil;
        [fWebViewDelegate detach];
        [fWebViewDelegate release];
    }
    
    AppleView*
    CCYoutubeViewImplIOS::_createUIView()
    {
        CGRect r = CGRectMake(0, 0, 100, 100);
        UIWebView* webView = [[UIWebView alloc] initWithFrame:r];
        webView.backgroundColor = [UIColor clearColor];
        webView.scrollView.backgroundColor = [UIColor clearColor];
        fWebViewDelegate = [[CCYoutubeViewImplIOS_WebViewDelegate alloc] initWithImpl:this webView:webView];
        return webView;
    }
    
    void
    CCYoutubeViewImplIOS::_updateNativeUIView()
    {
        if (fPlaying)
        {
            SuperClass::_updateNativeUIView();
        }
        else
        {
            uiWebView().hidden = YES;
        }
    }
    
    UIWebView*
    CCYoutubeViewImplIOS::uiWebView() const
    {
        UIView* view = uiView();
        assert([view isKindOfClass:[UIWebView class]]);
        return (UIWebView*) view;
    }
    
    CCYoutubeView*
    CCYoutubeViewImplIOS::youtubeView() const
    {
        return static_cast<CCYoutubeView*>(owner());
    }
    
    void
    CCYoutubeViewImplIOS::playMovie(const std::string& iMovieID)
    {
        CCNode* view = owner();
        const CCSize& size = view->getContentSize();
        
        NSString* templatePath = [[NSBundle mainBundle] pathForResource:@"UIWebViewYouTubePlaybackTemplate.html" ofType:nil];
        
        NSStringEncoding enc;
        NSError* err = nil;
        NSString* tmpl = [NSString stringWithContentsOfFile:templatePath usedEncoding:&enc error:&err];
        
        int width = size.width;
        int height = size.height;
        
        NSString* ID = [NSString stringWithStdString:iMovieID];
        NSString* html = [NSString stringWithFormat:tmpl, width, height, ID];
        
        UIWebView* webView = uiWebView();
        [webView loadHTMLString:html baseURL:nil];
    }
    
    void
    CCYoutubeViewImplIOS::setPlaying()
    {
        fPlaying = true;
    }

NS_CC_EXT_END
