//
#include "GUI/NativeNodes/CCYoutubeView.h"
#include "GUI/NativeNodes/CCYoutubeViewImpl.h"
#include "GUI/NativeNodes/apple/CCNativeViewImplApple.h"

#include <WebKit/WebKit.h>


NS_CC_EXT_BEGIN
    
	class OSXYoutubeView : public CCNativeViewImplApple, public CCYoutubeViewImpl
	{
	public:
		virtual void playMovie(const std::string& iMovieID) override {
        fMovieId = iMovieID;
        initWebPage(iMovieID);
    }
    
    protected:
        virtual NSView* _createUIView() override {    
            return [[WebView alloc] initWithFrame: NSMakeRect(0,0,100,100) frameName:nil groupName:nil];
        }

    private:
        std::string fMovieId;

        void initWebPage(const std::string& iMovieID) {
            const std::string youtubeUrlPrefix("http://www.youtube.com/embed/");
            const std::string youtubeUrlPostfix("?autoplay=1&fs=1&hd=1&fmt=22&cc_load_policy=1&cc=1&rel=0");
            const std::string urlStr = youtubeUrlPrefix + iMovieID + youtubeUrlPostfix;
            
            NSURL* url = [NSURL URLWithString: [NSString stringWithUTF8String: urlStr.c_str()]];
            NSURLRequest* request =  [NSURLRequest requestWithURL:url];
            [[(WebView *)uiView() mainFrame] loadRequest:request];
        }
    };

    std::unique_ptr<CCNativeViewImpl>
    CCYoutubeView::_createImpl()
    {
        return std::unique_ptr<CCNativeViewImpl>(new OSXYoutubeView());
    }

NS_CC_EXT_END


