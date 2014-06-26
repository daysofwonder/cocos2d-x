//
#include "GUI/NativeNodes/CCYoutubeViewImpl.h"
#include "CCNativeViewImplLinux.h"
#include <webkit/webkit.h>

NS_CC_EXT_BEGIN

    class YoutubeLinux : public CCNativeViewImplLinux , public CCYoutubeViewImpl {
    public:
        YoutubeLinux();
        virtual ~YoutubeLinux();

        virtual void playMovie(const std::string& iMovieID) override final;
    protected:
        virtual void onEnterTransitionDidFinish() override final;
        virtual void onExitTransitionDidStart() override final;

    private:
        WebKitWebView* fWebKitView;
    };

NS_CC_EXT_END


