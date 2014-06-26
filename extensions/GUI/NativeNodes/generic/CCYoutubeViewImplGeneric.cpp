//
#include "CCYoutubeView.h"
#include "CCYoutubeViewImpl.h"
#include "CCNativeViewImpl.h"

namespace swapp
{

	class NoYoutubeView : public CCNativeViewImpl, public CCYoutubeViewImpl
	{
	public:
		NoYoutubeView() {}
		virtual ~NoYoutubeView() {}

		virtual bool init(CCNode* iNode) { return true; }
		virtual CCNode* owner() const { return nullptr; }
		virtual void onEnter() {}
		virtual void onEnterTransitionDidFinish() {}
		virtual void onExit(void) {}
		virtual void onExitTransitionDidStart() {}

		virtual void playMovie(const std::string& iMovieID) override {}
	};

    std::unique_ptr<CCNativeViewImpl>
    CCYoutubeView::_createImpl()
    {
		return std::unique_ptr<CCNativeViewImpl>(new NoYoutubeView());
    }
}

