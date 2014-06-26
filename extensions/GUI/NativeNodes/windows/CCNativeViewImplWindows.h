//

#ifndef __CCNativeViewImplWindows__
#define __CCNativeViewImplWindows__

#include <cocos2d.h>
#include "layers_scenes_transitions_nodes/CCNativeViewImpl.h"

#include <atlbase.h> 
#include <atlwin.h>
#include <functional>

#include "ExtensionMacros.h"

NS_CC_EXT_BEGIN

    class CCNativeViewImplWindows : public CCNativeViewImpl
    {
    public:
		CCNativeViewImplWindows();
		virtual ~CCNativeViewImplWindows();
        
        virtual bool init(CCNode* iNode) override;
        
        virtual CCNode* owner() const override { return fOwner; }

        virtual void onEnter() override;
        virtual void onEnterTransitionDidFinish() override;
        
        virtual void onExit(void) override;
        virtual void onExitTransitionDidStart() override;

		static bool processAccelerator(LPMSG lpmsg);
		typedef std::function<bool(LPMSG lpmsg)> Accelerator;
		static void setAccelerator(Accelerator iAccelerator);

    protected:
        virtual void _updateNativeUIView();

        virtual void _onEnter();
        virtual void _onExit();

		CAxWindow& uiView() const { return fAxWindow; }

		virtual void _createUIView() = 0;
		virtual CCRect getContentArea();
		virtual bool isVisible() const;


    private:
        CCNode*             fOwner;
		mutable CAxWindow   fAxWindow;
        bool                fIsDiscarded;
		RECT				fBoundsRect;
		int					fZoomPercent;
        class Updater;
        CCObjectPtr<Updater> fUpdater;
    };

NS_CC_EXT_END

#endif

