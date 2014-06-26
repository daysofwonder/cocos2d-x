//
//  CCYoutubeView.h
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCYoutubeView__
#define __CCYoutubeView__

#include "layers_scenes_transitions_nodes/CCNativeSmartLayer.h"
#include "CCBReader/CCSmartLayerLoader.h"

NS_CC_EXT_BEGIN

    class CCYoutubeViewImpl;    
    class CCYoutubeView : public CCNativeSmartLayer
    {
    public:
        DOW_DECLARE_HIERARCHY(CCYoutubeView, CCNativeSmartLayer);
        static CCYoutubeView* create();
        virtual ~CCYoutubeView();

        void playMovie(const std::string& iMovieID);
        
#if __ANDROID__
        static void playMovieInSeparateActivity(const std::string& iMovieID);
#endif
        
    protected:
        CCYoutubeView();
        
        virtual std::unique_ptr<CCNativeViewImpl> _createImpl() override;
        CCYoutubeViewImpl* _youtubeViewImpl() const;        
    };
        
    class CCYoutubeViewLoader : public CCSmartLayerLoader
    {
    public:
        DOW_DECLARE_HIERARCHY(CCYoutubeViewLoader, CCSmartLayerLoader);
        
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCYoutubeViewLoader, loader);
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CCYoutubeView);
        
        virtual void onHandlePropTypeString(CCNode * pNode, CCNode * pParent, const char* pPropertyName, const char * pString, CCBReader * pCCBReader) override;
    };

NS_CC_EXT_END

#endif /* defined(__CCYoutubeView__) */
