//
//  NodeController.h
//
//  Created by Gérald Guyomard on 09/10/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __NodeController__
#define __NodeController__

#include "cocoa/CCObjectPtr.h"
#include "NodeControllers/NodeLoadingFacilities.h"
#include "DOWFoundation/DOWFoundation.h"

NS_CC_EXT_BEGIN

    class NodeController;
    typedef CCObjectPtr<NodeController> NodeControllerPtr;
    
    class NodeController : public CCObject, public NodeLoadingFacilities
    {
    public:
        DOW_DECLARE_HIERARCHY(NodeController, CCObject);
        virtual ~NodeController();
        
		void release(void); // Non virtual override of CCObject

        static NodeControllerPtr createFromResource(const std::string& iResourceName);
        
        virtual CCObject* owner();
                
        CCNode* rootNode() const { return fRootNode; }
        
        // When loaded as a sub controller
        void preLoad(Resolvers& ioResolvers);
        void postLoad(CCNode* iRootNode);
        void postLoad();
        
        // Schedule
        CCScheduler* scheduler() const;
        
        void schedule(SEL_SCHEDULE selector, float interval, unsigned int repeat, float delay);
        void scheduleOnce(SEL_SCHEDULE selector, float delay);
        void unschedule(SEL_SCHEDULE selector);
        
    protected:
        NodeController();

        // To be overridden by subclasses
        virtual void _didLoad();
        
        CCObjectPtr<CCNode> fRootNode;
        
    private:
        
         virtual void _postLoad(CCNode* iLoadedRootNode);
    };


NS_CC_EXT_END

#endif /* defined(__NodeController__) */
