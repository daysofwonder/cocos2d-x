//
#ifndef __CCWEBVIEW_IMPL_WINDOWS_H__
#define __CCWEBVIEW_IMPL_WINDOWS_H__

#include "GUI/NativeNodes/CCWebViewImpl.h"
#include "GUI/NativeNodes/windows/CCNativeViewImplWindows.h"
#include "GUI/NativeNodes/CCWebView.h"

#include <string>

#include <atlbase.h> 
#include <atlwin.h>

NS_CC_EXT_BEGIN

	class Win32WebView : public CCNativeViewImplWindows, public CCWebViewImpl
	{
	public:
		Win32WebView();
		virtual ~Win32WebView();

		virtual bool init(CCNode* iNode);

		virtual void loadUrl(const Url& iUrl, bool iForceReload);

		virtual Url url() const;
		virtual void loadHTML(const std::string& iHTMLCode);

		virtual void setBackgroundColor(const ccColor3B& iColor);
		virtual void setNavigationFeatures(uint32_t iFeatures);

	protected:
		virtual void _createUIView() override;

	private:
		CCWebView* fWebView;
		CCWebViewDelegate* fDelegate;
		Url fUrl;
		CComPtr<IDispatch> fSink;
		bool fOutsideNavigation;
		uint32_t fFeatures;
		ccColor3B fColor;
		std::string fHtml;
		CComPtr<IConnectionPoint> fConnectionPoint;
		DWORD fAdviseCookie;

		CCWebViewDelegate& delegate() {return *fWebView->delegate();}
		bool hasDelegate() const {return fWebView->delegate() != nullptr;}
		void setOutsideNavigation() {fOutsideNavigation = true;}

		void _hookSink();
		void _unhookSink();
		HRESULT onSink(DISPID id, DISPPARAMS* pDispParams, VARIANT* pVarResult);
	};

NS_CC_EXT_END

#endif
