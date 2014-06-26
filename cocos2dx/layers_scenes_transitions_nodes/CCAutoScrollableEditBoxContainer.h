//
//  CCAutoScrollableEditBoxContainer.h
//
//  Created by Gérald Guyomard on 28/01/13.

// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCAutoScrollableEditBoxContainer__
#define __CCAutoScrollableEditBoxContainer__

#include "layers_scenes_transitions_nodes/CCSmartLayer.h"
#include "CCBReader/CCSmartLayerLoader.h"

NS_CC_BEGIN

    class CCAutoScrollableEditBoxContainer : public CCSmartLayer, public CCIMEDelegate
    {
    public:
        DOW_DECLARE_HIERARCHY(CCAutoScrollableEditBoxContainer, CCSmartLayer);
        CCAutoScrollableEditBoxContainer();
        static CCAutoScrollableEditBoxContainer* create();

        virtual void keyboardWillShow(CCIMEKeyboardNotificationInfo& info) override;
        virtual void keyboardDidShow(CCIMEKeyboardNotificationInfo& info)  override;
        virtual void keyboardWillHide(CCIMEKeyboardNotificationInfo& info) override;
        virtual void keyboardDidHide(CCIMEKeyboardNotificationInfo& info) override;

        virtual void onEnter() override;
        virtual void onExit() override;
        
        float distanceFromKeyboard() const { return fDistanceFromKeyboard; }
        void setDistanceFromKeyboard(float iDistance) { fDistanceFromKeyboard = iDistance; }
        
    private:
        CCPoint fSavedPosition;
        bool    fScrolledUp;
        
        float   fDistanceFromKeyboard;
    };

NS_CC_END

NS_CC_EXT_BEGIN

    class CCAutoScrollableEditBoxContainerLoader : public CCSmartLayerLoader
    {
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCAutoScrollableEditBoxContainerLoader, loader);
        
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CCAutoScrollableEditBoxContainer);
        
        virtual void onHandlePropTypeFloat(CCNode * pNode, CCNode * pParent, const char* pPropertyName, float pFloat, CCBReader * pCCBReader) override;
    };

NS_CC_EXT_END

#endif /* defined(__CCAutoScrollableEditBoxContainer__) */
