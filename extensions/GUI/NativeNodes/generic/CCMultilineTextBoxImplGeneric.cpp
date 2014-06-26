//
#include "CCMultilineTextBox.h"
#include "CCMultilineTextBoxImpl.h"
#include "CCNativeViewImpl.h"

namespace swapp {
    
	class NoMultilineView : public CCNativeViewImpl, public CCMultilineTextBoxImpl
	{
	public:
		NoMultilineView() {}
		virtual ~NoMultilineView() {}

		virtual bool init(CCNode* iNode) override { return true; }
		virtual CCNode* owner() const override { return nullptr; }
		virtual void onEnter() override {}
		virtual void onEnterTransitionDidFinish() override {}
		virtual void onExit(void) override {}
		virtual void onExitTransitionDidStart() override {}

		virtual std::string getText() const override { return ""; }
		virtual void setText(const std::string& iText) override {}
		virtual void setFont(const std::string& pName, float iSize) override {}
		virtual void setBackgroundColor(const ccColor4B& iColor) override {}
		virtual void setTextColor(const ccColor4B& iColor) override {}
		virtual void retainFocus() override {}
		virtual void releaseFocus() override {}

	};

    std::unique_ptr<CCNativeViewImpl>
    CCMultilineTextBox::_createImpl()
    {
		return std::unique_ptr<CCNativeViewImpl>(new NoMultilineView());
    }
    
}

