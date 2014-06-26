#include "SW2EGLView.h"

#include <GL/gl.h>
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>

#include "CCDirector.h"
#include "CCGeometry.h"
#include "base_nodes/CCNode.h"
#include "layers_scenes_transitions_nodes/CCScene.h"

#include <boost/log/trivial.hpp>
#include <gdk/gdkkeysyms.h>
#include "keypad_dispatcher/CCKeypadDispatcher.h"

#include "CCApplication.h"

namespace {

static gboolean animateDrawingArea (gpointer user_data)
{
	cocos2d::CCApplication::sharedApplication()->timerCallback();

    GtkWidget *gtkWidget = GTK_WIDGET (user_data);
    GdkWindow *gdkWin = gtk_widget_get_window(GTK_WIDGET(gtkWidget));

    //BOOST_LOG_TRIVIAL(info) << "begin animateDrawingArea" << std::endl;

    gdk_window_invalidate_rect (gdkWin, &gtkWidget->allocation, FALSE);
    //gdk_window_process_updates (gdkWin, FALSE);

    //BOOST_LOG_TRIVIAL(info) << "end animateDrawingArea" << std::endl;

    return TRUE;
}


    static gboolean exposeLog (GtkWidget *widget, GdkEventExpose *event, gpointer user_data)
    {
		const GdkRectangle area = event->area;
        BOOST_LOG_TRIVIAL(info) <<"exposeLog area("<<area.x<<","<<area.y<<","<<area.width<<","<<area.height<<")"<<std::endl;
		return FALSE;
    }

    static gboolean
    exposeDrawingArea (GtkWidget *widget, GdkEventExpose *event, gpointer user_data)
    {
        GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
        GdkGLDrawable* glDrawable = gtk_widget_get_gl_drawable (widget);

        //BOOST_LOG_TRIVIAL(info) << "begin exposeDrawingArea" << std::endl;

        if (!gdk_gl_drawable_gl_begin (glDrawable, glcontext)) {
            g_assert_not_reached ();
        }

        cocos2d::CCDirector::sharedDirector()->mainLoop();

        if (gdk_gl_drawable_is_double_buffered (glDrawable)) {
            gdk_gl_drawable_swap_buffers (glDrawable);
        } else {
            glFlush ();
        }

        gdk_gl_drawable_gl_end (glDrawable);

        //BOOST_LOG_TRIVIAL(info) << "end exposeDrawingArea" << std::endl;

        return FALSE;
    }

    struct ExposeLayoutChildData {
    	GdkRegion *region;
    	cairo_t *cr;
    };
    void exposeLayoutChild(GtkWidget *child,gpointer opaque)
    {
    	struct ExposeLayoutChildData* data = static_cast<struct ExposeLayoutChildData*>(opaque);
    	cairo_t *cr = data->cr;
		cairo_save(cr);
		gdk_cairo_set_source_window (cr, child->window,child->allocation.x,child->allocation.y);
		GdkRegion* region = gdk_region_rectangle (&child->allocation);
		gdk_region_intersect (region, data->region);
		gdk_cairo_region (cr, region);
		cairo_clip (cr);
		cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
		cairo_paint(cr);
		cairo_restore(cr);
    }
    gboolean exposeLayout (GtkWidget *widget, GdkEventExpose *event, gpointer user_data)
    {
		ExposeLayoutChildData data;
		data.cr =  gdk_cairo_create(gtk_layout_get_bin_window(GTK_LAYOUT(widget)));
		data.region = event->region;
		gtk_container_foreach(GTK_CONTAINER(widget),exposeLayoutChild , &data);
		cairo_destroy(data.cr);
		return FALSE;
    }


