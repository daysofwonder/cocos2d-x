//
//  CCNativeViewImplAndroid.h
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCNativeViewImplAndroid__
#define __CCNativeViewImplAndroid__

#include "layers_scenes_transitions_nodes/CCNativeViewImpl.h"
#include "../DOWFoundationImpl/Android/JNIManager.h"
#include "ExtensionMacros.h"
#include "cocoa/CCGeometry.h"
#include "cocoa/CCObjectPtr.h"
#include <string>
#include "DOWFoundation/DOWFoundation.h"

NS_CC_EXT_BEGIN

    using namespace DOW;

    class CCNativeViewImplAndroid : public CCNativeViewImpl
    {
    public:
        CCNativeViewImplAndroid();
        virtual ~CCNativeViewImplAndroid();
        
        virtual bool init(CCNode* iNode) override;
        
        virtual CCNode* owner() const override { return fOwner; }

        virtual void onEnter() override;
        virtual void onEnterTransitionDidFinish() override;
        
        virtual void onExit(void) override;
        virtual void onExitTransitionDidStart() override;

        jobject nativeViewController() const { return (fNativeViewController != nullptr) ? fNativeViewController->javaObject() : nullptr; }
        
    protected:
        virtual std::string _nativeViewControllerFullName() = 0;
        JavaObject::Ptr _createNativeViewController();
        virtual void _updateNativeUIView();

        virtual void _onEnter();
        virtual void _onExit();

        void _addToSuperview();
        void _removeFromSuperview();
        void _setVisible(bool iVisible);
        void _setTransform(float iRotation, const CCRect& iBoundingBox);

        // returns false if invisible
        bool _computeTransform(float& oRotation, CCRect& oBoundingBox) const;

        CCNode*             fOwner;
        JavaObject::Ptr     fNativeViewController;
        bool                fIsDiscarded;
        
        class Updater;
        CCObjectPtr<Updater> fUpdater;
    };

NS_CC_EXT_END

#endif /* defined(__CCNativeViewImplAndroid__) */
