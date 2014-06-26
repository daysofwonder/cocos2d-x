#ifndef SW2EGLVIEW_H_
#define SW2EGLVIEW_H_

#include "CCEGLView.h"
#include <functional>
#include <gtk/gtk.h>

namespace swapp
{

    class SW2EGLView : public cocos2d::CCEGLView
    {
    public:
        static SW2EGLView* sharedOpenGLView();


        void postToGTK(const std::function<void()>& callback);
        GtkContainer* getContainer();

        virtual void setFrameSize(float width, float height) override final;
        float getHeight() const {return fHeight;}

        void attachFloating(GtkWidget* iWidget,const cocos2d::CCRect& iViewRect);
        void detachFloating(GtkWidget* iWidget);
        void moveFloating(GtkWidget* iWidget,const cocos2d::CCRect& iViewRect);

        GdkWindow* getGdkWindow() { return fDrawingArea->widget.GSEAL(window); }

    private:
        SW2EGLView();
        ~SW2EGLView();

        GtkWindow* fWindow;
        GtkLayout* fLayout;
        GtkDrawingArea* fDrawingArea;
        float fHeight;

        static gboolean configureW (GtkWidget *window, GdkEventConfigure *event, gpointer user_data);
        static gboolean do_button_press (GtkWidget *window, GdkEventButton* event , gpointer user_data);
        static void on_set_focus(GtkWindow *window, GtkWidget *widget, gpointer user_data);

    };

}

#endif
