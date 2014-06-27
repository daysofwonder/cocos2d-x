//
//  DurationFormatter.h
//
//  Created by Gérald Guyomard on 17/05/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __DurationFormatter__
#define __DurationFormatter__

#include <chrono>
#include <string>
#include "platform/CCPlatformMacros.h"

NS_CC_BEGIN

    enum class TimeUnit : uint32_t
    {
        Seconds = 0,
        Minutes = 1,
        Hours = 2,
        Days = 3
    };
    
    class Duration
    {
    public:
        const TimeUnit      fUnit;
        const uint32_t      fValue;
        
        Duration(TimeUnit iUnit, uint32_t iValue)
        : fUnit(iUnit), fValue(iValue)
        {}
        
        std::chrono::seconds seconds() const;
        
        std::string toString(bool iShort = false) const;
    };
    
    class DurationFormatter
    {
    public:
        static std::string format(const std::chrono::seconds& iSeconds, bool iShort = false);
        
        enum FormatStrategy
        {
            shortFormat,
            shortFormatUnlessSingleUnit,
            longFormat,
        };
        static std::string formatWithPrecision(const std::chrono::seconds& iSeconds, FormatStrategy iStrategy = FormatStrategy::longFormat, uint32_t iNbUnits = 2);
        
        static Duration duration(const std::chrono::seconds& iSeconds);
    };

NS_CC_END

#endif /* defined(__DurationFormatter__) */
