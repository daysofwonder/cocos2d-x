//
//  CCNativeSmartLayer.h
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCNativeSmartLayer__
#define __CCNativeSmartLayer__

#include "layers_scenes_transitions_nodes/CCSmartLayer.h"

NS_CC_BEGIN
    
    class CCNativeViewImpl;
    class CCNativeSmartLayer : public CCSmartLayer
    {
    public:
        DOW_DECLARE_HIERARCHY(CCNativeSmartLayer, CCSmartLayer);
        static CCNativeSmartLayer* create();
        virtual ~CCNativeSmartLayer();

        // Overrides
        virtual void onEnter() override;
        virtual void onEnterTransitionDidFinish() override;

        virtual void onExit(void) override;
        virtual void onExitTransitionDidStart() override;

    protected:
        CCNativeSmartLayer();
        bool init();

        virtual std::unique_ptr<CCNativeViewImpl> _createImpl() = 0;

        CCNativeViewImpl* _impl() const;

    private:
        std::unique_ptr<CCNativeViewImpl>  fImpl;
    };

    extern CCNode* nodeWithKeyboardFocus();

NS_CC_END

#endif /* defined(__CCNativeSmartLayer__) */
