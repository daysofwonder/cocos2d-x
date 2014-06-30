//
//  AnimationQueue.h
//
//  Created by Gérald Guyomard on 18/10/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __AnimationQueue__
#define __AnimationQueue__

#include "animations/Animation.h"
#include <functional>
#include <vector>
#include <cstdint>

NS_CC_BEGIN

    class AnimationQueue;
    typedef CCObjectPtr<AnimationQueue> AnimationQueuePtr;

    class AnimationQueueDelegate;
    
    class AnimationQueue : public CCObject, protected AnimationDelegate
    {
    public:
        static AnimationQueuePtr create();
        virtual ~AnimationQueue();
        
        void queueAnimation(const AnimationPtr& iAnimation, bool iStart = true);
        void queueInstantAnimation(const std::function<void (const AnimationPtr& iAnimation)>& iInstantLambda, bool iStart = true);
        void queueLastingAnimation(const std::function<void (const AnimationPtr& iAnimation)>& iStartLambda, const std::function<void (const AnimationPtr& iAnimation)>& iStopLambda, bool iTouchesEnabled = false, bool iStart = true);
        
        bool isValid() const { return fIsValid != 0; }
        void clearPendingAnimations(bool iInvalidateQueue = true);
        
        AnimationQueueDelegate* delegate() const { return fDelegate; }
        void setDelegate(AnimationQueueDelegate* iDelegate) { fDelegate = iDelegate; }

        void startNextAnimation();

        // Return true to stop
        typedef std::function<bool (const AnimationPtr& iAnim)> TIterator;
        bool iterateAnimations(const TIterator& iIterator);
        
        bool isPaused() const;
        void pause();
        void resume();
        
        // Skip
        bool canSkipCurrentAnimation() const;
        void skipCurrentAnimation();
        
    protected:
        AnimationQueue();

        // From AnimationDelegate
		virtual bool animationAllowedToSkip(const AnimationPtr& iAnimation) override;
        virtual void animationDidStop(const AnimationPtr& iAnimation, bool iFinished) override;

    private:
        AnimationPtr                fAnimationInProgress;
        std::vector<AnimationPtr>   fQueue;
        AnimationQueueDelegate*     fDelegate;
        
        int16_t                     fPauseCounter;
        uint16_t                    fIsValid;
    };
     
    class AnimationQueueDelegate
    {
    public:
        virtual void animationWillStart(const AnimationPtr& iAnim) {}
        virtual void animationDidStart(const AnimationPtr& iAnim) {}

		virtual void animationDidFinish(const AnimationPtr& iAnim, bool iFinished) {}
        
        virtual void enableUserInteraction(bool iEnable) {};
        
        virtual void animationQueueWillPause(AnimationQueue& iQueue) {}
        virtual void animationQueueDidResume(AnimationQueue& iQueue) {}

		virtual bool animationAllowedToSkip(AnimationQueue& iQueue, const AnimationPtr& iAnimation) { return true; }
		virtual void animationQueueWillSkip(AnimationQueue& iQueue) {}
		virtual void animationWasReplacedForSkip(const AnimationPtr& iOldAnimation, const AnimationPtr& iNewAnimation) {}
		virtual void animationQueueDidSkip(AnimationQueue& iQueue) {}

    protected:
        ~AnimationQueueDelegate() {}
    };

NS_CC_END

#endif /* defined(__AnimationQueue__) */
