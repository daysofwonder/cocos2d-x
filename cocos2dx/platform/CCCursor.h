#ifndef __CCCURSOR_H__
#define __CCCURSOR_H__

#include "CCPlatformMacros.h"

NS_CC_BEGIN

class CC_DLL CCCursor
{
private:
    CCCursor();
public:

    static bool isSupported();
    
    static void showCurrentCursor();
    static void hideCurrentCursor();
};


NS_CC_END

#endif /* __CCCURSOR_H__ */
