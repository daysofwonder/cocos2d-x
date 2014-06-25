//
//  CCTooltip
//
//  Created by Gérald Guyomard on 01/09/14.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCTooltip__
#define __CCTooltip__

#include <memory>
#include <string>
#include "cocoa/CCObjectPtr.h"
#include "DOWFoundation/DOWFoundation.h"
#include "cocoa/CCGeometry.h"

NS_CC_BEGIN

class CCTooltip : public CCObject
    {
    public:
        DOW_DECLARE_HIERARCHY(CCTooltip, CCObject);
        typedef CCObjectPtr<CCTooltip> Ptr;
        virtual ~CCTooltip();
        
        void terminate();
        
        static void hideCurrentTooltip();
        
        virtual void onMouseEnter(const CCPoint& iWorldMouseLocation);
        virtual void onMouseOver(const CCPoint& iWorldMouseLocation);
        virtual void onMouseExit(const CCPoint& iWorldMouseLocation);
        
    protected:
        CCTooltip() {}
        
        virtual void _show(const CCPoint& iWorldMouseLocation) = 0;
        virtual void _hide() = 0;
        
    private:
        void _onMouseStill(float iDt);
        
        CCPoint fLastWorldMousePosition;
    };
    
    class CCTextTooltip : public CCTooltip
    {
    public:
        DOW_DECLARE_HIERARCHY(CCTextTooltip, CCTooltip);
        virtual ~CCTextTooltip();
        
        static Ptr create(const std::string& iText);
        static Ptr createWithLocalizedTextKey(const std::string& iKey);
        
        void setText(const std::string& iText);
        
    protected:
        CCTextTooltip();
        
        virtual void _show(const CCPoint& iWorldMouseLocation) override;
        virtual void _hide() override;
        
    private:
        class Controller;
        CCObjectPtr<Controller> fController;
        std::string fText;
    };

NS_CC_END


#endif /* defined(__CCTooltip__) */
