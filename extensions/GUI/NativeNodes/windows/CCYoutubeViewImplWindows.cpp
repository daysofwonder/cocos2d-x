//
#include "CCYoutubeView.h"
#include "CCYoutubeViewImpl.h"
#include "CCNativeViewImplWindows.h"

#include <atlbase.h> 
#include <atlwin.h>

NS_CC_EXT_BEGIN

	class Win32YoutubeView : public CCNativeViewImplWindows, public CCYoutubeViewImpl
	{
	public:
		virtual void playMovie(const std::string& iMovieID) override {
			fMovieId = iMovieID;
			initWebPage(iMovieID);
		}

	protected:
		virtual void _createUIView() override
		{
			HWND hParent = CCEGLView::sharedOpenGLView()->getHWnd();
			RECT empty;
			empty.left = 0;
			empty.right = 0;
			empty.top = 0;
			empty.bottom = 0;
			HWND child = uiView().Create(hParent, empty, "about:blank", WS_CHILD | WS_VISIBLE);
			if (!child) {
				DWORD error = ::GetLastError();
				assert(error);
			}
		}

	private:
		std::string fMovieId;

		void initWebPage(const std::string& iMovieID)
		{
			CComPtr<IWebBrowser2> pWB2;
			HRESULT hr6 = uiView().QueryControl(&pWB2);
			assert(hr6 == S_OK);

#if 0
			RECT clientRect;
			uiView().GetClientRect(&clientRect);

			int width = clientRect.right - clientRect.left;
			int height = clientRect.bottom - clientRect.top;
#endif

			const std::string youtubeUrlPrefix("http://www.youtube.com/embed/");
			const std::string youtubeUrlPostfix("?autoplay=1&fs=1&hd=1&fmt=22&cc_load_policy=1&cc=1&rel=0");
			const std::string urlStr = youtubeUrlPrefix + iMovieID + youtubeUrlPostfix;

			CComVariant v;  // empty variant
			CComBSTR comUrl(urlStr.c_str());
			HRESULT hr7 = pWB2->Navigate(comUrl, &v, &v, &v, &v);
			assert(hr7 == S_OK);
		}
	};

    std::unique_ptr<CCNativeViewImpl>
    CCYoutubeView::_createImpl()
    {
		return std::unique_ptr<CCNativeViewImpl>(new Win32YoutubeView());
    }

NS_CC_EXT_END