    static gboolean
    configureDrawingArea (GtkWidget *da, GdkEventConfigure *event, gpointer user_data)
    {
        const int windowW = da->allocation.width;
        const int windowH = da->allocation.height;
		BOOST_LOG_TRIVIAL(info) << "configureDrawingArea: " << windowW << ","  << windowH	<<std::endl;


        GdkGLContext *glcontext = gtk_widget_get_gl_context (da);
        GdkGLDrawable* glDrawable = gtk_widget_get_gl_drawable (da);

        // g_print (" :: expose\n");

        if (!gdk_gl_drawable_gl_begin (glDrawable, glcontext)) {
            g_assert_not_reached ();
        }

        //glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);

        if (gdk_gl_drawable_is_double_buffered (glDrawable)) {
            gdk_gl_drawable_swap_buffers (glDrawable);
        } else {
            glFlush ();
        }
        gdk_gl_drawable_gl_end (glDrawable);


        // gtk_widget_set_size_request(da, event->width, event->height);
        return FALSE;
    }



    gboolean
    do_mouse_moved(GtkWidget *widget,
    				GdkEventMotion  *event,
                   gpointer   user_data)
    {
        swapp::SW2EGLView* pEGLView = swapp::SW2EGLView::sharedOpenGLView();
        assert(pEGLView);
        const float height = pEGLView->getHeight();
        cocos2d::CCPoint mousePoint(event->x,event->y);
        mousePoint.x /= pEGLView->getFrameZoomFactor();
        mousePoint.y /= pEGLView->getFrameZoomFactor();

        const guint modifier = event->state;
        if (modifier & GdkModifierType::GDK_BUTTON1_MASK) {
            int id = 0;
        	pEGLView->handleTouchesMove(1, &id, &mousePoint.x, &mousePoint.y);

        } else {
        	pEGLView->handleMouseMoved(mousePoint.x,height - mousePoint.y);
        }
    	return TRUE;
    }

    gboolean do_scroll_event (GtkWidget *widget,GdkEventScroll   *event,gpointer   user_data)
    {
        swapp::SW2EGLView* pEGLView = swapp::SW2EGLView::sharedOpenGLView();
        assert(pEGLView);

        cocos2d::CCPoint mousePoint(event->x,event->y);
        mousePoint.x /= pEGLView->getFrameZoomFactor();
        mousePoint.y /= pEGLView->getFrameZoomFactor();

        float dx=0;
        float dy=0;
        float dz=0;
        const float speed = 16; // pixels I guess, in whatever resolution things happen

        switch(event->direction) {
        case GdkScrollDirection::GDK_SCROLL_UP:
        	dy=+speed;
        	break;
        case GdkScrollDirection::GDK_SCROLL_DOWN:
        	dy=-speed;
        	break;
        case GdkScrollDirection::GDK_SCROLL_LEFT:
        	dx=+speed;
        	break;
        case GdkScrollDirection::GDK_SCROLL_RIGHT:
        	dx=-speed;
        	break;
        }

        pEGLView->handleWheel(mousePoint.x, mousePoint.y, dx, dy, dz);
    	return TRUE;
    }

    void realizeFloating (GtkWidget *widget,  gpointer   user_data)
    {
        gdk_window_set_composited (widget->window, TRUE);
    }

    gboolean doRepostInitRootSize(gpointer data) {
    	const cocos2d::CCSize* size = static_cast<const cocos2d::CCSize*>(data);
        cocos2d::CCNode* root = cocos2d::CCDirector::sharedDirector()->getRunningScene();
        if (root) {
        	root->setContentSize(*size);
        	BOOST_LOG_TRIVIAL(info) << "doRepostInitRootSize: " << size->width << "," << size->height << std::endl;
        	delete size;
        	return FALSE; //remove idle
        } else {
        	BOOST_LOG_TRIVIAL(info) << "doRepostInitRootSize: no ROOT" << std::endl;
        	return TRUE; //continue idle
        }
    }

    void postInitRootSize(const cocos2d::CCSize& screenSizeInPoints) {
    	const cocos2d::CCSize* size = new cocos2d::CCSize(screenSizeInPoints);
    	const void *data = static_cast<const void *>(size);
    	const gboolean doContinue = doRepostInitRootSize(const_cast<gpointer>(data));
    	if (doContinue) {
    		g_idle_add(doRepostInitRootSize,const_cast<gpointer>(data));
    		BOOST_LOG_TRIVIAL(info) << "postInitRootSize: "  << size->width << "," << size->height << std::endl;
    	}
    }


