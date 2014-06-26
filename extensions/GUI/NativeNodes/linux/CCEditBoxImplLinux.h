#ifndef __CCEditBoxIMPLLINUX_H__
#define __CCEditBoxIMPLLINUX_H__

#include "GUI/CCEditBox/CCEditBoxImpl.h"
#include "GUI/CCEditBox/CCEditBox.h"
#include <gtk/gtk.h>
#include "CCNativeViewImplLinux.h"
#include "ExtensionMacros.h"

NS_CC_EXT_BEGIN

class CCEditBoxImplLinux : public CCNativeViewImplLinux , public cocos2d::extension::CCEditBoxImpl
{
public:
	CCEditBoxImplLinux(cocos2d::extension::CCEditBox* pEditText);
	virtual ~CCEditBoxImplLinux();

	virtual bool initWithSize(const cocos2d::CCSize& size) override;
	virtual void setFont(const char* pFontName, int fontSize) override;
	virtual void setFontColor(const cocos2d::ccColor3B& color) override;
	virtual void setPlaceholderFont(const char* pFontName, int fontSize) override;
	virtual void setPlaceholderFontColor(const cocos2d::ccColor3B& color) override;
	virtual void setInputMode(cocos2d::extension::EditBoxInputMode inputMode) override;
	virtual void setInputFlag(cocos2d::extension::EditBoxInputFlag inputFlag) override;
	virtual void setMaxLength(int maxLength) override;
	virtual int  getMaxLength() override;
	virtual void setReturnType(cocos2d::extension::KeyboardReturnType returnType) override;
	virtual bool isEditing() override;

	virtual void setText(const char* pText) override;
	virtual const char* getText(void) override;
	virtual void setPlaceHolder(const char* pText) override;
	virtual void setPosition(const cocos2d::CCPoint& pos) override;
	virtual void setVisible(bool visible) override;
	virtual void setContentSize(const cocos2d::CCSize& size) override;
	virtual void setAnchorPoint(const cocos2d::CCPoint& anchorPoint) override;
	virtual void visit(void) override;
	virtual void doAnimationWhenKeyboardMove(float duration, float distance) override;
	virtual void openKeyboard() override;
	virtual void closeKeyboard() override;
	virtual void onEnter(void) override;

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

protected:
	virtual cocos2d::CCRect _getContentArea() override final;

private:
	cocos2d::CCLabelTTF* fLabel;
	GtkWidget* fEntry;
    std::string fText;
    gulong fOnFocusIn;
    gulong fOnFocusOut;
    gulong fOnKeyReleased;
    std::string fFontName;
    int fFontSize;
    cocos2d::extension::EditBoxInputFlag fInputFlag;

    //
    static gboolean onFocusIn(GtkWidget* widget,GdkEventFocus* event,gpointer user_data);
    static gboolean onFocusOut(GtkWidget* widget,GdkEventFocus* event,gpointer user_data);
    static gboolean onKeyReleased(GtkWidget* widget,GdkEventKey* event,gpointer user_data);
    //
    void _attachEntry();
    void _detachEntry();
    cocos2d::CCRect _calcViewRect();
    void _positionLabel();
    void _beginEdit();
    void _endEdit();

    GtkWidget* _createWebView();
    void _nativeGetText(std::string& oText);
    void _nativeSetText();
    GObject* _getInputElement();
    void _nativeSetFont();

    static void _loadFinished(GtkWidget *view, const char *uri, gpointer user_data);

};

NS_CC_EXT_END

#endif
