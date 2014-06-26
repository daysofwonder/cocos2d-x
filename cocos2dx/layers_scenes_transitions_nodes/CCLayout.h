//
//  CCLayout.h
//
//
// Copyright (c) 2013 Days of Wonder Inc.
//

#ifndef __CCLayout__
#define __CCLayout__

#include "layers_scenes_transitions_nodes/CCSmartLayer.h"
#include <memory>
#include <map>
#include "CCBReader/CCSmartLayerLoader.h"

NS_CC_BEGIN

    using namespace extension;

    class CCLayout : public CCSmartLayer
    {
    public:
        DOW_DECLARE_HIERARCHY(CCLayout, CCSmartLayer);
        
        static CCLayout* create();
        virtual ~CCLayout();
        
        // From CCNode
        virtual void setContentSize(const CCSize& iNewSize) override;
        virtual void visit() override;

        virtual void onEnter() override;
        virtual void onExit(void) override;

        virtual void update(float dT) override;
        
        // From CCNodeLoaderListener
        virtual void onChildPositionParsed(CCNode * pNode, CCNode* pChild, const CCPoint& pos, CCBPositionType ntype, const CCSize& containerSize, const char* pPropertyName) override;
        virtual void onChildSizeParsed(CCNode * pNode, CCNode* pChild, const CCSize& size, CCBSizeType ntype, const CCSize& containerSize) override;
        
        enum EAutoScaleMode
        {
            eNoAutoScale,
            eAutoScaleToRevealAll,
            eAutoScaleWithNoBorder
        };
        
        void setAutoScaleMode(EAutoScaleMode iMode);
		void setPositionAttributes(const CCPoint& iPosition, CCBPositionType nType);
        void setSizeAttributes(const CCSize& iSize, CCBSizeType nType);
        void setSiblingTagToFitScale(int iSiblingTag); // < 0 to disable
        void setResizeToFitParentBounds(bool iResize);
        
        EAutoScaleMode childAutoScaleMode(int iChildTag) const;
        void setChildAutoScaleMode(EAutoScaleMode iMode, int iChildTag);
        
		void setChildPositionAttributes(CCNode* iChild, const CCPoint& iPosition, CCBPositionType ntype);
		void setChildSizeAttributes(CCNode* iChild, const CCSize& iSize, CCBSizeType ntype);

        void needsLayout();
        
    private:
        friend class CCLayoutLoader;
        CCLayout();
        
        void _registerUpdate();
        void _unregisterUpdate();
        
        void _layoutIfNeeded();
        void _layout();
        void _layoutNode(CCNode* iNode);

        void _setPositionAttributes(CCNode* iNode, const CCPoint& iPosition, CCBPositionType nType);
        void _setSizeAttributes(CCNode* iNode, const CCSize& iSize, CCBSizeType nType);

        CCSize         fContainerSize;

        struct PositionAttributes
        {
            CCPoint			fPosition;
			CCBPositionType fType;
            
            PositionAttributes();
            
            typedef std::unique_ptr<PositionAttributes> Ptr;
        };

        PositionAttributes* _positionAttributes(const CCNode* iNode);
        PositionAttributes& _createPositionAttributes(const CCNode* iNode);
        
        struct SizeAttributes
        {
            CCSize			fSize;
            CCBSizeType     fType;
            
            SizeAttributes();
            
            typedef std::unique_ptr<SizeAttributes> Ptr;

        };
        
        SizeAttributes* _sizeAttributes(const CCNode* iNode);
        SizeAttributes& _createSizeAttributes(const CCNode* iNode);

        struct Attributes
        {
            PositionAttributes::Ptr fPosAttrs;
            SizeAttributes::Ptr     fSizeAttrs;
            
            typedef std::unique_ptr<Attributes> Ptr;
            
            Attributes();
        };
        
        Attributes* _attributes(const CCNode* iNode);
        Attributes& _createAttributes(const CCNode* iNode);
        
        typedef std::map<const CCNode*, Attributes::Ptr> TNodeToAttributes;
        TNodeToAttributes fNodeToAttributes;
        
        typedef std::map<int, EAutoScaleMode> TChildTagToAutoScaleMode;
        TChildTagToAutoScaleMode fChildTagToAutoScaleMode;
        EAutoScaleMode           fAutoScaleMode;
        int                      fSiblingTagToFitScale;

		union
		{
			struct
			{
				uint32_t fResizeToFitParentBounds : 1;
				uint32_t fLayoutInProgress : 1;
				uint32_t fContentSizeLocked : 1;
			};

			uint32_t fPackedFlags;
		};
    };

NS_CC_END

NS_CC_EXT_BEGIN
    class CCLayoutLoader : public CCSmartLayerLoader
    {
    public:
        DOW_DECLARE_HIERARCHY(CCLayoutLoader, CCSmartLayerLoader);
        
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCLayoutLoader, loader);
        
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CCLayout);
        
        virtual void onHandlePropTypeString(CCNode * pNode, CCNode * pParent, const char* pPropertyName, const char * pString, CCBReader * pCCBReader) override;
        virtual void onHandlePropTypeInteger(CCNode * pNode, CCNode * pParent, const char* pPropertyName, int pInteger, CCBReader * pCCBReader) override;
        virtual void onHandlePropTypeCheck(CCNode * pNode, CCNode * pParent, const char* pPropertyName, bool pCheck, CCBReader * pCCBReader) override;

        virtual CCPoint processPropTypePosition(CCNode* pNode, CCNode* pParent, const CCPoint& pos, int ntype, const CCSize& containerSize, const char* pPropertyName) override;
        virtual CCSize processPropTypeSize(CCNode* pNode, CCNode* pParent, const CCSize& size, int ntype, const CCSize& containerSize) override;
    };
NS_CC_EXT_END

#endif /* defined(__CCLayout__) */