    gboolean do_key_event (GtkWidget *widget,GdkEventKey   *event, gpointer   user_data)
    {
    	if (event->type == GDK_KEY_PRESS) {
    		if (event->keyval==GDK_KEY_Return) {
                cocos2d::CCDirector::sharedDirector()->getKeypadDispatcher()->dispatchKeypadMSG(cocos2d::kTypeEnterClicked);
            	return TRUE; // stop other handler
    		} else if (event->keyval==GDK_KEY_BackSpace) {
    			cocos2d::CCDirector::sharedDirector()->getKeypadDispatcher()->dispatchKeypadMSG(cocos2d::kTypeBackClicked);
            	return TRUE; // stop other handler
    		}
    	}
    	return FALSE; // continue to other handler
    }

    gboolean do_focus_event(GtkWidget *widget,GdkEventFocus   *event, gpointer   user_data)
    {
		if (event->type == GDK_FOCUS_CHANGE) {
			if (event->in) {
		        BOOST_LOG_TRIVIAL(info) << "GL Gains Focus" << std::endl;
			} else {
		        BOOST_LOG_TRIVIAL(info) << "GL Looses Focus" << std::endl;
			}
		}
    	return FALSE; // continue to other handler
    }

}


namespace swapp {


    SW2EGLView* SW2EGLView::sharedOpenGLView()
    {
        static SW2EGLView* gSingleton = nullptr;
        if (gSingleton == NULL) {
            gSingleton = new SW2EGLView();
        }
        return gSingleton;
    }


    SW2EGLView::SW2EGLView()
    {
        fWindow=nullptr;
        fLayout = nullptr;
        fDrawingArea = nullptr;
        fHeight=0;
    }

    SW2EGLView::~SW2EGLView()
    {
    }

    void SW2EGLView::on_set_focus(GtkWindow *window, GtkWidget *widget, gpointer user_data)
    {
        swapp::SW2EGLView* self = static_cast<swapp::SW2EGLView *>(user_data);
        BOOST_LOG_TRIVIAL(info) << "Window Set Focus: "  << widget << std::endl;
        if (!widget) {
        	//we lost focus, lets reput it into GL
        	GtkWidget* glWidget = GTK_WIDGET(self->fDrawingArea);
    		self->postToGTK([glWidget](){
                gtk_widget_grab_focus(glWidget);
        	});

        }
    }

    gboolean
    SW2EGLView::do_button_press (GtkWidget *window, GdkEventButton* event , gpointer user_data)
    {
        swapp::SW2EGLView* self = static_cast<swapp::SW2EGLView *>(user_data);
        assert(self);

		//get current mouse pos
		int x,y;
		//glfwGetMousePos(&x, &y);
		x = event->x;
		y= event->y;

		cocos2d::CCPoint oPoint((float)x,(float)y);
		/*
		if (!CCRect::CCRectContainsPoint(s_pMainWindow->m_rcViewPort,oPoint))
		{
			CCLOG("not in the viewport");
			return;
		}
		*/
		oPoint.x /= self->getFrameZoomFactor();
		oPoint.y /= self->getFrameZoomFactor();
		int id = 0;

		if (event->type == GDK_BUTTON_PRESS) {
			gtk_window_set_focus(GTK_WINDOW(self->fWindow),GTK_WIDGET(self->fDrawingArea));
			if (event->button==1) {
				self->handleTouchesBegin(1, &id, &oPoint.x, &oPoint.y);
			} else if (event->button==3) {
				self->handleSecondaryButtonDown(oPoint.x, oPoint.y, 0);
			}
		} else if (event->type == GDK_BUTTON_RELEASE) {
			if (event->button==1) {
				self->handleTouchesEnd(1, &id, &oPoint.x, &oPoint.y);
			} else if (event->button==3) {
				self->handleSecondaryButtonUp(oPoint.x, oPoint.y, 0);
			}
		}
        return TRUE;
    }

