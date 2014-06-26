
#include "CCWebViewImplWindows.h"

#include "FunctionObject.h"
#include "DOWFoundation/Logger.h"


#include "CCEGLView.h"
#include <string>
#include <cvt/wstring>
#include <codecvt>

#include <exdispid.h>
#include <exdisp.h>

// to make sure we do not log wchar because we could not convert them (because to sink to call imbue on)
#define BOOST_NO_INTRINSIC_WCHAR_T
#define BOOST_NO_CXX11_CHAR16_T
#define BOOST_NO_CXX11_CHAR32_T
#include <boost/log/trivial.hpp>

NS_CC_EXT_BEGIN

	Win32WebView::Win32WebView()
	{
		fOutsideNavigation = false;
		fAdviseCookie = 0;
		BOOST_LOG_TRIVIAL(debug) << "Win32WebView::Win32WebView " << std::endl;

	}

	Win32WebView::~Win32WebView()
	{
		_unhookSink();
		BOOST_LOG_TRIVIAL(debug) << "Win32WebView::~Win32WebView " << std::endl;
	}

	bool Win32WebView::init(CCNode* iNode) {
		bool result = CCNativeViewImplWindows::init(iNode);
		if (result) {
			fWebView = dynamic_cast<CCWebView *>(iNode);
			assert(fWebView);
		}
		return result;
	}


	void Win32WebView::_createUIView()
	{
		if (uiView().IsWindow()) {
			return; // only create once, lazy creation needs this kind of protection
		}
		const bool scrolling = fFeatures & CCWebView::FNavigationFeatures::fScrollingEnabled;

		HWND hParent = CCEGLView::sharedOpenGLView()->getHWnd();
		RECT empty;
		empty.left = 0;
		empty.right = 0;
		empty.top = 0;
		empty.bottom = 0;

		DWORD style = WS_CHILD /*| WS_VISIBLE*/;
		if (scrolling) {
			style |= WS_VSCROLL;
			//style |= WS_HSCROLL;
		}

		BOOST_LOG_TRIVIAL(debug) << "Win32WebView::_createUIView:  about:blank";

		HWND child = uiView().Create(hParent, empty, "about:blank", style);
		if (!child) {
			DWORD error = ::GetLastError();
			assert(error);
		}
		_hookSink();
		if (!fHtml.empty()) {
			loadHTML(fHtml);
		}
	}

	void Win32WebView::loadUrl(const Url& iUrl, bool iForceReload)
	{
		fHtml.clear(); // cannot be both
		if (!uiView().IsWindow()) {
			_createUIView(); //lazy creation
		}

		fUrl = iUrl;
		const std::string urlStr = iUrl.urlAsString();
		BOOST_LOG_TRIVIAL(debug) << "Win32WebView::loadUrl:  " << urlStr << std::endl;

		assert(uiView().IsWindow());

		CComPtr<IWebBrowser2> pWB2;
		HRESULT hr6 = uiView().QueryControl(&pWB2);
		assert(hr6 == S_OK);

		setOutsideNavigation();

		CComVariant v;  // empty variant
		CComBSTR comUrl(urlStr.c_str());
		HRESULT hr7 = pWB2->Navigate(comUrl, &v, &v, &v, &v);
		assert(hr7 == S_OK);
	}

	Url Win32WebView::url() const { 
		return fUrl;
	}

	void Win32WebView::loadHTML(const std::string& iHTMLCode) {
		if (uiView().IsWindow()) {
			CComPtr<IWebBrowser2> pWebBrowser;
			HRESULT hr1 = uiView().QueryControl(&pWebBrowser);
			assert(hr1 == S_OK);

			CComPtr<IDispatch> pHtmlDoc;
			HRESULT hr2 = pWebBrowser->get_Document(&pHtmlDoc);
			assert(hr2 == S_OK);

			CComQIPtr<IPersistStreamInit> pPersistStreamInit = pHtmlDoc;
			assert(pPersistStreamInit);
			HRESULT hr3 = pPersistStreamInit->InitNew();
			assert(hr3 == S_OK);

			CComPtr<IStream> pStream(SHCreateMemStream(reinterpret_cast<const BYTE *>(iHTMLCode.data()), iHTMLCode.size()));
			HRESULT hr4 = pPersistStreamInit->Load(pStream);
			assert(hr4 == S_OK);

			BOOST_LOG_TRIVIAL(debug) << "Win32WebView::loadHTML:  " << std::dec << iHTMLCode.size() << " bytes" << std::endl;
		}
		else {
			fHtml = iHTMLCode;
		}
	}

	void Win32WebView::setBackgroundColor(const ccColor3B& iColor) {
		fColor = iColor;
		if (uiView().IsWindow()) {
			uiView().m_hWnd;
		}
	}

	void Win32WebView::setNavigationFeatures(uint32_t iFeatures) {
		fFeatures = iFeatures;
	}



	void Win32WebView::_hookSink()
	{
		CComPtr<IConnectionPointContainer> pConnectionPointContainer;
		HRESULT hr2 = uiView().QueryControl(&pConnectionPointContainer);
		assert(hr2 == S_OK);

		HRESULT hr3 = pConnectionPointContainer->FindConnectionPoint(DIID_DWebBrowserEvents2, &fConnectionPoint);
		assert(hr3 == S_OK);

		fSink = CFunctionObject<Win32WebView>::CreateEventFunctionObject([this](DISPID id, DISPPARAMS* pDispParams, VARIANT* pVarResult){
			return onSink(id, pDispParams, pVarResult);
		});

		HRESULT hr4 = fConnectionPoint->Advise(fSink, &fAdviseCookie);
		assert(hr4 == S_OK);
	}

	void Win32WebView::_unhookSink()
	{
		if (fConnectionPoint) {
			HRESULT hr4 = fConnectionPoint->Unadvise(fAdviseCookie);
			assert(hr4 == S_OK);
			fConnectionPoint = nullptr;
			fSink = nullptr;
			fAdviseCookie = 0;
		}
	}

	HRESULT Win32WebView::onSink(DISPID idDispMember, DISPPARAMS* pDispParams, VARIANT* pVarResult)
	{
		if (!pDispParams) {
			return DISP_E_PARAMNOTOPTIONAL;
		}
		VARIANTARG *args = pDispParams->rgvarg;

		switch (idDispMember) {
		case DISPID_DOWNLOADBEGIN:
			BOOST_LOG_TRIVIAL(debug) << "Win32WebView::onSink: DISPID_DOWNLOADBEGIN";
			if (hasDelegate() /*&& fUrl.isValid()*/) {
				delegate().webViewDidStartLoad(fWebView);
			}
			break;

		case DISPID_PROGRESSCHANGE:
		{
			assert(pDispParams->cArgs == 2);
			LONG Progress = args[0].lVal;
			LONG ProgressMax = args[1].lVal;
			BOOST_LOG_TRIVIAL(debug) << "Win32WebView::onSink: DISPID_PROGRESSCHANGE: " << Progress << "/" << ProgressMax;
		}
			break;

		case DISPID_TITLECHANGE:
		{
			assert(pDispParams->cArgs == 1);
			BSTR title = args[0].bstrVal;
			BOOST_LOG_TRIVIAL(debug) << "Win32WebView::onSink: DISPID_TITLECHANGE: " << const_cast<const wchar_t *>(title);
		}
			break;

		case DISPID_STATUSTEXTCHANGE:
		{
			assert(pDispParams->cArgs == 1);
			//BSTR status = args[0].bstrVal;
			std::wstring status(args[0].bstrVal);
			BOOST_LOG_TRIVIAL(debug) << "Win32WebView::onSink: DISPID_STATUSTEXTCHANGE: " << status;
		}
			break;

		case DISPID_DOWNLOADCOMPLETE:
			BOOST_LOG_TRIVIAL(debug) << "Win32WebView::onSink: DISPID_DOWNLOADCOMPLETE";
			break;

		case DISPID_DOCUMENTCOMPLETE: 
		{
			assert(pDispParams->cArgs >= 1);
			BSTR url = args[0].pvarVal->bstrVal;
			const std::wstring aboutBlank(L"about:blank");
			const std::wstring wurl(url);
			BOOST_LOG_TRIVIAL(debug) << "Win32WebView::onSink: DISPID_DOCUMENTCOMPLETE " << wurl << std::endl;
			if (hasDelegate() && (wurl != aboutBlank || !fHtml.empty()) ) {

				std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
				std::string uUrl = converter.to_bytes(url);
				Url newUrl;
				if (newUrl.createFromString(uUrl) || newUrl.createFromSystemPath(uUrl)) {
					// Important, we moved our URL, so we need this info to update status on "other" webpage that must be in sync with this one, via webViewDidFinishLoad
					fUrl = newUrl;
				}
				else {
					BOOST_LOG_TRIVIAL(debug) << "Win32WebView::onSink: DISPID_DOCUMENTCOMPLETE Cannot parse URL: " << wurl << std::endl;
				}
				delegate().webViewDidFinishLoad(fWebView);
			}
		}
			break;

		case DISPID_NAVIGATEERROR:
			BOOST_LOG_TRIVIAL(debug) << "Win32WebView::onSink: DISPID_NAVIGATEERROR " << std::endl;
			{
				//Extract the status code from the DISPPARAMS structure
				VARIANT * vt_statuscode = pDispParams->rgvarg[1].pvarVal;
				DWORD  dwStatusCode = vt_statuscode->lVal;
				//Extract the event's IDispatch pointer
				IDispatch *pdispFiredEvent = pDispParams->rgvarg[4].pdispVal;
				if (hasDelegate()) {
					DOW::Exception myException("DISPID_NAVIGATEERROR");
					delegate().webViewDidFailLoadWithException(fWebView, myException);
				}
								 }
			break;

		case DISPID_BEFORENAVIGATE2:
			BOOST_LOG_TRIVIAL(debug) << "Win32WebView::onSink: DISPID_BEFORENAVIGATE2 " << std::endl;
			{
				// The parameters for this DISPID are as follows:
				// [0]: Cancel flag  - VT_BYREF|VT_BOOL
				// [1]: HTTP headers - VT_BYREF|VT_VARIANT
				// [2]: Address of HTTP POST data  - VT_BYREF|VT_VARIANT
				// [3]: Target frame name - VT_BYREF|VT_VARIANT
				// [4]: Option flags - VT_BYREF|VT_VARIANT
				// [5]: URL to navigate to - VT_BYREF|VT_VARIANT
				// [6]: An object that evaluates to the top-level or frame
				//      WebBrowser object corresponding to the event.
				CComPtr<IDispatch> window_disp = args[6].pdispVal;
				CComPtr< IWebBrowser2 > browser;
				window_disp.QueryInterface(&browser);
				assert(browser);
				BSTR url = args[5].pvarVal->bstrVal;
				const uint32_t flags = args[4].lVal;
				BSTR target = args[3].pvarVal->bstrVal;

				if (flags & 0x01) {
					BOOST_LOG_TRIVIAL(debug) << "beforeNavigateExternalFrameTarget" << std::endl;
				}
				if (target) {
					BOOST_LOG_TRIVIAL(debug) << "has target: " << target << std::endl;
				}
				BOOST_LOG_TRIVIAL(debug) << "BrowserListener:DISPID_BEFORENAVIGATE2:Url: " << url << std::endl;

				if (fOutsideNavigation) {
					fOutsideNavigation = false;
					return S_OK;
				}

				if (hasDelegate()) {
					std::wstring wUrl(url);
					Url dowUrl;

					const bool is1 = PathIsURLW(url);
					const bool is2 = PathIsUNCW(url);
					const bool is3 = UrlIsFileUrlW(url);
					const bool is4 = PathFileExistsW(url);
					if (is4) {
						DWORD size = MAX_PATH;
						WCHAR buf[MAX_PATH];
						HRESULT res = UrlCreateFromPathW(url, buf, &size, 0);
						assert(res == S_OK);
						wUrl = buf;
					}


					std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
					std::string uUrl = converter.to_bytes(wUrl);
					dowUrl.createFromString(uUrl);

					const bool canContinue = delegate().webViewShouldStartLoadWithUrl(fWebView, dowUrl, CCWebView::ENavigationType::eLinkClicked);
					if (!canContinue) {
						//browser->Stop();
						*(((*pDispParams).rgvarg)[0].pboolVal) = TRUE;
					}
				}
				break;
			 }

		case DISPID_NAVIGATECOMPLETE2:
		{
			assert(pDispParams->cArgs >= 1);
			BSTR url = args[0].pvarVal->bstrVal;
			BOOST_LOG_TRIVIAL(debug) << "Win32WebView::onSink: DISPID_NAVIGATECOMPLETE2: " << const_cast<const wchar_t *>(url);
		}
			break;

		default:
			BOOST_LOG_TRIVIAL(debug) << "Win32WebView::onSink: default " << std::dec << idDispMember << std::endl;
		}
		return S_OK;
	}


	std::unique_ptr<CCNativeViewImpl>
		CCWebView::_createImpl()
	{
		return std::unique_ptr<CCNativeViewImpl>(new Win32WebView());
		}

NS_CC_EXT_END

