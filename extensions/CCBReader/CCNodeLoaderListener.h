#ifndef _CCB_CCNODELOADERLISTENER_H_
#define _CCB_CCNODELOADERLISTENER_H_

#include "cocos2d.h"

NS_CC_EXT_BEGIN

class CCNodeLoader;

class CCNodeLoaderListener {
    public:
        virtual ~CCNodeLoaderListener() {};

        virtual void onNodeLoaded(CCNode * pNode, CCNodeLoader * pNodeLoader) {}
    
        virtual void onChildPositionParsed(CCNode * pNode, CCNode* pChild, const CCPoint& pos, int ntype, const CCSize& containerSize, const char* pPropertyName) {}
        virtual void onChildSizeParsed(CCNode * pNode, CCNode* pChild, const CCSize& size, int ntype, const CCSize& containerSize) {}
};

NS_CC_EXT_END

#endif
