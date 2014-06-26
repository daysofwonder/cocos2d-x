//
//  CCMultilineTextBox.h
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCMultilineTextBox__
#define __CCMultilineTextBox__

#include "layers_scenes_transitions_nodes/CCNativeSmartLayer.h"
#include "CCBReader/CCSmartLayerLoader.h"

NS_CC_EXT_BEGIN

    class CCMultilineTextBoxImpl;
    class CCMultilineTextBoxDelegate;
    
    class CCMultilineTextBox : public CCNativeSmartLayer
    {
    public:
        DOW_DECLARE_HIERARCHY(CCMultilineTextBox, CCNativeSmartLayer);
        static CCMultilineTextBox* create();
        virtual ~CCMultilineTextBox();
        
        std::string getText() const;
        void setText(const std::string& iText);
    
        void setFont(const std::string& pName, float iSize);
        
        // FontName, size
        void setFontAsString(const std::string& iFontAsString);
        
        void setBackgroundColor(const ccColor4B& iColor);
        void setTextColor(const ccColor4B& iColor);
        
        void retainFocus();
        void releaseFocus();
        
        CCMultilineTextBoxDelegate* delegate() const { return fDelegate; }
        void setDelegate(CCMultilineTextBoxDelegate* iDelegate) { fDelegate = iDelegate; }
        
        // Helpers
        void setBackgroundColorAsString(const std::string& iColorAsString);
        void setTextColorAsString(const std::string& iColorAsString);
        
    protected:
        CCMultilineTextBox();
        
        virtual std::unique_ptr<CCNativeViewImpl> _createImpl() override;
        CCMultilineTextBoxImpl* _multiLineTextBoxImpl() const;
        
        CCMultilineTextBoxDelegate* fDelegate;
    };
    
    class CCMultilineTextBoxDelegate
    {
    public:
        virtual ~CCMultilineTextBoxDelegate() {}
        
        /**
         * This method is called when an edit box gains focus after keyboard is shown.
         * @param editBox The edit box object that generated the event.
         */
        virtual void textBoxEditingDidBegin(CCMultilineTextBox* editBox) {}
        
        
        /**
         * This method is called when an edit box loses focus after keyboard is hidden.
         * @param editBox The edit box object that generated the event.
         */
        virtual void textBoxEditingDidEnd(CCMultilineTextBox* editBox) {}
        
        /**
         * This method is called when the edit box text was changed.
         * @param editBox The edit box object that generated the event.
         * @param text The new text.
         */
        virtual void textBoxTextChanged(CCMultilineTextBox* editBox, const std::string& text) {}
    };
    
    class CCMultilineTextBoxLoader : public CCSmartLayerLoader
    {
    public:
        DOW_DECLARE_HIERARCHY(CCMultilineTextBoxLoader, CCSmartLayerLoader);
        
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCMultilineTextBoxLoader, loader);
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CCMultilineTextBox);
        
        virtual void onHandlePropTypeString(CCNode * pNode, CCNode * pParent, const char* pPropertyName, const char * pString, CCBReader * pCCBReader) override;
    };

NS_CC_EXT_END

#endif /* defined(__CCMultilineTextBox__) */
