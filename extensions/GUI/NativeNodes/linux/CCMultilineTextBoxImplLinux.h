//
#include "GUI/NativeNodes/CCMultilineTextBoxImpl.h"
#include "GUI/NativeNodes/CCMultilineTextBox.h"
#include <gtk/gtk.h>
#include "CCNativeViewImplLinux.h"

NS_CC_EXT_BEGIN

	class CCMultilineTextBoxImplLinux : public CCNativeViewImplLinux, public CCMultilineTextBoxImpl
	{
	public:
		CCMultilineTextBoxImplLinux();
		virtual ~CCMultilineTextBoxImplLinux();

		virtual bool init(CCNode* iNode) override final;
		virtual CCNode* owner() const override final;
		virtual void onEnter() override final;
		virtual void onEnterTransitionDidFinish() override final;
		virtual void onExit(void) override final;
		virtual void onExitTransitionDidStart() override final;

		virtual std::string getText() const override final;
		virtual void setText(const std::string& iText) override final;
		virtual void setFont(const std::string& pName, float iSize) override final;
		virtual void setBackgroundColor(const ccColor4B& iColor) override final;
		virtual void setTextColor(const ccColor4B& iColor) override final;
		virtual void retainFocus() override final;
		virtual void releaseFocus() override final;

	protected:
		virtual void _onShow() override final;
		virtual void _onHide() override final;

	private:
		CCMultilineTextBox* fOwner;
		GtkWidget* fWebView;
		mutable std::string fText;
		std::string fFontName;
		float fFontSize;
		ccColor4B fBackgroundColor;
		ccColor4B fTextColor;
		GObject* fTextArea;

	    void _attachBox();
	    void _detachBox();

	    void _createWebView();
	    static void _loadFinished(GtkWidget *view, const char *uri, gpointer user_data);
	    void _nativeGetText() const;
	    void _nativeSetText();
	    void _nativeSetFont();
	    void _nativeSetTextColor();
	    void _nativeSetBackgroundColor();

	    CCMultilineTextBoxDelegate* _delegate();
        void _sendEditingDidBegin();
        void _sendEditingDidEnd();
        void _sendTextChanged(const std::string& text);

        static gboolean _onKeyPress(GtkWidget* target, GObject* event, gpointer opaque);
        static gboolean _onKeyRelease(GtkWidget* target, GdkEventKey* event, gpointer opaque);
	};

NS_CC_EXT_END