    gboolean
    SW2EGLView::configureW (GtkWidget *window, GdkEventConfigure *event, gpointer user_data)
    {
        swapp::SW2EGLView* self = static_cast<swapp::SW2EGLView *>(user_data);
        assert(self);
        cocos2d::CCEGLView* eGLView = cocos2d::CCEGLView::sharedOpenGLView();
        assert(eGLView);

        const int windowW = window->allocation.width;
        const int windowH = window->allocation.height;


        const cocos2d::CCSize& oldSize = eGLView->getFrameSize();
        if (oldSize.width==event->width && oldSize.height==event->height) {
    		BOOST_LOG_TRIVIAL(info) << "configureW: No change in Size" <<std::endl;
        	return FALSE;
        }

		BOOST_LOG_TRIVIAL(info) << "configureW: (" << windowW << "," << windowH	<< ")->("
				<< event->x << "," << event->y << ","
				<< event->width << "," << event->height << ")" <<std::endl;

        self->fHeight = event->height;

        gtk_widget_set_size_request(GTK_WIDGET(self->fDrawingArea), event->width, event->height);


        eGLView->CCEGLViewProtocol::setFrameSize(event->width, event->height);

        cocos2d::CCSize screenSizeInPixels;
        screenSizeInPixels.width = event->width;
        screenSizeInPixels.height = event->height;


        const float contentScale  = cocos2d::CCDirector::sharedDirector()->getContentScaleFactor();
        cocos2d::CCSize screenSizeInPoints(ceilf(screenSizeInPixels.width / contentScale), ceilf(screenSizeInPixels.height / contentScale));

        postInitRootSize(screenSizeInPoints); // moved from after setDesignResolutionSize
        eGLView->setDesignResolutionSize(screenSizeInPoints.width, screenSizeInPoints.height, kResolutionNoBorder);

        return FALSE;
    }

    void SW2EGLView::attachFloating(GtkWidget* iWidget,const cocos2d::CCRect& iViewRect)
    {
    	assert(iWidget);
    	GtkLayout* layout = fLayout;
    	//postToGTK([iViewRect,iWidget,layout](){
			GdkScreen* screen = gtk_widget_get_screen (iWidget);
			GdkColormap* rgba = gdk_screen_get_rgba_colormap (screen);
			gtk_widget_set_colormap (iWidget, rgba);

			//g_signal_connect(iWidget, "realize", G_CALLBACK (realizeFloating), NULL);

			gtk_widget_set_size_request(GTK_WIDGET(iWidget), iViewRect.size.width, iViewRect.size.height);
			gtk_layout_put(layout, GTK_WIDGET(iWidget), iViewRect.origin.x, iViewRect.origin.y);

			gtk_widget_show_all(iWidget);
			gdk_window_set_composited (iWidget->window, TRUE);
		//});
    }

    void SW2EGLView::detachFloating(GtkWidget* iWidget)
    {
    	assert(iWidget);
    	GtkLayout* layout = fLayout;
    	//postToGTK([iWidget,layout](){
            gtk_container_remove(GTK_CONTAINER(layout),iWidget);
    	//});
    }

    void SW2EGLView::moveFloating(GtkWidget* iWidget,const cocos2d::CCRect& iViewRect)
    {
    	assert(iWidget);
    	GtkLayout* layout = fLayout;
    	//postToGTK([iViewRect,iWidget,layout](){
			gtk_layout_move(layout, iWidget, iViewRect.origin.x, iViewRect.origin.y);
			gtk_widget_set_size_request(iWidget, iViewRect.size.width, iViewRect.size.height);
    	//});
    }


    struct closure {
        std::function<void()> callback;
    };

    gboolean
    do_once(gpointer user_data)
    {
        closure* myClosure = (closure *) user_data;
        myClosure->callback();
        delete myClosure;
        return FALSE;
    }

    void
    SW2EGLView::postToGTK(const std::function<void()>& callback)
    {
        closure* myClosure = new closure();
        myClosure->callback = callback;
        g_timeout_add(0,do_once,myClosure);
    }

