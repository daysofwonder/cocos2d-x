//
//  CCYoutubeView.cpp
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCYoutubeView.h"
#include "CCYoutubeViewImpl.h"

#include "layers_scenes_transitions_nodes/CCNativeViewImpl.h"

NS_CC_EXT_BEGIN

    CCYoutubeView*
    CCYoutubeView::create()
    {
        CCYoutubeView* pRet = new CCYoutubeView();
        if (pRet && pRet->init())
        {
            pRet->autorelease();
        }
        else
        {
            CC_SAFE_DELETE(pRet);
        }
        return pRet;
    }

    CCYoutubeView::CCYoutubeView()
    {
    }

    CCYoutubeView::~CCYoutubeView()
    {
    }
        
    CCYoutubeViewImpl*
    CCYoutubeView::_youtubeViewImpl() const
    {
        CCYoutubeViewImpl* impl = dynamic_cast<CCYoutubeViewImpl*>(_impl());
        assert(impl != nullptr);
        return impl;
    }
    
    void
    CCYoutubeView::playMovie(const std::string& iMovieID)
    {
        _youtubeViewImpl()->playMovie(iMovieID);
    }

    void
    CCYoutubeViewLoader::onHandlePropTypeString(CCNode * pNode, CCNode * pParent, const char* pPropertyName, const char * pString, CCBReader * pCCBReader)
    {
        CCYoutubeView* view = static_cast<CCYoutubeView*>(pNode);
        assert(dynamic_cast<CCYoutubeView*>(pNode) != nullptr);

        /*if (::strcmp(pPropertyName, "backgroundColor") == 0)
        {
            view->setBackgroundColorFromPlainText(pString);
        }
        else*/
        {
            SuperClass::onHandlePropTypeString(pNode, pParent, pPropertyName, pString, pCCBReader);
        }
    }

NS_CC_EXT_END
