//
//  SequenceAnimation.h
//
//  Created by Gérald Guyomard on 19/12/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __SequenceAnimation__
#define __SequenceAnimation__

#include "animations/Animation.h"
#include "animations/AnimationQueue.h"
#include "DOWFoundation/DOWFoundation.h"

NS_CC_BEGIN

    class SequenceAnimation;
    typedef CCObjectPtr<SequenceAnimation> SequenceAnimationPtr;
    
    class SequenceAnimation : public Animation, public AnimationQueueDelegate
    {
    public:
		DOW_DECLARE_HIERARCHY(SequenceAnimation, Animation);

        static SequenceAnimationPtr create();
        virtual ~SequenceAnimation();
        
        void addAnimation(const AnimationPtr& iAnimation);
        
        void addInstantAnimation(const std::function<void (const AnimationPtr& iAnimation)>& iInstantLambda);
        void addLastingAnimation(const std::function<void (const AnimationPtr& iAnimation)>& iStartLambda, const std::function<void (const AnimationPtr& iAnimation)>& iStopLambda, bool iTouchesEnabled = false);

		virtual bool canSkip() override;
		virtual bool furtherAnimationsAreInvalidAfterSkip() const { return fFurtherAnimsAreInvalidAfterSkip;  }
        virtual void start() override;
        virtual void stop() override;

    protected:
        SequenceAnimation();

		virtual bool _pause() override;
		virtual void _resume() override;
		virtual AnimationPtr _skip() override;

    private:

        virtual void animationWillStart(const AnimationPtr& iAnim) override;
        virtual void animationDidFinish(const AnimationPtr& iAnim, bool iFinished) override;
		virtual void animationWasReplacedForSkip(const AnimationPtr& iOldAnimation, const AnimationPtr& iNewAnimation) override;

        AnimationQueuePtr fQueue;
        int32_t           fNbPendingAnimations;
		bool			  fFurtherAnimsAreInvalidAfterSkip = true;
    };
    
NS_CC_END

#endif /* defined(__SequenceAnimation__) */