    void SW2EGLView::setFrameSize(float width, float height)
    {
        fWindow = GTK_WINDOW(gtk_window_new (GTK_WINDOW_TOPLEVEL));
        fLayout = GTK_LAYOUT(gtk_layout_new(nullptr,nullptr));
        fDrawingArea = GTK_DRAWING_AREA(gtk_drawing_area_new());

        gtk_widget_set_can_focus(GTK_WIDGET(fDrawingArea),true);

        //gtk_widget_set_app_paintable(GTK_WIDGET(fWindow), TRUE); // impact of first screen wrong size at startup? NO

        g_signal_connect_swapped (fWindow, "destroy",G_CALLBACK (gtk_main_quit), NULL);

        gtk_container_set_reallocate_redraws (GTK_CONTAINER (fLayout), TRUE);

        /* prepare GL */
        GdkGLConfig *glconfig = gdk_gl_config_new_by_mode (static_cast<GdkGLConfigMode>(
                                    GDK_GL_MODE_RGB |
                                    GDK_GL_MODE_DEPTH |
                                    GDK_GL_MODE_DOUBLE));
        if (!glconfig) {
            g_assert_not_reached ();
        }
        if (!gtk_widget_set_gl_capability (GTK_WIDGET(fDrawingArea), glconfig, nullptr, true ,GDK_GL_RGBA_TYPE)) {
            g_assert_not_reached ();
        }

        gtk_widget_set_events (GTK_WIDGET(fDrawingArea),  GDK_FOCUS_CHANGE_MASK
        			| GDK_KEY_PRESS_MASK | GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK
        			| GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);
        //gtk_widget_set_events(GTK_WIDGET(fWindow) , GDK_KEY_PRESS_MASK);

        g_signal_connect (fWindow, "configure-event", G_CALLBACK (configureW), this);
        //g_signal_connect (fWindow, "expose-event", G_CALLBACK (exposeWindow), this);
        //g_signal_connect(fWindow,"key-press-event",G_CALLBACK(do_key_event) , NULL);
        g_signal_connect(fWindow,"set-focus",G_CALLBACK(on_set_focus) , this);

        g_signal_connect (fLayout, "expose-event", G_CALLBACK (exposeLayout), NULL);

        //g_signal_connect(fDrawingArea, "realize", G_CALLBACK (realizeFloating), NULL);
        //g_signal_connect (fDrawingArea, "realize", G_CALLBACK (configureDrawingArea), NULL);
        g_signal_connect (fDrawingArea, "configure-event", G_CALLBACK (configureDrawingArea), NULL);
        g_signal_connect (fDrawingArea, "expose-event", G_CALLBACK (exposeDrawingArea), NULL);
        g_signal_connect(fDrawingArea,"button-press-event",G_CALLBACK(do_button_press) , this);
        g_signal_connect(fDrawingArea,"button-release-event",G_CALLBACK(do_button_press) , this);
        g_signal_connect(fDrawingArea,"motion-notify-event",G_CALLBACK(do_mouse_moved) , NULL);
        g_signal_connect(fDrawingArea,"key-press-event",G_CALLBACK(do_key_event) , NULL);
        g_signal_connect(fDrawingArea,"focus-in-event",G_CALLBACK(do_focus_event) , NULL);
        g_signal_connect(fDrawingArea,"focus-out-event",G_CALLBACK(do_focus_event) , NULL);
        g_signal_connect(fDrawingArea,"scroll-event",G_CALLBACK(do_scroll_event) , NULL);


        gtk_container_add(GTK_CONTAINER(fWindow), GTK_WIDGET(fLayout));
        gtk_container_add(GTK_CONTAINER(fLayout), GTK_WIDGET(fDrawingArea));

        g_timeout_add (1000*cocos2d::CCDirector::sharedDirector()->getAnimationInterval() , animateDrawingArea, fDrawingArea);

        fHeight = height;
        gtk_window_set_default_size (GTK_WINDOW (fWindow), width, height);
        gtk_window_set_position(fWindow, GTK_WIN_POS_CENTER);

        gtk_widget_show_all(GTK_WIDGET(fWindow));
        gdk_window_set_composited(GTK_WIDGET(fDrawingArea)->window, TRUE); //(done in realize)
        gtk_widget_grab_focus(GTK_WIDGET(fDrawingArea));
        CCEGLView::setFrameSize(0,0); // actually InitGL (somehow after show_all because of mystery crash)
    }




}


cocos2d::CCEGLView* cocos2d::CCEGLView::sharedOpenGLView()
{
    return swapp::SW2EGLView::sharedOpenGLView();
}


