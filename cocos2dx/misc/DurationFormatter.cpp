//
//  DurationFormatter.cpp
//
//  Created by Gérald Guyomard on 17/05/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "misc/DurationFormatter.h"
#include "DOWFoundation/StringUtils.h"
#include "support/CCStringPool.h"

NS_CC_BEGIN

using namespace DOW;

    typedef std::chrono::duration<long, std::ratio<86400>> days;
    
    std::string
    Duration::toString(bool iShort) const
    {
        std::string format;
        
        switch (fUnit)
        {
            case TimeUnit::Days:
                format = CCLocalizedStdString(iShort ? "NbDays_ShortFormat" : "NbDays_Format");
                break;

            case TimeUnit::Hours:
                format = CCLocalizedStdString(iShort ? "NbHours_ShortFormat" : "NbHours_Format");
                break;

            case TimeUnit::Minutes:
                format = CCLocalizedStdString(iShort ? "NbMinutes_ShortFormat" : "NbMinutes_Format");
                break;
              
            case TimeUnit::Seconds:
            default:
                format = CCLocalizedStdString(iShort ? "NbSeconds_ShortFormat" : "NbSeconds_Format");
                break;
        }
        
        std::string label;
        StringUtils::format(label, format.c_str(), fValue);
        
        return label;
    }
    
    std::chrono::seconds
    Duration::seconds() const
    {
        switch (fUnit)
        {
            case TimeUnit::Days:
                return std::chrono::duration_cast<std::chrono::seconds>(days(fValue));
                
            case TimeUnit::Hours:
                return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::hours(fValue));
                
            case TimeUnit::Minutes:
                return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::minutes(fValue));
                
            case TimeUnit::Seconds:
                return std::chrono::seconds(fValue);
                
            default:
            {
                assert(false);
                return std::chrono::seconds(-1);
            }
        }
    }
    
    Duration
    DurationFormatter::duration(const std::chrono::seconds& iSeconds)
    {
        if (int nbDays = std::chrono::duration_cast<days>(iSeconds).count())
        {
            return Duration(TimeUnit::Days, nbDays);
        }
        else if (int nbHours = std::chrono::duration_cast<std::chrono::hours>(iSeconds).count())
        {
            return Duration(TimeUnit::Hours, nbHours);
        }
        else if (int nbMinutes = std::chrono::duration_cast<std::chrono::minutes>(iSeconds).count())
        {
            return Duration(TimeUnit::Minutes, nbMinutes);
        }
        else
        {
            return Duration(TimeUnit::Seconds, iSeconds.count());
        }
    }
    
    std::string
    DurationFormatter::format(const std::chrono::seconds& iSeconds, bool iShort)
    {
        Duration d = duration(iSeconds);
        return d.toString(iShort);
    }
    
    std::string
    DurationFormatter::formatWithPrecision(const std::chrono::seconds& iSeconds, FormatStrategy iStrategy, uint32_t iNbUnits)
    {
        std::chrono::seconds nbSeconds = iSeconds;
        
        const int nbDays = std::chrono::duration_cast<days>(nbSeconds).count();
        nbSeconds -= std::chrono::seconds(days(nbDays));
        
        const int nbHours = std::chrono::duration_cast<std::chrono::hours>(nbSeconds).count();
        nbSeconds -= std::chrono::seconds(std::chrono::hours(nbHours));
        
        const int nbMinutes = std::chrono::duration_cast<std::chrono::minutes>(nbSeconds).count();
        nbSeconds -= std::chrono::seconds(std::chrono::minutes(nbMinutes));
        
        int units[4];
        units[(int) TimeUnit::Days] = nbDays;
        units[(int) TimeUnit::Hours] = nbHours;
        units[(int) TimeUnit::Minutes] = nbMinutes;
        units[(int) TimeUnit::Seconds] = nbSeconds.count();
        
        if (iNbUnits == 0)
        {
            iNbUnits = 1;
        }
        else if (iNbUnits > 4)
        {
            iNbUnits = 4;
        }
        
        bool shortFormat;
        
        if (iStrategy == FormatStrategy::shortFormatUnlessSingleUnit)
        {
            int i = (int) TimeUnit::Days;
            uint32_t nbUnitsDone = 0;
            
            while (i >= 0)
            {
                if (units[i] != 0)
                {
                    if (++nbUnitsDone == iNbUnits)
                    {
                        break;
                    }
                }
                
                --i;
            }
            
            shortFormat = nbUnitsDone > 1;
        }
        else
        {
            shortFormat = (iStrategy == FormatStrategy::shortFormat);
        }
        
        std::string s;
        
        int i = (int) TimeUnit::Days;
        uint32_t nbUnitsDone = 0;
        
        while (i >= 0)
        {
            if (units[i] != 0)
            {
                if (!s.empty())
                {
                    s += ' ';
                }
                
                Duration duration((TimeUnit) i, units[i]);
                s += duration.toString(shortFormat);
                
                if (++nbUnitsDone == iNbUnits)
                {
                    break;
                }
            }
            
            --i;
        }
        
        return s;
    }

NS_CC_END
