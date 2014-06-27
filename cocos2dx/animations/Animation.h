//
//  Animation.h
//
//  Created by Gérald Guyomard on 18/10/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __Animation__
#define __Animation__

#include "cocoa/CCObjectPtr.h"
#include "CCScheduler.h"

NS_CC_BEGIN

    class AnimationDelegate;
	class Animation;
	typedef CCObjectPtr<Animation> AnimationPtr;

    class Animation : public CCObject
    {
    public:
        Animation();
        virtual ~Animation();

        virtual void start() = 0;
        virtual void stop() = 0;

        bool isPaused() const { return fIsPaused; }
        bool pause();
        void resume();

        virtual bool touchesEnabled() const; // default false

		virtual bool canSkip();
		AnimationPtr skip(); // Returm replacement anim to play right now, otherwise nullptr to let the anim finish
		bool skipping() const { return fSkipping;  }
		virtual bool furtherAnimationsAreInvalidAfterSkip() const { return true; }

        AnimationDelegate* delegate() const { return fDelegate; }
        void setDelegate(AnimationDelegate* iDelegate) { fDelegate = iDelegate; }

        void finish();

        // Schedule
        CCScheduler* scheduler() const;

        void schedule(SEL_SCHEDULE selector, float interval, unsigned int repeat, float delay);
        void scheduleOnce(SEL_SCHEDULE selector, float delay);
        void unschedule(SEL_SCHEDULE selector);

    protected:

        void _didStop(bool iFinished);

        // default not supported
        virtual bool _pause() { return false; }
        virtual void _resume() {}

		virtual AnimationPtr _skip();
		virtual void _didSkip(bool iFinished)  {}

        AnimationDelegate* fDelegate;

    private:
        bool fIsPaused;
		bool fSkipping;
    };

    class AnimationDelegate
    {
    public:
        virtual void animationDidStop(const AnimationPtr& iAnimation, bool iFinished) = 0;

		virtual bool animationAllowedToSkip(const AnimationPtr& iAnimation) { return false; }

    protected:
        ~AnimationDelegate() {}
    };

NS_CC_END

#endif /* defined(__Animation__) */
