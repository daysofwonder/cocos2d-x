//
//  CCStringPool.h
//
//  Created by Gérald Guyomard on 12/10/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCStringPool__
#define __CCStringPool__

#include <map>
#include "cocoa/CCObject.h"

NS_CC_BEGIN

class CCStringPool : public CCObject
{
public:

    static const CCStringPool& sharedLocalizedStringPool();

    CCStringPool();
    ~CCStringPool();

    std::string stdStringForKey(const std::string& iKey) const;
    std::string stdStringForKey(const std::string& iKey, const std::string& iDefaultValue) const;
    CCString* stringForKey(const std::string& iKey) const;

    const std::string* stdStringForKeyIfFound(const std::string& iKey) const;

    void addString(const std::string& iKey, const std::string& iValue);
    void addSubPool(CCStringPool* iPool);

    bool loadFromFile(const std::string& iPath);

protected:
    std::string fPath;
    typedef std::map<std::string /*key*/, std::string /*value*/> TKeyToValue;
    TKeyToValue fKeyToValue;

    CCArray* fSubPools;
};

NS_CC_END

#define CCLocalizedString(key) cocos2d::CCStringPool::sharedLocalizedStringPool().stringForKey(key)
#define CCLocalizedStdString(key) cocos2d::CCStringPool::sharedLocalizedStringPool().stdStringForKey(key)
#define CCLocalizedStdStringWithDefaultValue(key, defaultValue) cocos2d::CCStringPool::sharedLocalizedStringPool().stdStringForKey(key, defaultValue)

#endif /* defined(__SmallWorld2__CCStringPool__) */
