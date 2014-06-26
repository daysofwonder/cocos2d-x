//
//  CCYoutubeViewImplAndroid.h
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCYoutubeViewImplAndroid__
#define __CCYoutubeViewImplAndroid__

#include "GUI/NativeNodes/android/CCNativeViewImplAndroid.h"
#include "GUI/NativeNodes/CCYoutubeViewImpl.h"
#include <atomic>

NS_CC_EXT_BEGIN

    class CCYoutubeView;
    
    class CCYoutubeViewImplAndroid : public CCNativeViewImplAndroid, public CCYoutubeViewImpl
    {
    public:
        DOW_DECLARE_HIERARCHY(CCYoutubeViewImplAndroid, CCNativeViewImplAndroid);
        
        CCYoutubeViewImplAndroid();
        virtual ~CCYoutubeViewImplAndroid();

        virtual void playMovie(const std::string& iMovieID) override;

    private:
        virtual std::string _nativeViewControllerFullName() override;
    };

NS_CC_EXT_END

#endif /* defined(__CCYoutubeViewImplAndroid__) */

