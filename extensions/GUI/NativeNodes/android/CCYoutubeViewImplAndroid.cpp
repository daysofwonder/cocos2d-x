//
//  CCYoutubeViewImplAndroid.cpp
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "GUI/NativeNodes/android/CCYoutubeViewImplAndroid.h"
#include "GUI/NativeNodes/CCYoutubeView.h"
#include "DOWFoundation/StringUtils.h"

NS_CC_EXT_BEGIN

    std::unique_ptr<CCNativeViewImpl>
    CCYoutubeView::_createImpl()
    {
        std::unique_ptr<CCNativeViewImpl> impl(new CCYoutubeViewImplAndroid);
        return impl;
    }
    
    CCYoutubeViewImplAndroid::CCYoutubeViewImplAndroid()
    {
    }
    
    CCYoutubeViewImplAndroid::~CCYoutubeViewImplAndroid()
    {
    }
    
    static const std::string kYoutubeViewControllerJavaClassName = "com/daysofwonder/smallworld2/YoutubeViewController";

    std::string
    CCYoutubeViewImplAndroid::_nativeViewControllerFullName()
    {
        return kYoutubeViewControllerJavaClassName;
    }

    void
    CCYoutubeViewImplAndroid::playMovie(const std::string& iMovieID)
    {
    	auto controller = nativeViewController();
    	assert(controller != nullptr);

    	auto& manager = JNIManager::instance();

        auto method = manager.getMethodInfo(kYoutubeViewControllerJavaClassName, "playMovie", "(Ljava/lang/String;)V");

        if (method != nullptr)
        {
            auto str = manager.string2jstring(iMovieID);
            jstring s = str->javaObject();

             method->callVoidMethod(controller, s);
        }
    }

    void
    CCYoutubeView::playMovieInSeparateActivity(const std::string& iMovieID)
    {
       	auto& manager = JNIManager::instance();

            auto method = manager.getStaticMethodInfo(kYoutubeViewControllerJavaClassName, "playMovieInSeparateActivity", "(Ljava/lang/String;)V");

            if (method != nullptr)
            {
                auto str = manager.string2jstring(iMovieID);
                jstring s = str->javaObject();

                 method->callStaticVoidMethod(1, s);
            }
    }

NS_CC_EXT_END

