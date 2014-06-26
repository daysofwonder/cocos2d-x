//
//  CCSpinner.h
//
//  Created by Gérald Guyomard on 22/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCSpinner__
#define __CCSpinner__

#include "layers_scenes_transitions_nodes/CCSmartLayer.h"
#include "CCBReader/CCNodeLoaderListener.h"
#include "CCBReader/CCSmartLayerLoader.h"
#include "ExtensionMacros.h"
#include "DOWFoundation/DOWFoundation.h"

NS_CC_EXT_BEGIN

    class CCSpinner : public CCSmartLayer
    {
    public:
        DOW_DECLARE_HIERARCHY(CCSpinner, CCSmartLayer);
        
        static CCSpinner* create();
        
        virtual void setVisible(bool iVisible) override;
        
        bool isAnimating() const;
        void startAnimating();
        void stopAnimating();

        // Defaults true
        bool autoAnimatesWhenVisible() const { return fAutoAnimateWhenVisible; }
        void setAutoAnimateWhenVisible(bool iAuto) { fAutoAnimateWhenVisible = iAuto; }
        
    private:
        CCSpinner();
        
        virtual void onNodeLoaded(CCNode * pNode, CCNodeLoader * pNodeLoader) override;
        
        class Content;
        CCObjectPtr<Content> fContent;
        bool fAutoAnimateWhenVisible;

    };
    
    class CCSpinnerLoader : public CCSmartLayerLoader
    {
    public:
        DOW_DECLARE_HIERARCHY(CCSpinnerLoader, CCSmartLayerLoader);
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCSpinnerLoader, loader);
        
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CCSpinner);
        
        virtual void onHandlePropTypeCheck(CCNode * pNode, CCNode * pParent, const char * pPropertyName, bool pCheck, CCBReader * pCCBReader) override;
    };

NS_CC_EXT_END

#endif /* defined(__CCSpinner__) */
