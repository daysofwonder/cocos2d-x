//
//  CCWebViewImplIOS.cpp
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "GUI/NativeNodes/ios/CCWebViewImplIOS.h"
#include "GUI/NativeNodes/CCWebView.h"
#include "../DOWFoundationImpl/Apple/NSURL+DOWUrl.h"
#include "../DOWFoundationImpl/Apple/NSString+STLExtensions.h"


@interface CCWebViewImplIOS_WebViewDelegate : NSObject<UIWebViewDelegate>
{
@public
    
@private
    cocos2d::extension::CCWebViewImplIOS* fImpl;
    __weak UIWebView*        fWebView;
}

-(id) initWithImpl:(cocos2d::extension::CCWebViewImplIOS*)iImpl webView:(UIWebView*)iWebView;

-(void) detach;

@end

@implementation CCWebViewImplIOS_WebViewDelegate

-(id) initWithImpl:(cocos2d::extension::CCWebViewImplIOS*)iImpl webView:(UIWebView*)iWebView
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

-(void) detach
{
    fImpl = nullptr;
    fWebView.delegate = nullptr;
    fWebView = nullptr;
}

-(BOOL) webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType
{
    if (fImpl != nullptr)
    {
        cocos2d::extension::CCWebView* owner = fImpl->webView();
        if (owner != nullptr)
        {
            cocos2d::extension::CCWebViewDelegate* delegate = owner->delegate();
            if (delegate != nullptr)
            {
                NSURL* nativeURL = request.URL;
                DOW::Url url = [nativeURL dowUrl];
                
                cocos2d::extension::CCWebView::ENavigationType type;
                
                switch (navigationType)
                {
                    case UIWebViewNavigationTypeLinkClicked: type = cocos2d::extension::CCWebView::ENavigationType::eLinkClicked; break;
                    case UIWebViewNavigationTypeFormSubmitted: type = cocos2d::extension::CCWebView::ENavigationType::eFormResubmitted; break;
                    case UIWebViewNavigationTypeBackForward: type = cocos2d::extension::CCWebView::ENavigationType::eBackForward; break;
                    case UIWebViewNavigationTypeReload: type = cocos2d::extension::CCWebView::ENavigationType::eReload; break;
                    case UIWebViewNavigationTypeFormResubmitted: type = cocos2d::extension::CCWebView::ENavigationType::eFormResubmitted; break;
                        
                    default: type = cocos2d::extension::CCWebView::ENavigationType::eOther;
                }
                return delegate->webViewShouldStartLoadWithUrl(owner, url, type) ? YES : NO;
            }                    
        }
    }
    
    return YES;
}

-(void) webViewDidStartLoad:(UIWebView *)webView
{
    if (fImpl != nullptr)
    {
        cocos2d::extension::CCWebView* owner = fImpl->webView();
        if (owner != nullptr)
        {
            cocos2d::extension::CCWebViewDelegate* delegate = owner->delegate();
            if (delegate != nullptr)
            {
                delegate->webViewDidStartLoad(owner);
            }            
        }
    }
}

-(void) webViewDidFinishLoad:(UIWebView *)webView
{
    if (fImpl != nullptr)
    {
        cocos2d::extension::CCWebView* owner = fImpl->webView();
        if (owner != nullptr)
        {
            cocos2d::extension::CCWebViewDelegate* delegate = owner->delegate();
            if (delegate != nullptr)
            {
                delegate->webViewDidFinishLoad(owner);
            }            
        }
    }
}

-(void) webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
    if (fImpl != nullptr)
    {
        cocos2d::extension::CCWebView* owner = fImpl->webView();
        if (owner != nullptr)
        {
            cocos2d::extension::CCWebViewDelegate* delegate = owner->delegate();
            if (delegate != nullptr)
            {
                NSString* msg = error.localizedDescription;
                DOW::Exception exception([msg stdString]);
                
                delegate->webViewDidFailLoadWithException(owner, exception);
            }                    
        }
    }
}

@end

NS_CC_EXT_BEGIN

    std::unique_ptr<CCNativeViewImpl>
    CCWebView::_createImpl()
    {
        std::unique_ptr<CCNativeViewImpl> impl(new CCWebViewImplIOS);
        return impl;
    }
    
    CCWebViewImplIOS::CCWebViewImplIOS()
    : fWebViewDelegate(nil)
    {
    }
    
    CCWebViewImplIOS::~CCWebViewImplIOS()
    {
        auto* view = uiWebView();
        view.delegate = nil;
        [fWebViewDelegate detach];
        [fWebViewDelegate release];
    }
    
    AppleView*
    CCWebViewImplIOS::_createUIView()
    {
        CGRect r = CGRectMake(0, 0, 100, 100);
        UIWebView* webView = [[UIWebView alloc] initWithFrame:r];
        fWebViewDelegate = [[CCWebViewImplIOS_WebViewDelegate alloc] initWithImpl:this webView:webView];
        return webView;
    }
    
    UIWebView*
    CCWebViewImplIOS::uiWebView() const
    {
        AppleView* view = uiView();
        assert([view isKindOfClass:[UIWebView class]]);
        return (UIWebView*) view;
    }

    CCWebView*
    CCWebViewImplIOS::webView() const
    {
        return static_cast<CCWebView*>(owner());
    }
    
    void
    CCWebViewImplIOS::loadUrl(const Url& iUrl, bool iForceReload)
    {
        NSURL* url = [NSURL urlWithDowUrl:iUrl];
        NSURLRequest* request = iForceReload ? [NSURLRequest requestWithURL:url cachePolicy:NSURLRequestReloadIgnoringCacheData timeoutInterval:0] : [NSURLRequest requestWithURL:url];
        
        [uiWebView() loadRequest:request];
    }
    
    void
    CCWebViewImplIOS::loadHTML(const std::string &iHTMLCode)
    {
        [uiWebView() loadHTMLString:[NSString stringWithStdString:iHTMLCode] baseURL:nil];
    }
    
    Url
    CCWebViewImplIOS::url() const
    {
        NSURLRequest* request = uiWebView().request;
        NSURL* url = request.URL;
        return [NSURL dowUrlWithURL:url];
    }
    
    void
    CCWebViewImplIOS::setBackgroundColor(const ccColor3B& iColor)
    {
        UIScrollView* scrollView = uiWebView().scrollView;
        UIColor* color = [UIColor colorWithRed:CGFloat(iColor.r) / 255.f green:CGFloat(iColor.g) / 255.f blue:CGFloat(iColor.b) / 255.f alpha:1.f];
        scrollView.backgroundColor = color;
    }

    void
    CCWebViewImplIOS::setNavigationFeatures(uint32_t iFeatures)
    {
        UIScrollView* scrollView = uiWebView().scrollView;
        scrollView.scrollEnabled = ((iFeatures & CCWebView::fScrollingEnabled) != 0);
        scrollView.bounces = ((iFeatures & CCWebView::fBouncingEnabled) != 0);
    }

NS_CC_EXT_END
