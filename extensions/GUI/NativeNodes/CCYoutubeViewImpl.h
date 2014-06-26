//
//  CCYoutubeViewImpl.h
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCYoutubeViewImpl__
#define __CCYoutubeViewImpl__

#include "ExtensionMacros.h"
#include "DOWFoundation/Url.h"

NS_CC_EXT_BEGIN

    class CCYoutubeView;
    class CCYoutubeViewImpl
    {
    public:
        virtual ~CCYoutubeViewImpl() {}

        virtual void playMovie(const std::string& iMovieID) = 0;
    };

NS_CC_EXT_END

#endif /* defined(__CCYoutubeViewImpl__) */
