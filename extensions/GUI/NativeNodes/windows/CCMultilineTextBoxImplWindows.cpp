
#include "GUI/NativeNodes/CCMultilineTextBox.h"
#include "GUI/NativeNodes/CCMultilineTextBoxImpl.h"
#include "layers_scenes_transitions_nodes/CCNativeViewImpl.h"
#include "GUI/NativeNodes/windows/CCNativeViewImplWindows.h"
#include "FunctionObject.h"
#include "DOWFoundation/Logger.h"

#include <cvt/wstring>
#include <codecvt>

#include <atlbase.h> 
#include <atlwin.h>

#include <comutil.h>
#include <comdef.h>

#include <scrptids.h>

#include <mshtmdid.h>

NS_CC_EXT_BEGIN
    
	class CCMultilineTextBoxImplWindows : public CCNativeViewImplWindows, public CCMultilineTextBoxImpl
	{
	public:
		CCMultilineTextBoxImplWindows() 
		{
			fTextColor.r = 0;
			fTextColor.g = 0;
			fTextColor.b = 0;
			fTextColor.a = 255;
			fBackgroundColor.r = 0;
			fBackgroundColor.g = 0;
			fBackgroundColor.b = 0;
			fBackgroundColor.a = 0;
		}

		virtual ~CCMultilineTextBoxImplWindows() 
		{
			unhookCallbacks();
		}

		virtual bool init(CCNode* iNode) override
		{
			bool result = CCNativeViewImplWindows::init(iNode);
			if (result) {
				fMultilineTextBox = dynamic_cast<CCMultilineTextBox*>(iNode);
				if (!fMultilineTextBox) {
					return false;
				}
			}
			return result;
		}

		virtual void _createUIView() override
		{
			HWND hParent = CCEGLView::sharedOpenGLView()->getHWnd();
			RECT emptyRect;
			emptyRect.top = 0;
			emptyRect.left = 0;
			emptyRect.bottom = 0;
			emptyRect.right = 0;
			std::stringstream ss;
			ss << "mshtml:";
			ss << "<HTML><BODY style='margin: 0; padding: 0;'>";
			ss << "<TEXTAREA style='width: 100%; height: 100% ; border: none;'></TEXTAREA>";
			ss << "</BODY></HTML>";
			const std ::string html(ss.str());
			HWND child = uiView().Create(hParent, emptyRect, html.c_str() , WS_CHILD);
			if (!child) {
				DWORD error = ::GetLastError();
				assert(error);
			}

			CComPtr<IConnectionPointContainer> pConnectionPointContainer;
			HRESULT hr2 = uiView().QueryControl(&pConnectionPointContainer);
			assert(hr2 == S_OK);

			HRESULT hr3 = pConnectionPointContainer->FindConnectionPoint(DIID_HTMLDocumentEvents2, &fConnectionPoint);
			assert(hr3 == S_OK);

			fOnLoad = CFunctionObject<CCMultilineTextBoxImplWindows>::CreateEventFunctionObject([this](DISPID id, DISPPARAMS* pDispParams, VARIANT* pVarResult){
				if (id == DISPID_HTMLDOCUMENTEVENTS_ONACTIVATE) {
					assert(pDispParams);
					CComPtr<IDispatch> spEventObj = pDispParams->rgvarg[0].pdispVal;
					assert(spEventObj);
					CComQIPtr<IHTMLEventObj> pHtmlEvent = spEventObj;
					assert(pHtmlEvent);
					CComPtr<IHTMLElement> source;
					HRESULT hr1 = pHtmlEvent->get_srcElement(&source);
					assert(hr1 == S_OK);

					BSTR tagName;
					HRESULT hr2 = source->get_tagName(&tagName);
					assert(hr2 == S_OK);

					const std::wstring textAreaStr(L"TEXTAREA");
					dowlog::myLog() << "onactivate: " << tagName << std::endl;

					if (textAreaStr == tagName) {
						updateAll();
						hookCallbacks();

						HRESULT hr3 = fConnectionPoint->Unadvise(fCookie);
						assert(hr3 == S_OK);
					}
				}
				return S_OK;
			});

			fCookie = 0;
			HRESULT hr4 = fConnectionPoint->Advise(fOnLoad, &fCookie);
			assert(hr4 == S_OK);
			assert(fCookie);
		}

		HRESULT onKeyPress(DISPID id, DISPPARAMS* pDispParams, VARIANT* pVarResult)
		{
			assert(id == 0);
			assert(pDispParams);
			CComPtr<IDispatch> spEventObj = pDispParams->rgvarg[0].pdispVal;
			assert(spEventObj);
			CComQIPtr<IHTMLEventObj> pHtmlEvent = spEventObj;
			assert(pHtmlEvent);
			long keyCode = 0;
			HRESULT hr1 = pHtmlEvent->get_keyCode(&keyCode);
			assert(hr1 == S_OK);
			dowlog::myLog() << "onKeyPress: " << keyCode << std::endl;

			CCMultilineTextBoxDelegate* myDelegate = delegate();
			if (myDelegate) {
				if (keyCode == VK_ESCAPE) {
					myDelegate->textBoxEditingDidEnd(fMultilineTextBox);
				}
			}
			return S_OK;
		}

        HRESULT onChange(DISPID id, DISPPARAMS* pDispParams, VARIANT* pVarResult)
        {
            dowlog::myLog() << "onChange " << std::endl;
            CCMultilineTextBoxDelegate* myDelegate = delegate();
            if (myDelegate) {
                std::string text = getText();
                myDelegate->textBoxTextChanged(fMultilineTextBox, text);
            }
            return S_OK;
        }

		HRESULT onBlur(DISPID id, DISPPARAMS* pDispParams, VARIANT* pVarResult)
		{
			dowlog::myLog() << "onBlur fHasFocus: " << std::endl;
			CCMultilineTextBoxDelegate* myDelegate = delegate();
			if (myDelegate) {
				myDelegate->textBoxEditingDidEnd(fMultilineTextBox);
			}
			return S_OK;
		}

		HRESULT onFocus(DISPID id, DISPPARAMS* pDispParams, VARIANT* pVarResult)
		{
			dowlog::myLog() << "onFocus fHasFocus: "  << std::endl;
			CCMultilineTextBoxDelegate* myDelegate = delegate();
			if (myDelegate) {
				myDelegate->textBoxEditingDidBegin(fMultilineTextBox);
			}
			return S_OK;
		}

		virtual std::string getText() const override
		{ 
			std::string result;
			CComPtr<IHTMLTextAreaElement> textArea = getTextArea(uiView());
			if (textArea) {
				result = getText(textArea);
			}
			return result;
		}

		virtual void setText(const std::string& iText) override 
		{
			fText = iText;
			CComPtr<IHTMLTextAreaElement> textArea = getTextArea(uiView());
			if (textArea) {
				setText(textArea, iText);
			}
		}

		virtual void setFont(const std::string& pName, float iSize) override 
		{
			fFontName = pName;
			fFontSize = iSize;
			CComPtr<IHTMLStyle> style = getStyle(uiView());
			if (style) {
				setFontNameAndSize(style,pName,iSize);
			}
		}

		virtual void setBackgroundColor(const ccColor4B& iColor) override 
		{
			fBackgroundColor = iColor;
			CComPtr<IHTMLStyle> style = getStyle(uiView());
			if (style) {
				setBackgroundColor(style, iColor);
			}
		}

		virtual void setTextColor(const ccColor4B& iColor) override 
		{
			fTextColor = iColor;
			CComPtr<IHTMLStyle> style = getStyle(uiView());
			if (style) {
				setTextColor(style, iColor);
			}
		}

		virtual void retainFocus() override 
		{
			dowlog::myLog() << "retainFocus before SetFocus" << std::endl;
			uiView().SetFocus();
			dowlog::myLog() << "retainFocus after SetFocus" << std::endl;
		}

		virtual void releaseFocus() override 
		{
			dowlog::myLog() << "releaseFocus before SetFocus" << std::endl;
            HWND hWndGLView = CCEGLView::sharedOpenGLView()->getHWnd();
            SetFocus(hWndGLView);
			dowlog::myLog() << "releaseFocus after SetFocus" << std::endl;
		}

	private:
		std::string fText;
		std::string fFontName;
		float fFontSize;
		ccColor4B fBackgroundColor;
		ccColor4B fTextColor;
		CCMultilineTextBox* fMultilineTextBox;
		CComPtr<IDispatch>  fOnLoad;
		DWORD fCookie;
		CComPtr<IConnectionPoint> fConnectionPoint;
		CComPtr<IDispatch> fOnKeyPress;
        CComPtr<IDispatch> fOnChange;
        CComPtr<IDispatch> fOnFocus;
		CComPtr<IDispatch> fOnBlur;


		CCMultilineTextBoxDelegate* delegate() const 
		{ 
			return fMultilineTextBox->delegate();
		}

		static CComPtr<IHTMLDocument2> getHTMLDocument(CAxWindow& iWindow)
		{
			CComPtr<IHTMLDocument2> result;
			if (iWindow.IsWindow()) {
				HRESULT hr4 = iWindow.QueryControl(&result);
				assert(hr4 == S_OK);
			}
			return result;
		}

		static CComPtr<IHTMLBodyElement> getBody(CAxWindow& iWindow)
		{
			CComQIPtr<IHTMLBodyElement> body;
			IHTMLDocument2* document = getHTMLDocument(iWindow);
			if (document) {
				CComPtr<IHTMLElement> element;
				HRESULT hr3 = document->get_body(&element);
				assert(hr3 == S_OK);
				body = element;
				assert(body);
			}
			return body;
		}

		static CComPtr<IHTMLTextAreaElement> getTextArea(CAxWindow& iWindow)
		{
			CComQIPtr<IHTMLTextAreaElement> textArea;
			CComPtr<IHTMLBodyElement> body = getBody(iWindow);
			if (body) {
				CComQIPtr<IHTMLDOMNode> node = body;
				assert(node);
					
				CComPtr<IHTMLDOMNode> firstChild;
				HRESULT hr = node->get_firstChild(&firstChild);
				assert(hr == S_OK);

				textArea = firstChild;
				//maybe QI null (<P>&npsp</P>), before document is loaded
				
			}
			return textArea;
		}

		void hookCallbacks()
		{
			using namespace std::placeholders;

			CComPtr<IHTMLTextAreaElement> textArea = getTextArea(uiView());
			CComQIPtr<IHTMLElement2> element = textArea;
			if (element) {
				fOnKeyPress = CFunctionObject<CCMultilineTextBoxImplWindows>::CreateEventFunctionObject(std::bind(&CCMultilineTextBoxImplWindows::onKeyPress, this, _1, _2, _3));
				//HRESULT hr1 = element->put_onkeypress(fOnKeyPress);
				VARIANT_BOOL someResult;
				HRESULT hr1 = element->attachEvent(L"onkeypress", fOnKeyPress, &someResult);
				//assert(hr1 == S_OK);
				checkError(hr1);

                // onpropertychange (IE) - We may try also oninput and onchange
                fOnChange = CFunctionObject<CCMultilineTextBoxImplWindows>::CreateEventFunctionObject(std::bind(&CCMultilineTextBoxImplWindows::onChange, this, _1, _2, _3));
                HRESULT hr2 = element->attachEvent(L"onpropertychange", fOnChange, &someResult);
                //assert(hr2 == S_OK);
                checkError(hr2);
			}
			CComQIPtr<IHTMLElement2> element2 = textArea;
			if (element2) {
				fOnBlur = CFunctionObject<CCMultilineTextBoxImplWindows>::CreateEventFunctionObject(std::bind(&CCMultilineTextBoxImplWindows::onBlur, this, _1, _2,_3));
				VARIANT_BOOL someResult;
				HRESULT hr1 = element2->attachEvent(L"onblur", fOnBlur, &someResult);
				//HRESULT hr1 = element2->put_onblur(fOnBlur);
				checkError(hr1);
				fOnFocus = CFunctionObject<CCMultilineTextBoxImplWindows>::CreateEventFunctionObject(std::bind(&CCMultilineTextBoxImplWindows::onFocus, this, _1, _2,_3));
				//HRESULT hr2 = element2->put_onfocus(fOnFocus);
				HRESULT hr2 = element2->attachEvent(L"onfocus", fOnFocus, &someResult);
				checkError(hr2);
			}

		}

		void unhookCallbacks()
		{
			CComPtr<IHTMLTextAreaElement> textArea = getTextArea(uiView());
			CComQIPtr<IHTMLElement2> element = textArea;
			CComVariant varNull;
			if (element) {
				//element->put_onkeypress(varNull);
				HRESULT hr1 = element->detachEvent(L"onkeypress", fOnKeyPress);
				assert(hr1 == S_OK);
                HRESULT hr2 = element->detachEvent(L"onpropertychange", fOnChange);
                assert(hr2 == S_OK);
			}
			CComQIPtr<IHTMLElement2> element2 = textArea;
			if (element2) {
				HRESULT hr2 = element->detachEvent(L"onfocus", fOnFocus);
				assert(hr2 == S_OK);
				HRESULT hr3 = element->detachEvent(L"onblur", fOnBlur);
				assert(hr3 == S_OK);
				//element2->put_onblur(varNull);
				//element2->put_onfocus(varNull);
			}
		}

		static CComPtr<IHTMLStyle> getStyle(CAxWindow& iWindow)
		{
			CComPtr<IHTMLStyle> style;
			CComPtr<IHTMLTextAreaElement> textArea = getTextArea(iWindow);
			CComQIPtr<IHTMLElement> element = textArea;
			if (element) {
				HRESULT hr = element->get_style(&style);
				assert(hr == S_OK);
			}
			return style;
		}

		void updateAll() {
			CComPtr<IHTMLStyle> style = getStyle(uiView());
			if (style) {
				setTextColor(style, fTextColor);
				setBackgroundColor(style, fBackgroundColor);
				if (!fFontName.empty()) {
					setFontNameAndSize(style, fFontName, fFontSize);
				}
			}
			CComPtr<IHTMLTextAreaElement> textArea = getTextArea(uiView());
			if (textArea) {
				setText(textArea, fText);
			}

		}

		static void setBackgroundColor(CComPtr<IHTMLStyle>  style, const ccColor4B& iColor) {
			if (iColor.a != 0) {
				std::stringstream ss;
				ss << "rgb(" << int(iColor.r) << "," << int(iColor.g) << "," << int(iColor.b) << ")";
				const std::string colorStr = ss.str();
				CComVariant color(colorStr.c_str());
				HRESULT res = style->put_backgroundColor(color);
				checkError(res);
			}
		}

		static void setTextColor(CComPtr<IHTMLStyle>  style, const ccColor4B& iColor)
		{
			std::stringstream ss;
			ss << "rgb(" << int(iColor.r) << "," << int(iColor.g) << "," << int(iColor.b) << ")";
			const std::string colorStr = ss.str();
			CComVariant color(colorStr.c_str());
			HRESULT res = style->put_color(color);
			checkError(res);
		}

		static void setFontNameAndSize(CComPtr<IHTMLStyle>  style ,const std::string& iFontName , float iFontSize)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
			std::wstring wFontName = converter.from_bytes(iFontName);
			BSTR oleFontName = const_cast<BSTR>(wFontName.data());
			HRESULT res = style->put_fontFamily(oleFontName);
			checkError(res);

			CComVariant fontSize(iFontSize);
			HRESULT res1 = style->put_fontSize(fontSize);
			checkError(res1);
		}

		static void setText(CComPtr<IHTMLTextAreaElement> textArea, const std::string& iText)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
			std::wstring wText = converter.from_bytes(iText);
			BSTR oleText = const_cast<BSTR>(wText.data());
			HRESULT hr6 = textArea->put_value(oleText);
			assert(hr6 == S_OK);
		}

		static std::string getText(CComPtr<IHTMLTextAreaElement> textArea)
		{
			std::string result;
			BSTR textAreaValue = NULL;
			HRESULT hr6 = textArea->get_value(&textAreaValue);
			assert(hr6 == S_OK);
			if (textAreaValue) { // CAN legally be NULL even with S_OK
				std::wstring wideResult(textAreaValue);
				std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
				result = converter.to_bytes(wideResult);
			}
			return result;
		}

		static void checkError(HRESULT error)
		{
			if (error != S_OK) {
				_com_error comError(error);
				LPCTSTR errorText = comError.ErrorMessage();
				assert(false);
			}
		}

	};

    std::unique_ptr<CCNativeViewImpl>
    CCMultilineTextBox::_createImpl()
    {
		return std::unique_ptr<CCNativeViewImpl>(new CCMultilineTextBoxImplWindows());
    }
    
NS_CC_EXT_END


