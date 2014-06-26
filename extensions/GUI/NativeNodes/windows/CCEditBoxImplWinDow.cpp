
#include "CCEditBoxImplWinDow.h"

#include "GUI/CCEditBox/CCEditBox.h"
#include "GUI/CCEditBox/CCEditBoxImpl.h"
#include "DOWFoundation/Logger.h"

#include "FunctionObject.h"

#include <cvt/wstring>
#include <codecvt>

#include <comutil.h>
#include <boost/log/trivial.hpp>

namespace cocos2d
{
	namespace extension
	{
		CCEditBoxImpl* __createSystemEditBox(CCEditBox* pEditBox)
		{
			return new swapp::CCEditBoxImplWinDow(pEditBox);
		}
	}
}

namespace
{

	bool IsDowBetterThanWindowsXP()
	{
		OSVERSIONINFO osvi;
		ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osvi);

		return osvi.dwMajorVersion >= 6;
	}

	std::wstring innerBody(const bool isPassword) {
		std::wstringstream ss;
		const std::wstring type = isPassword ? L"password" : L"text";
		ss << "<INPUT type=" << type << " id=input style=' border: none; margin: 0; padding: 0; background-color: transparent;'>";
		return ss.str();
	}


	CComPtr<IHTMLDocument2> getDocument(CAxWindow& iWindow)
	{
		CComQIPtr<IHTMLDocument2> result;
		if (iWindow.IsWindow()) {
			CComPtr<IWebBrowser2> pWebBrowser;
			HRESULT hr0 = iWindow.QueryControl(&pWebBrowser);
			assert(hr0 == S_OK);


			CComPtr<IDispatch> pHtmlDoc;
			HRESULT hr1 = pWebBrowser->get_Document(&pHtmlDoc);
			assert(hr1 == S_OK);

			result = pHtmlDoc;
		}
		return result;
	}

	CComPtr<IHTMLInputElement> getInput(CAxWindow& iWindow)
	{
		CComPtr<IHTMLDocument2> document = getDocument(iWindow);
		CComQIPtr<IHTMLInputElement> result;
		if (document) {
			CComQIPtr<IHTMLDocument3> document3 = document;
			assert(document3);
			CComPtr<IHTMLElement> element;
			HRESULT hr2 = document3->getElementById(L"input", &element);
			assert(hr2 == S_OK);
			result = element;
		}
		return result;
	}

	CComPtr<IHTMLElement2> createInput(CAxWindow& iWindow, const bool isPassword)
	{
		CComPtr<IHTMLDocument2> document = getDocument(iWindow);
		assert(document);

		CComQIPtr<IHTMLElement> body;
		HRESULT hr1a = document->get_body(&body);
		assert(hr1a == S_OK);
		assert(body);

		const std::wstring subHtml = innerBody(isPassword);

		HRESULT hr1b = body->put_innerHTML(const_cast<BSTR>(subHtml.data()));
		assert(hr1b == S_OK);

		CComQIPtr<IHTMLDOMNode> bodyNode = body;
		assert(bodyNode);

		CComPtr<IHTMLDOMNode> firstChild;
		HRESULT hr3 = bodyNode->get_firstChild(&firstChild);
		assert(hr3 == S_OK);
		assert(firstChild);

		CComQIPtr<IHTMLElement2> inputElement2 = firstChild;
		assert(inputElement2);
		return inputElement2;
#if 0
		SetFocus(iWindow.m_hWnd); 

		HRESULT hr4 = inputElement2->focus();
		assert(hr4 == S_OK);
#endif

	}



	LRESULT APIENTRY EditBoxWndProc(
		HWND hwnd,        // handle to window
		UINT uMsg,        // message identifier
		WPARAM wParam,    // first message parameter
		LPARAM lParam)    // second message parameter
	{
		if (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) {
			BOOST_LOG_TRIVIAL(debug) << "EditBoxWndProc: " << uMsg << " : " << wParam << " -> " << lParam << std::endl;
		}
		LONG_PTR oldWndProc = GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (oldWndProc) {
			return CallWindowProc(reinterpret_cast<WNDPROC>(oldWndProc), hwnd, uMsg, wParam, lParam);
		}
		else {
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
}

NS_CC_EXT_BEGIN

	using namespace cocos2d;
	using namespace cocos2d::extension;
	using namespace std::placeholders;
	

	CCEditBoxImplWinDow::CCEditBoxImplWinDow(CCEditBox* pEditText)
		: CCEditBoxImpl(pEditText)
		, m_pLabel(NULL)
		, m_eEditBoxInputMode(kEditBoxInputModeSingleLine)
		, m_eEditBoxInputFlag(kEditBoxInputFlagInitialCapsAllCharacters)
		, m_eKeyboardReturnType(kKeyboardReturnTypeDefault)
		, m_colText(ccWHITE) 
		, m_nMaxLength(-1)
		, fNativeView(*this)
	{
		fIsEditing = false;
	}

	CCEditBoxImplWinDow::~CCEditBoxImplWinDow()
	{
		unhookCallbacks();
	}

	void CCEditBoxImplWinDow::doAnimationWhenKeyboardMove(float duration, float distance)
	{
	}

	bool CCEditBoxImplWinDow::initWithSize(const CCSize& size)
	{
		m_pLabel = CCLabelTTF::create("", "", size.height - 12);
		// align the text vertically center
		m_pLabel->setAnchorPoint(ccp(0, 0.5f));
		m_pLabel->setPosition(ccp(5, size.height / 2.0f));
		m_pLabel->setColor(m_colText);
		m_pEditBox->addChild(m_pLabel);

		fPlaceholder = CCLabelTTF::create("", "", size.height - 12);
		fPlaceholder->setAnchorPoint(ccp(0, 0.5f));
		fPlaceholder->setPosition(ccp(5, size.height / 2.0f));
		fPlaceholder->setColor(ccGRAY);
		m_pEditBox->addChild(fPlaceholder);

		m_EditSize = size;

		fNativeView.init(m_pEditBox);
		return true;
	}

	void CCEditBoxImplWinDow::setFont(const char* pFontName, int fontSize)
	{
		if (m_pLabel != NULL) {
			m_pLabel->setFontName(pFontName);
			m_pLabel->setFontSize(fontSize);
		}
		if (fPlaceholder != NULL) {
			fPlaceholder->setFontName(pFontName);
			fPlaceholder->setFontSize(fontSize);
		}
	}

	void CCEditBoxImplWinDow::setFontColor(const ccColor3B& color)
	{
		m_colText = color;
		m_pLabel->setColor(color);
	}

	void CCEditBoxImplWinDow::setPlaceholderFont(const char* pFontName, int fontSize)
	{
		if (fPlaceholder != NULL) {
			fPlaceholder->setFontName(pFontName);
			fPlaceholder->setFontSize(fontSize);
		}
	}

	void CCEditBoxImplWinDow::setPlaceholderFontColor(const ccColor3B& color)
	{
		fPlaceholder->setColor(color);
	}

	void CCEditBoxImplWinDow::setInputMode(EditBoxInputMode inputMode)
	{
		m_eEditBoxInputMode = inputMode;
	}

	void CCEditBoxImplWinDow::setMaxLength(int maxLength)
	{
		m_nMaxLength = maxLength;
	}

	int CCEditBoxImplWinDow::getMaxLength()
	{
		return m_nMaxLength;
	}

	void CCEditBoxImplWinDow::setInputFlag(EditBoxInputFlag inputFlag)
	{
		m_eEditBoxInputFlag = inputFlag;
	}

	void CCEditBoxImplWinDow::setReturnType(KeyboardReturnType returnType)
	{
		m_eKeyboardReturnType = returnType;
	}

	bool CCEditBoxImplWinDow::isEditing()
	{
		return fIsEditing;
	}

	void CCEditBoxImplWinDow::setText(const char* pText)
	{
		if (pText != NULL)
		{
			m_strText = pText;

			if (m_strText.length() > 0)
			{
				std::string strToShow;

				if (kEditBoxInputFlagPassword == m_eEditBoxInputFlag)
				{
					long length = cc_utf8_strlen(m_strText.c_str(), -1);
					for (long i = 0; i < length; i++)
					{
						strToShow.append("*");
					}
				}
				else
				{
					strToShow = m_strText;
				}

				//! std::string strWithEllipsis = getStringWithEllipsisJni(strToShow.c_str(), m_EditSize.width, m_EditSize.height-12);
				//! m_pLabel->setString(strWithEllipsis.c_str());
				m_pLabel->setString(strToShow.c_str());
			}
			else
			{
				m_pLabel->setString("");
			}

		}
		if (!fIsEditing) {
			if (m_strText.empty()) {
				m_pLabel->setVisible(false);
				fPlaceholder->setVisible(true);
			} else {
				m_pLabel->setVisible(true);
				fPlaceholder->setVisible(false);
			}
		}
	}

	const char*  CCEditBoxImplWinDow::getText(void)
	{
		return m_strText.c_str();
	}

	void CCEditBoxImplWinDow::setPlaceHolder(const char* pText)
	{
		fPlaceholder->setString(pText);
	}

	void CCEditBoxImplWinDow::setPosition(const CCPoint& pos)
	{
		//m_pLabel->setPosition(pos);
		//m_pLabelPlaceHolder->setPosition(pos);
	}

	void CCEditBoxImplWinDow::setVisible(bool visible)
	{ // don't need to be implemented on win32 platform.
	}

	void CCEditBoxImplWinDow::setContentSize(const CCSize& size)
	{
	}

	void CCEditBoxImplWinDow::setAnchorPoint(const CCPoint& anchorPoint)
	{ // don't need to be implemented on win32 platform.

	}

	void CCEditBoxImplWinDow::visit(void)
	{
	}

	VOID CALLBACK TimerProc(
		_In_  HWND hwnd,
		_In_  UINT uMsg,
		_In_  UINT_PTR idEvent,
		_In_  DWORD dwTime
		) {
		KillTimer(hwnd, idEvent); // first, make sure this is done
		BOOST_LOG_TRIVIAL(debug) << "TimerProc: " << idEvent << " after " << dwTime  << std::endl;
		CComPtr<IHTMLElement2> inputElement = reinterpret_cast<IHTMLElement2*>(idEvent);

		HRESULT hr4 = inputElement->focus();
		assert(hr4 == S_OK);
	}

	void CCEditBoxImplWinDow::openKeyboard()
	{
		const std::string text = getText();
		const std::string fontName = m_pLabel->getFontName();
		const float fontSize = m_pLabel->getFontSize();

		CComPtr<IHTMLElement2> inputElement = createInput(fNativeView.axWindow(), m_eEditBoxInputFlag == kEditBoxInputFlagPassword);

		fNativeView.setFontName(fontName);
		fNativeView.setFontSize(fontSize);
		fNativeView.setText(text);
		fNativeView.setVisible(true);

		m_pLabel->setVisible(false);
		fPlaceholder->setVisible(false);
		fIsEditing = true;
		hookCallbacks();

#if 0
		//SetFocus(fNativeView.axWindow().m_hWnd); not the correct windows, this is the host to the actual browser window
		HRESULT hr4 = inputElement->focus();
		assert(hr4 == S_OK);
#else
		const UINT_PTR timerId = reinterpret_cast<UINT_PTR>(inputElement.Detach());
		const UINT_PTR timerIdOut = SetTimer(fNativeView.axWindow().m_hWnd, timerId, 50 , TimerProc);
		assert(timerIdOut == timerId);
		BOOST_LOG_TRIVIAL(debug) << "CCEditBoxImplWinDow::openKeyboard timerId: " << timerId << std::endl;

#endif
	}



	void CCEditBoxImplWinDow::closeKeyboard()
	{
#if 1
		unhookCallbacks();
		fNativeView.setVisible(false);
		m_pLabel->setVisible(true);
#endif
	}

	void CCEditBoxImplWinDow::onEnter(void)
	{
		fNativeView.onEnter();
	}


	void CCEditBoxImplWinDow::onEnterTransitionDidFinish()
	{
		fNativeView.onEnterTransitionDidFinish();
	}

	void CCEditBoxImplWinDow::onExitTransitionDidStart()
	{
		fNativeView.onExitTransitionDidStart();
	}

	void CCEditBoxImplWinDow::setAutoCorrectionMode(EditBoxAutoCorrectionMode mode)
	{

	}

	bool CCEditBoxImplWinDow::clearsOnBeginEditing() const
	{
		return false;
	}

	void CCEditBoxImplWinDow::setClearsOnBeginEditing(bool iEnable)
	{

	}

	void CCEditBoxImplWinDow::setClearButtonMode(EditBoxClearButtonMode iMode)
	{

	}

	static void _setupLabel(CCLabelTTF* iLabel, const CCRect& iInnerBounds)
	{
		iLabel->ignoreAnchorPointForPosition(false);
		iLabel->setAnchorPoint(ccp(0, 0.5f));

		iLabel->setHorizontalAlignment(kCCTextAlignmentLeft);
		iLabel->setVerticalAlignment(kCCVerticalTextAlignmentCenter);

		iLabel->setPosition(ccp(iInnerBounds.origin.x, iInnerBounds.getMidY()));
		iLabel->setDimensions(iInnerBounds.size);
	}


	void CCEditBoxImplWinDow::needsLayout()
	{
#if 0
		CCEditBox* editBox = getCCEditBox();
		assert(editBox);
		const CCRect inside = editBox->inputLocalBounds();
		CCPoint pos;
		pos.x = inside.origin.x;
		pos.y = inside.origin.y + inside.size.height/2;
		m_pLabel->setPosition(pos);
#else
		CCRect inputBox = m_pEditBox->inputLocalBounds();
		_setupLabel(m_pLabel, inputBox);
		_setupLabel(fPlaceholder, inputBox);
#endif
	}

	CCLabelTTF* CCEditBoxImplWinDow::getLabel() const
	{
		return m_pLabel;
	}

	void CCEditBoxImplWinDow::setLabel(CCLabelTTF* iLabel)
	{
		const int kLabelZOrder = 9999;

		if (m_pLabel != iLabel)
		{
			if (m_pLabel != NULL)
			{
				m_pLabel->removeFromParentAndCleanup(true);
				CC_SAFE_RELEASE(m_pLabel);
			}

			m_pLabel = iLabel;
			CC_SAFE_RETAIN(m_pLabel);

			if (m_pLabel != NULL)
			{
				m_pLabel->removeFromParentAndCleanup(true);

				m_pLabel->setVisible(false);



				m_pEditBox->addChild(m_pLabel, kLabelZOrder);

				// Update the native text field accordingly
				setFont(m_pLabel->getFontName(), m_pLabel->getFontSize());
				setFontColor(m_pLabel->getDisplayedColor());
			}
		}
	}



	HRESULT CCEditBoxImplWinDow::onKeyDown(DISPID id, DISPPARAMS* pDispParams, VARIANT* pVarResult)
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
		dowlog::myLog() << "onKeyDown: " << keyCode << std::endl;
		return S_OK;
	}

	HRESULT CCEditBoxImplWinDow::onKeyUp(DISPID id, DISPPARAMS* pDispParams, VARIANT* pVarResult)
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
		dowlog::myLog() << "onKeyUp: " << keyCode << std::endl;

		if (m_pDelegate != NULL) {
			const std::string newText = fNativeView.getText();
			setText(newText.c_str());
			m_pDelegate->editBoxTextChanged(m_pEditBox, newText);
		}

		return S_OK;
	}


	HRESULT CCEditBoxImplWinDow::onKeyPress(DISPID id, DISPPARAMS* pDispParams, VARIANT* pVarResult)
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

		if (keyCode == VK_RETURN || keyCode == VK_TAB) {
			exitEditMode();
		} 

		return S_OK;
	}


	void CCEditBoxImplWinDow::hookCallbacks()
	{
		if (fNativeView.axWindow().IsWindow()) {
			CComPtr<IHTMLInputElement> inputElement = getInput(fNativeView.axWindow());
			CComQIPtr<IHTMLElement2> element = inputElement;
			CComVariant varNull;
			if (element) {
				fOnKeyPress = CFunctionObject<CCEditBoxImplWinDow>::CreateEventFunctionObject(std::bind(&CCEditBoxImplWinDow::onKeyPress, this, _1, _2, _3));
				fOnKeyDown = CFunctionObject<CCEditBoxImplWinDow>::CreateEventFunctionObject(std::bind(&CCEditBoxImplWinDow::onKeyDown, this, _1, _2, _3));
				fOnKeyUp = CFunctionObject<CCEditBoxImplWinDow>::CreateEventFunctionObject(std::bind(&CCEditBoxImplWinDow::onKeyUp, this, _1, _2, _3));

				fOnFocus = CFunctionObject<CCEditBoxImplWinDow>::CreateEventFunctionObject([this](DISPID id, DISPPARAMS* pDispParams, VARIANT* pVarResult){
					assert(pDispParams);
					assert(pDispParams->cArgs == 1);
					CComPtr<IDispatch> spEventObj = pDispParams->rgvarg[0].pdispVal;
					assert(spEventObj);
					CComQIPtr<IHTMLEventObj> pHtmlEvent = spEventObj;
					assert(pHtmlEvent);

					CComPtr<IHTMLElement> srcElement;
					HRESULT hr1 = pHtmlEvent->get_srcElement(&srcElement);
					assert(hr1 == S_OK);

					BSTR tagName;
					HRESULT hr2 = srcElement->get_tagName(&tagName);
					assert(hr2 == S_OK);

					BOOST_LOG_TRIVIAL(debug) << "CCEditBoxImplWinDow::hookCallbacks focus: " << std::wstring(tagName) << std::endl;

					if (m_pDelegate != NULL)
					{
						m_pDelegate->editBoxEditingDidBegin(m_pEditBox);
					}
					return S_OK;
				});

				fOnBlur = CFunctionObject<CCEditBoxImplWinDow>::CreateEventFunctionObject([this](DISPID id, DISPPARAMS* pDispParams, VARIANT* pVarResult){
					assert(pDispParams);
					assert(pDispParams->cArgs==1);
					CComPtr<IDispatch> spEventObj = pDispParams->rgvarg[0].pdispVal;
					assert(spEventObj);
					CComQIPtr<IHTMLEventObj> pHtmlEvent = spEventObj;
					assert(pHtmlEvent);

					CComPtr<IHTMLElement> srcElement;
					HRESULT hr1 = pHtmlEvent->get_srcElement(&srcElement);
					assert(hr1 == S_OK);

					BSTR tagName;
					HRESULT hr2 = srcElement->get_tagName(&tagName);
					assert(hr2 == S_OK);

					BOOST_LOG_TRIVIAL(debug) << "CCEditBoxImplWinDow::hookCallbacks blur: " << std::wstring(tagName) << std::endl;

					exitEditMode();

					/*
					if (m_pDelegate != NULL) {
						m_pDelegate->editBoxEditingDidEnd(m_pEditBox);
					}
					*/
					return S_OK;
				});

				VARIANT_BOOL someResult;
				HRESULT hr1 = element->attachEvent(L"onkeypress", fOnKeyPress, &someResult);
				assert(hr1 == S_OK);
				HRESULT hr1a = element->attachEvent(L"onkeydown", fOnKeyDown, &someResult);
				assert(hr1a == S_OK);
				HRESULT hr1b = element->attachEvent(L"onkeyup", fOnKeyUp, &someResult);
				assert(hr1b == S_OK);
				HRESULT hr2 = element->attachEvent(L"onfocus", fOnFocus, &someResult);
				assert(hr2 == S_OK);
				HRESULT hr3 = element->attachEvent(L"onblur", fOnBlur, &someResult);
				assert(hr3 == S_OK);
			}
		}

		CCNativeViewImplWindows::Accelerator accelerator = [this](LPMSG lpmsg) -> bool {
			CComPtr<IWebBrowser2> pWebBrowser;
			CComQIPtr<IOleInPlaceActiveObject> pIOIPAO;
			HRESULT hr0 = fNativeView.axWindow().QueryControl(&pWebBrowser);
			assert(hr0 == S_OK);
			pIOIPAO = pWebBrowser;
			assert(pIOIPAO);
			HRESULT hr = pIOIPAO->TranslateAccelerator(lpmsg);
			return hr == S_OK;
		};
		fNativeView.setAccelerator(accelerator);
	}

	void CCEditBoxImplWinDow::unhookCallbacks()
	{
		fNativeView.setAccelerator(nullptr);
		if (fNativeView.axWindow().IsWindow()) {
			CComPtr<IHTMLInputElement> inputElement = getInput(fNativeView.axWindow());
			CComQIPtr<IHTMLElement2> element = inputElement;
			CComVariant varNull;
			if (element) {
				HRESULT hr1 = element->detachEvent(L"onkeypress", fOnKeyPress);
				assert(hr1 == S_OK);
				HRESULT hr1a = element->detachEvent(L"onkeydown", fOnKeyDown);
				assert(hr1a == S_OK);
				HRESULT hr1b = element->detachEvent(L"onkeyup", fOnKeyUp);
				assert(hr1b == S_OK);
				HRESULT hr2 = element->detachEvent(L"onfocus", fOnFocus);
				assert(hr2 == S_OK);
				HRESULT hr3 = element->detachEvent(L"onblur", fOnBlur);
				assert(hr3 == S_OK);
			}
		}
	}



	void CCEditBoxImplWinDow::exitEditMode()
	{
		const std::string oldText = getText();
		const std::string newText = fNativeView.getText();

		const bool didChange = newText != oldText;
		if (didChange) {
			setText(newText.c_str());
		}

		if (m_pDelegate != NULL) {
			if (didChange) {
				m_pDelegate->editBoxTextChanged(m_pEditBox, getText());
			}
			m_pDelegate->editBoxReturn(m_pEditBox);
			m_pDelegate->editBoxEditingDidEnd(m_pEditBox);
		}
		unhookCallbacks();
		fNativeView.setVisible(false);
		m_pLabel->setVisible(true);
		fPlaceholder->setVisible(true);
		fIsEditing = false;
	}


	CCRect NativeEditBoxWindows::getContentArea()
	{
		CCRect result;

		CCEditBox* editBox = fCCEditBoxImplWinDow.getCCEditBox();
		assert(editBox);
		const CCRect inside = editBox->inputLocalBounds();
		result = inside;

		return result;
	}

	void NativeEditBoxWindows::_createUIView()
	{
		HWND hParent = CCEGLView::sharedOpenGLView()->getHWnd();
		RECT emptyRect;
		emptyRect.top = 0;
		emptyRect.left = 0;
		emptyRect.bottom = 0;
		emptyRect.right = 0;
		DWORD dwExStyle = WS_EX_NOACTIVATE;
		if (IsDowBetterThanWindowsXP()) {
			dwExStyle |= WS_EX_LAYERED;
		}
		else {
			BOOST_LOG_TRIVIAL(info) << "NativeEditBoxWindows::_createUIView Windows XP workaround: No WS_EX_LAYERED" << std::endl;
		}
		HWND child = uiView().Create(hParent, emptyRect, "about:blank", WS_POPUP, dwExStyle);

		if (!child) {
			DWORD error = ::GetLastError();
			assert(error);
		}
		BOOL isOK = SetLayeredWindowAttributes(child, RGB(255, 255, 255), 0, LWA_COLORKEY);
		assert(isOK);

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

		std::stringstream ss;
		ss << "<HTML><BODY style='background-color: transparent; margin: 0; padding: 0; overflow: hidden'>";
		ss << "</BODY></HTML>";
		const std::string html(ss.str());

		CComPtr<IStream> pStream(SHCreateMemStream(reinterpret_cast<const BYTE *>(html.data()), html.size()));
		HRESULT hr4 = pPersistStreamInit->Load(pStream);
		assert(hr4 == S_OK);

		//WNDPROC newProc = EditBoxWndProc;
		//LONG_PTR oldProc = uiView().SetWindowLongPtr(GWL_WNDPROC, reinterpret_cast<LONG_PTR>(newProc));		
		//LONG_PTR oldData = uiView().SetWindowLongPtr(GWLP_USERDATA, oldProc);
		//assert(oldData==0);
	}

	NativeEditBoxWindows::NativeEditBoxWindows(CCEditBoxImplWinDow& iCCEditBoxImplWinDow)
		: fCCEditBoxImplWinDow(iCCEditBoxImplWinDow)
	{
		fVisible = false;
	}

	void NativeEditBoxWindows::setVisible(bool isVisible)
	{
		if (fVisible != isVisible) {
			fVisible = isVisible;
			_updateNativeUIView();
		}
	}

	bool NativeEditBoxWindows::isVisible() const
	{
		return fVisible;
	}

	void NativeEditBoxWindows::_updateNativeUIView()
	{
		CCNativeViewImplWindows::_updateNativeUIView();

		if (fVisible) {
			if (uiView().IsWindow()) {
				CComPtr<IHTMLInputElement> inputElement = getInput(uiView());
				if (inputElement) {
					CComQIPtr<IHTMLElement> element = inputElement;
					if (element) {

#if 0

						CComPtr<IHTMLStyle> style;
						HRESULT hr = element->get_style(&style);
						assert(hr == S_OK);

						RECT clientRect;
						uiView().GetClientRect(&clientRect);

						int width = clientRect.right - clientRect.left;
						int height = clientRect.bottom - clientRect.top;

						HRESULT hr1 = style->put_width(CComVariant(width));
						assert(hr1 == S_OK);

						HRESULT hr2 = style->put_height(CComVariant(height));
						assert(hr2 == S_OK);

						HRESULT hr3 = style->put_lineHeight(CComVariant(height));
						assert(hr3 == S_OK);


						const float paddingFloat = (height - fFontSize) / 2;

						CComVariant padding(paddingFloat);
						HRESULT hr5 = style->put_paddingTop(padding);
						assert(hr5 == S_OK);
						HRESULT hr6 = style->put_paddingBottom(padding);
						assert(hr6 == S_OK);

						CComVariant fontSize(fontSizeFloat);
						HRESULT hr1 = style->put_fontSize(fontSize);
						assert(hr1 == S_OK);

						CComVariant padding(paddingFloat);
						HRESULT hr4 = style->put_paddingTop(padding);
						assert(hr4 == S_OK);
						HRESULT hr5 = style->put_paddingBottom(padding);
						assert(hr5 == S_OK);
#endif
					}
				}
			}
		}
	}


	void NativeEditBoxWindows::setFontName(const std::string& iFontName)
	{
		if (uiView().IsWindow()) {
			CComPtr<IHTMLInputElement> inputElement = getInput(axWindow());
			if (inputElement) {
				CComQIPtr<IHTMLElement> element = inputElement;
				if (element) {
					CComPtr<IHTMLStyle> style;
					HRESULT hr = element->get_style(&style);
					assert(hr == S_OK);


					std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
					std::wstring wFontName = converter.from_bytes(iFontName);
					BSTR oleFontName = const_cast<BSTR>(wFontName.data());
					HRESULT hr2 = style->put_fontFamily(oleFontName);
					assert(hr2 == S_OK);
				}

			}
		}

	}

	void NativeEditBoxWindows::setFontSize(float iFontSize)
	{
		fFontSize = iFontSize;

		if (uiView().IsWindow()) {
			CComPtr<IHTMLInputElement> inputElement = getInput(axWindow());
			if (inputElement) {
				CComQIPtr<IHTMLElement> element = inputElement;
				if (element) {
					CComPtr<IHTMLStyle> style;
					HRESULT hr = element->get_style(&style);
					assert(hr == S_OK);

					CComVariant fontSize(iFontSize);
					HRESULT hrb = style->put_fontSize(fontSize);
					assert(hrb == S_OK);

					//RECT clientRect;
					//uiView().GetClientRect(&clientRect);
					CCRect localRect = getContentArea();

					const float width = localRect.size.width;
					const float height = localRect.size.height;

					HRESULT hr1 = style->put_width(CComVariant(width));
					assert(hr1 == S_OK);

					HRESULT hr2 = style->put_height(CComVariant(height));
					assert(hr2 == S_OK);

					//HRESULT hr3 = style->put_lineHeight(CComVariant(height));
					//assert(hr3 == S_OK);


					const float paddingFloat = (height - fFontSize) / 2;
					assert(paddingFloat>0);
					CComVariant padding(paddingFloat);
					
					HRESULT hr5 = style->put_paddingTop(padding);
					assert(hr5 == S_OK);
					//HRESULT hr6 = style->put_paddingBottom(padding);
					//assert(hr6 == S_OK);

					BSTR outerHtml;
					HRESULT hr7 = element->get_outerHTML(&outerHtml);
					assert(hr7 == S_OK);

					BOOST_LOG_TRIVIAL(debug) << "CCEditBoxViewImplWindows::setFontSize outerHtml: " << std::wstring(outerHtml) << std::endl;


				}
			}
		}
	}


	void NativeEditBoxWindows::setText(const std::string& iText)
	{
		if (uiView().IsWindow()) {
			CComPtr<IHTMLInputElement> inputElement = getInput(axWindow());
			if (inputElement) {
				std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
				std::wstring wText = converter.from_bytes(iText);
				BSTR value = const_cast<BSTR>(wText.data());
				inputElement->put_value(value);
			}
		}
	}

	std::string NativeEditBoxWindows::getText()
	{
		std::string result;
		if (uiView().IsWindow()) {
			CComPtr<IHTMLInputElement> inputElement = getInput(axWindow());
			if (inputElement) {
				BSTR value;
				inputElement->get_value(&value);
				if (value) {
					std::wstring wideResult = value;
					std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
					result = converter.to_bytes(wideResult);
				}
			}
		}
		return result;
	}

NS_CC_EXT_END


