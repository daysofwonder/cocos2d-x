//
//  CCWebViewImplOSX.cpp
//  SmallWorld2
//
//  Created by Gérald Guyomard on 05/03/13.
//
//

#include "GUI/NativeNodes/apple/CCWebViewImplOSX.h"
#include "GUI/NativeNodes/CCWebView.h"
#include "../DOWFoundationImpl/Apple/NSURL+DOWUrl.h"
#include "../DOWFoundationImpl/Apple/NSString+STLExtensions.h"

@interface WebViewDelegate : NSObject {
    @private
    cocos2d::extension::CCWebViewImplOSX* fImpl;
    __weak WebView*        fWebView;
}
-(id) initWithImpl:(cocos2d::extension::CCWebViewImplOSX*)iImpl webView:(WebView*)iWebView;
-(void) detach;
@end


@implementation WebViewDelegate

-(id) initWithImpl:(cocos2d::extension::CCWebViewImplOSX*)iImpl webView:(WebView*)iWebView
{
    if (self = [self init])
    {
        // Weak
        fImpl = iImpl;
        fWebView = iWebView;
        [fWebView setFrameLoadDelegate:self];
        [fWebView setPolicyDelegate: self];
    }
    
    return self;
}

-(void) dealloc
{
    [fWebView setFrameLoadDelegate:nil];
    [fWebView setPolicyDelegate:nil];
    [super dealloc];
}

-(void) detach
{
    fImpl = nullptr;
    [fWebView setPolicyDelegate:nil];
    [fWebView setFrameLoadDelegate:nil];
    fWebView = nullptr;
}



- (void)webView:(WebView *)webView decidePolicyForNavigationAction:(NSDictionary *)actionInformation request:(NSURLRequest *)request frame:(WebFrame *)frame decisionListener:(id < WebPolicyDecisionListener >)listener
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
                NSNumber* actionType = [actionInformation objectForKey: WebActionNavigationTypeKey];
                switch([actionType intValue]) {
                    case  WebNavigationTypeLinkClicked:
                        type = cocos2d::extension::CCWebView::ENavigationType::eLinkClicked;
                        break;
                    case  WebNavigationTypeFormSubmitted:
                        type = cocos2d::extension::CCWebView::ENavigationType::eFormSubmitted;
                        break;
                    case  WebNavigationTypeBackForward:
                        type = cocos2d::extension::CCWebView::ENavigationType::eBackForward;
                        break;
                    case  WebNavigationTypeReload:
                        type = cocos2d::extension::CCWebView::ENavigationType::eReload;
                        break;
                    case  WebNavigationTypeFormResubmitted:
                        type = cocos2d::extension::CCWebView::ENavigationType::eFormResubmitted;
                        break;
                    case  WebNavigationTypeOther:
                        type = cocos2d::extension::CCWebView::ENavigationType::eOther;
                        break;
                }
                bool shouldStart = delegate->webViewShouldStartLoadWithUrl(owner, url, type);
                if (shouldStart) {
                    [listener use];
                } else {
                    [listener ignore];
                }
            }
        }
    }
    
}

-(void) webView:(WebView *)webView didStartProvisionalLoadForFrame:(WebFrame *)frame
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


-(void) webView:(WebView *)webView didFinishLoadForFrame:(WebFrame *)frame
{
    
    //DOMDocument *document = [frame DOMDocument];
    //DOMHTMLElement *element = (DOMHTMLElement *)[document documentElement];
    //NSString* html = [element outerHTML];
    //NSLog(@"document: %@",html);
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


-(void) webView:(WebView *)webView didFailLoadWithError:(NSError *)error forFrame:(WebFrame *)frame
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

- (void)webView:(WebView *)sender didFailProvisionalLoadWithError:(NSError *)error forFrame:(WebFrame *)frame
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
        std::unique_ptr<CCNativeViewImpl> impl(new CCWebViewImplOSX);
        return impl;
    }
    
    CCWebViewImplOSX::CCWebViewImplOSX()
    : fWebViewDelegate(nil)
    {
    }
    
    CCWebViewImplOSX::~CCWebViewImplOSX()
    {
        WebView* view = uiWebView();
        [view setFrameLoadDelegate:nil];
        [fWebViewDelegate detach];
        [fWebViewDelegate release];
    }
    
    NSView*
    CCWebViewImplOSX::_createUIView()
    {
        NSRect frameRect;
        frameRect.origin.x=0;
        frameRect.origin.y=0;
        frameRect.size.width=50;
        frameRect.size.height=50;
        WebView* webView = [[WebView alloc] initWithFrame:frameRect frameName:nil groupName:nil];
        fWebViewDelegate = [[WebViewDelegate alloc] initWithImpl:this webView:webView];
        return webView;
    }
    
    WebView*
    CCWebViewImplOSX::uiWebView() const
    {
        AppleView* view = uiView();
        assert([view isKindOfClass:[WebView class]]);
        return (WebView*) view;
    }

    CCWebView*
    CCWebViewImplOSX::webView() const
    {
        return static_cast<CCWebView*>(owner());
    }
    
    void
    CCWebViewImplOSX::loadUrl(const Url& iUrl, bool iForceReload)
    {
        NSURL* url = [NSURL urlWithDowUrl:iUrl];
        NSURLRequest* request = iForceReload ? [NSURLRequest requestWithURL:url cachePolicy:NSURLRequestReloadIgnoringCacheData timeoutInterval:0] : [NSURLRequest requestWithURL:url];
        
        [[uiWebView() mainFrame] loadRequest:request];
    }
    
    void
    CCWebViewImplOSX::loadHTML(const std::string &iHTMLCode)
    {
        [[uiWebView() mainFrame] loadHTMLString:[NSString stringWithStdString:iHTMLCode] baseURL:nil];
    }
    
    Url
    CCWebViewImplOSX::url() const
    {
        NSString* url = [uiWebView() mainFrameURL];
        return [NSURL dowUrlWithURL:[NSURL URLWithString:url]];
    }
    
    void
    CCWebViewImplOSX::setBackgroundColor(const ccColor3B& iColor)
    {
#ifndef CC_TARGET_OS_MAC
        UIScrollView* scrollView = uiWebView().scrollView;
        UIColor* color = [UIColor colorWithRed:CGFloat(iColor.r) / 255.f green:CGFloat(iColor.g) / 255.f blue:CGFloat(iColor.b) / 255.f alpha:1.f];
        scrollView.backgroundColor = color;
#endif
    }

    void
    CCWebViewImplOSX::setNavigationFeatures(uint32_t iFeatures)
    {
#ifndef CC_TARGET_OS_MAC
        UIScrollView* scrollView = uiWebView().scrollView;
        scrollView.scrollEnabled = ((iFeatures & CCWebView::fScrollingEnabled) != 0);
        scrollView.bounces = ((iFeatures & CCWebView::fBouncingEnabled) != 0);
#endif
    }

NS_CC_EXT_END



