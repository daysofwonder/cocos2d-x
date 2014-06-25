//
//  CCEnhancedTableView.h
//
//  Created by Gérald Guyomard on 5/12/14.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCEnhancedTableView__
#define __CCEnhancedTableView__

#include "GUI/CCScrollView/CCTableView.h"
#include "CCBReader/CCTableViewLoader.h"

NS_CC_BEGIN
    class CCTooltip;
NS_CC_END

NS_CC_EXT_BEGIN
    
    class CCEnhancedTableView : public CCTableView
    {
    public:
        virtual bool canHandleMouseOver() override;
        virtual bool isMouseOver(const CCPoint& iWorldMouseLocation) override;
        virtual void onMouseEnter(const CCPoint& iWorldMouseLocation) override;
        virtual void onMouseOver(const CCPoint& iWorldMouseLocation) override;
        virtual void onMouseExit(const CCPoint& iWorldMouseLocation) override;
        
        void setTooltipText(const std::string& iText);
        void setTooltipLocalizedTextKey(const std::string& iKey);
        
    protected:
        CCObjectPtr<CCTooltip> fTooltip;
    };
    
    class CCEnhancedTableViewLoader : public CCTableViewLoader
    {
    public:
        DOW_DECLARE_HIERARCHY(CCEnhancedTableViewLoader, CCTableViewLoader);
        
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCEnhancedTableViewLoader, loader);
        
    protected:
        virtual CCEnhancedTableView* createCCNode(CCNode * pParent, CCBReader * pCCBReader) override;
        
        virtual void onHandlePropTypeString(CCNode * pNode, CCNode * pParent, const char* pPropertyName, const char * pString, CCBReader * pCCBReader) override;
    };

NS_CC_EXT_END

#endif /* defined(__CCEnhancedTableView__) */
