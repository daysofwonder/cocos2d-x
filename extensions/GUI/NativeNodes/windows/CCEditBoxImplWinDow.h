//
#ifndef __CCEditBoxIMPLWINDOW_H__
#define __CCEditBoxIMPLWINDOW_H__

#include "cocos2d.h"
#include "GUI/CCEditBox/CCEditBoxImpl.h"
#include "CCNativeViewImplWindows.h"

#include <atlbase.h> 
#include <atlwin.h>

NS_CC_EXT_BEGIN

	class CCEditBoxImplWinDow;

	class NativeEditBoxWindows : public CCNativeViewImplWindows
	{
	public:
		NativeEditBoxWindows(CCEditBoxImplWinDow& iCCEditBoxImplWinDow);
		CAxWindow& axWindow() const { return uiView(); }
		void setVisible(bool isVisible);
		void setFontSize(float iFontSize);
		void setFontName(const std::string& iFontName);
		void setText(const std::string& iText);
		std::string getText();

	protected:
		virtual void _createUIView() override;
		virtual CCRect getContentArea() override;
		virtual bool isVisible() const override;
		virtual void _updateNativeUIView() override;

	private:
		CCEditBoxImplWinDow& fCCEditBoxImplWinDow;
		bool fVisible;
		float fFontSize;
	};


	class CCEditBoxImplWinDow : public cocos2d::extension::CCEditBoxImpl
	{
	public:
		CCEditBoxImplWinDow(cocos2d::extension::CCEditBox* pEditText);
		virtual ~CCEditBoxImplWinDow();

		virtual bool initWithSize(const cocos2d::CCSize& size);
		virtual void setFont(const char* pFontName, int fontSize);
		virtual void setFontColor(const cocos2d::ccColor3B& color);
		virtual void setPlaceholderFont(const char* pFontName, int fontSize);
		virtual void setPlaceholderFontColor(const cocos2d::ccColor3B& color);
		virtual void setInputMode(cocos2d::extension::EditBoxInputMode inputMode);
		virtual void setInputFlag(cocos2d::extension::EditBoxInputFlag inputFlag);
		virtual void setMaxLength(int maxLength);
		virtual int  getMaxLength();
		virtual void setReturnType(cocos2d::extension::KeyboardReturnType returnType);
		virtual bool isEditing();

		virtual void setText(const char* pText);
		virtual const char* getText(void);
		virtual void setPlaceHolder(const char* pText);
		virtual void setPosition(const cocos2d::CCPoint& pos);
		virtual void setVisible(bool visible);
		virtual void setContentSize(const cocos2d::CCSize& size);
		virtual void setAnchorPoint(const cocos2d::CCPoint& anchorPoint);
		virtual void visit(void);
		virtual void doAnimationWhenKeyboardMove(float duration, float distance);
		virtual void openKeyboard();
		virtual void closeKeyboard();
		virtual void onEnter(void);

		// DoW Additions
		virtual void onEnterTransitionDidFinish() override;
		virtual void onExitTransitionDidStart() override;
		virtual void setAutoCorrectionMode(cocos2d::extension::EditBoxAutoCorrectionMode mode) override;
		virtual bool clearsOnBeginEditing() const override;
		virtual void setClearsOnBeginEditing(bool iEnable) override;
		virtual void setClearButtonMode(cocos2d::extension::EditBoxClearButtonMode iMode) override;
		virtual void needsLayout() override;
		virtual cocos2d::CCLabelTTF* getLabel() const override;
		virtual void setLabel(cocos2d::CCLabelTTF* iLabel) override;

		HRESULT onKeyPress(DISPID id, DISPPARAMS* pDispParams, VARIANT* pVarResult);
		HRESULT onKeyDown(DISPID id, DISPPARAMS* pDispParams, VARIANT* pVarResult);
		HRESULT onKeyUp(DISPID id, DISPPARAMS* pDispParams, VARIANT* pVarResult);

		cocos2d::extension::EditBoxInputFlag    inputFlag() const { return m_eEditBoxInputFlag; }

	private:
		cocos2d::CCLabelTTF* m_pLabel;
		cocos2d::extension::EditBoxInputMode    m_eEditBoxInputMode;
		cocos2d::extension::EditBoxInputFlag    m_eEditBoxInputFlag;
		cocos2d::extension::KeyboardReturnType  m_eKeyboardReturnType;
		std::string m_strText;
		cocos2d::ccColor3B m_colText;
		int   m_nMaxLength;
		cocos2d::CCSize m_EditSize;
		CComPtr<IDispatch> fOnKeyDown;
		CComPtr<IDispatch> fOnKeyUp;
		CComPtr<IDispatch> fOnKeyPress;
		CComPtr<IDispatch> fOnFocus;
		CComPtr<IDispatch> fOnBlur;

		NativeEditBoxWindows fNativeView;
		cocos2d::CCLabelTTF* fPlaceholder;
		bool fIsEditing;

		void hookCallbacks();
		void unhookCallbacks();
		void exitEditMode();
	};

NS_CC_EXT_END

#endif

