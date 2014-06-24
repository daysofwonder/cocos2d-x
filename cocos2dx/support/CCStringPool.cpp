//
//  CCStringPool.cpp
//
//  Created by Gérald Guyomard on 12/10/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCStringPool.h"
#include "platform/CCSAXParser.h"
#include "ccMacros.h"
#include "CCArray.h"
#include "CCString.h"
#include "CCFileUtils.h"
#include <cassert>

NS_CC_BEGIN

const CCStringPool&
CCStringPool::sharedLocalizedStringPool()
{
    static CCStringPool* s_Pool =  NULL;
    if (s_Pool == NULL)
    {
        s_Pool = new CCStringPool;
        if (!s_Pool->loadFromFile("localizedStrings.xml"))
        {
            CCAssert(false, "failed when loading localized strings");
        }
    }

    return *s_Pool;
}

CCStringPool::CCStringPool()
: fSubPools(NULL)
{
}

CCStringPool::~CCStringPool()
{
    CC_SAFE_RELEASE(fSubPools);
}

std::string
CCStringPool::stdStringForKey(const std::string& iKey) const
{
    const std::string* value = stdStringForKeyIfFound(iKey);
    if (value != NULL)
    {
        return *value;
    }
    else
    {
        CCLOG("String not found");
        return "";
    }
}

std::string
CCStringPool::stdStringForKey(const std::string& iKey, const std::string& iDefaultValue) const
{
    const std::string* value = stdStringForKeyIfFound(iKey);
    if (value != NULL)
    {
        return *value;
    }
    else
    {
        return iDefaultValue;
    }
}

const std::string*
CCStringPool::stdStringForKeyIfFound(const std::string& iKey) const
{
    const TKeyToValue::const_iterator it = fKeyToValue.find(iKey);
    if (it != fKeyToValue.end())
    {
        return &(it->second);
    }
    else
    {
        // Try in sub pools
        CCObject* obj;
        CCARRAY_FOREACH(fSubPools, obj)
        {
            CCStringPool* pool = (CCStringPool*) obj;
            const std::string* s = pool->stdStringForKeyIfFound(iKey);
            if (s != NULL)
            {
                return s;
            }
        }

        return NULL;
    }
}

CCString*
CCStringPool::stringForKey(const std::string& iKey) const
{
    const std::string* s = stdStringForKeyIfFound(iKey);
    if (s == NULL)
    {
        return NULL;
    }

    return CCString::create(*s);
}

void
CCStringPool::addString(const std::string& iKey, const std::string& iValue)
{
    fKeyToValue[iKey] = iValue;
}

void
CCStringPool::addSubPool(CCStringPool* iPool)
{
    if (fSubPools == NULL)
    {
        fSubPools = new CCArray;
    }

    fSubPools->addObject(iPool);
}

class CCStringPoolParser : public CCSAXDelegator
{
public:
    CCStringPoolParser(CCStringPool& iPool);

    bool parse(const char* iPath);

    virtual void startElement(void *ctx, const char *name, const char **atts);
    virtual void endElement(void *ctx, const char *name);
    virtual void textHandler(void *ctx, const char *s, int len);

private:

    void _setFailure(const std::string& iErrorMessage);

    CCStringPool& fPool;
    bool          fParsingPool;
    std::string   fCurrentKey;
    std::string   fCurrentValue;

    bool          fSucceeded;
    std::string   fErrorMessage;
};

CCStringPoolParser::CCStringPoolParser(CCStringPool& iPool)
: fPool(iPool), fSucceeded(false), fParsingPool(false)
{}

void
CCStringPoolParser::_setFailure(const std::string& iErrorMessage)
{
    fSucceeded = false;
    fErrorMessage = iErrorMessage;
}

bool
CCStringPoolParser::parse(const char* iPath)
{
    CCSAXParser parser;
    parser.setDelegator(this);

    fSucceeded = true;
    parser.parse(iPath);

    return fSucceeded;
}

static const char* kPoolElementName = "strings";
static const char* kStringElementName = "string";
static const char* kKeyAttributeName = "key";
static const char* kStringPoolElementName = "subPool";
static const char* kStringPoolFileAttributeName = "name";

static inline bool _areAttributesEmpty(const char** pAttrs)
{
    return (pAttrs == NULL) || (*pAttrs == NULL);
}

void
CCStringPoolParser::startElement(void *ctx, const char *name, const char **atts)
{
    if (!fSucceeded)
    {
        return;
    }

    if (fParsingPool)
    {
        // Should be string
        if (strcmp(name, kStringElementName) == 0)
        {
            // Should have one attribute key = ""
            if (_areAttributesEmpty(atts))
            {
                _setFailure("No attributes found for string");
            }
            else
            {
                if (_areAttributesEmpty(atts))
                {
                    _setFailure("could not read first attribute of strings");
                    return;
                }

                const char* attrName = *(atts++);
                if (strcmp(attrName, kKeyAttributeName) != 0)
                {
                    _setFailure("attribute is not key");
                    return;
                }

                const char* key = *atts;
                if ((key == NULL) || (strlen(key) == 0))
                {
                    _setFailure("invalid value for key");
                    return;
                }

                fCurrentKey = key;
                fCurrentValue.erase();
            }
        }
        else if (strcmp(name, kStringPoolElementName) == 0)
        {
            if (_areAttributesEmpty(atts))
            {
                _setFailure("could not read first attribute of stringPool");
                return;
            }

            const char* attrName = *(atts++);
            if (strcmp(attrName, kStringPoolFileAttributeName) != 0)
            {
                _setFailure("attribute is not name");
                return;
            }

            const char* name = *atts;
            if ((name == NULL) || (strlen(name) == 0))
            {
                _setFailure("name is empty");
                return;
            }

            CCStringPool* pool = new CCStringPool;
            if (!pool->loadFromFile(name))
            {
                CC_SAFE_RELEASE(pool);
                _setFailure("could not read subpool");
                return;
            }

            fPool.addSubPool(pool);
            CC_SAFE_RELEASE(pool);
        }
        else
        {
            _setFailure("unknown tag");
        }
    }
    else
    {
        // Should be root element
        if (!fParsingPool && strcmp(name, kPoolElementName) == 0)
        {
            if (_areAttributesEmpty(atts))
            {
                fParsingPool = true;
            }
            else
            {
                _setFailure("There should be no attributes in element <strings>");
            }
        }
        else
        {
            _setFailure("xml root element is not strings");
        }
    }
}

void
CCStringPoolParser::endElement(void *ctx, const char *name)
{
    if (!fSucceeded)
    {
        return;
    }

    if (strcmp(name, kStringElementName) == 0)
    {
        assert(!fCurrentKey.empty());

        fPool.addString(fCurrentKey, fCurrentValue);
        fCurrentKey.clear();
        fCurrentValue.clear();
    }
    else if (strcmp(name, kPoolElementName) == 0)
    {
        fParsingPool = false;
    }
}

void
CCStringPoolParser::textHandler(void *ctx, const char *s, int len)
{
    if (!fSucceeded)
    {
        return;
    }

    if (!fCurrentKey.empty())
    {
        std::string v(s, len);
        fCurrentValue += v;
    }
}

bool
CCStringPool::loadFromFile(const std::string& iPath)
{
    fPath = iPath;

    CCStringPoolParser parser(*this);


    std::string fullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(fPath.c_str());

    return parser.parse(fullPath.c_str());
}

NS_CC_END
