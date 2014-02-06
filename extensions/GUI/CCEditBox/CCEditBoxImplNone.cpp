#include "CCEditBox.h"

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID ) && (CC_TARGET_PLATFORM != CC_PLATFORM_IOS ) && (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32) && (CC_TARGET_PLATFORM != CC_PLATFORM_MAC) && (CC_TARGET_PLATFORM != CC_PLATFORM_TIZEN)

#include "ExtensionMacros.h"
#include "CCEditBoxImpl.h"

NS_CC_EXT_BEGIN

class CCEditBoxImplNone : public CCEditBoxImpl
{
public:
    CCEditBoxImplNone(CCEditBox* pEditText) : CCEditBoxImpl(pEditText) {}
    virtual ~CCEditBoxImplNone() {}

    virtual bool initWithSize(const CCSize& size) {}
    virtual void setFont(const char* pFontName, int fontSize) {}
    virtual void setFontColor(const ccColor3B& color) {}
    virtual void setPlaceholderFont(const char* pFontName, int fontSize) {}
    virtual void setPlaceholderFontColor(const ccColor3B& color) {}
    virtual void setInputMode(EditBoxInputMode inputMode) {}
    virtual void setInputFlag(EditBoxInputFlag inputFlag) {}
    virtual void setMaxLength(int maxLength) {}
    virtual int  getMaxLength() {}
    virtual void setReturnType(KeyboardReturnType returnType) {}
    virtual bool isEditing() {}

    virtual void setText(const char* pText) {fText=pText;}
    virtual const char* getText(void) {return fText.c_str();}
    virtual void setPlaceHolder(const char* pText) {}
    virtual void setPosition(const CCPoint& pos) {}
    virtual void setVisible(bool visible) {}
    virtual void setContentSize(const CCSize& size) {}
    virtual void setAnchorPoint(const CCPoint& anchorPoint) {}
    virtual void visit(void) {}
    virtual void doAnimationWhenKeyboardMove(float duration, float distance) {}
    virtual void openKeyboard() {}
    virtual void closeKeyboard() {}
    virtual void onEnter(void) {}

    // DoW Additions
    virtual void onEnterTransitionDidFinish() override {}
    virtual void onExitTransitionDidStart() override {}
    virtual void setAutoCorrectionMode(EditBoxAutoCorrectionMode mode) override {}
    virtual bool clearsOnBeginEditing() const override {}
    virtual void setClearsOnBeginEditing(bool iEnable) override {}
    virtual void setClearButtonMode(EditBoxClearButtonMode iMode) override {}
    virtual void needsLayout() override {}
    virtual CCLabelTTF* getLabel() const override {}
    virtual void setLabel(CCLabelTTF* iLabel) override {}
private:
    std::string fText;

};


CCEditBoxImpl* __createSystemEditBox(CCEditBox* pEditBox)
{
    return new CCEditBoxImplNone(pEditBox);
}

NS_CC_EXT_END

#endif /* #if (..) */
