//

#ifndef CCNATIVEVIEWIMPLLINUX_H_
#define CCNATIVEVIEWIMPLLINUX_H_

#include "layers_scenes_transitions_nodes/CCNativeViewImpl.h"
#include "cocoa/CCObjectPtr.h"
#include "cocoa/CCGeometry.h"
#include "base_nodes/CCNode.h"
#include <gtk/gtk.h>
#include "ExtensionMacros.h"

NS_CC_EXT_BEGIN

    class  CCNativeViewImplLinux : public CCNativeViewImpl {
    public:
        virtual bool init(cocos2d::CCNode* iNode);
        virtual cocos2d::CCNode* owner() const;
        virtual void onEnter() override;
        virtual void onExit(void) override;

    protected:
        CCNativeViewImplLinux();
        ~CCNativeViewImplLinux();

        void updateViewRect();
        void attachView(GtkWidget *iCountainee);
        void detachView();
        bool isAttached() const;

		virtual cocos2d::CCRect _getContentArea();
		virtual void _onShow();
		virtual void _onHide();

    private:
        class Updater;
        typedef cocos2d::CCObjectPtr<Updater> UpdaterPtr;

        cocos2d::CCNode* fOwner;
        GtkWidget *fContainee;
        cocos2d::CCRect fViewRect;
        UpdaterPtr fUpdater;
        bool fIsDiscarded;
        bool fIsVisible;

        void _updateNativeUIView();
    };

NS_CC_EXT_END

#endif
