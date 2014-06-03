#include "platform/linux/CCCursorLinux.h"
#include "CCGeometry.h"
#include "CCImage.h"
#include "platform/CCCursor.h"
#include "platform/CCCommon.h"
#include <gdk/gdk.h>

NS_CC_BEGIN

void CCCursorManagerLinux::showCurrentCursor() {
    gdk_window_set_cursor(GDK_WINDOW(window), ((CCCursorLinux*)getCurrentCursor())->getCursor());
}

void
CCCursorManagerLinux::hideCurrentCursor() {
   gdk_window_set_cursor(GDK_WINDOW(window), gdk_cursor_new_for_display(NULL, GDK_BLANK_CURSOR));
}

CCCursor* CCCursorManagerLinux::createCursorFromImage(CCImage* iImage, const CCPoint& iHotspot) {
    return new CCCursorLinux(iImage, iHotspot);
}


void CCCursorManagerLinux::_updateCurrentCursor() {
    gdk_window_set_cursor(GDK_WINDOW(window), ((CCCursorLinux*)getCurrentCursor())->getCursor());
}



void put_pixel (GdkPixbuf *pixbuf, int x, int y, guchar red, guchar green, guchar blue, guchar alpha)
{
  int width, height, rowstride, n_channels;
  guchar *pixels, *p;

  n_channels = gdk_pixbuf_get_n_channels (pixbuf);

  g_assert (gdk_pixbuf_get_colorspace (pixbuf) == GDK_COLORSPACE_RGB);
  g_assert (gdk_pixbuf_get_bits_per_sample (pixbuf) == 8);
  g_assert (gdk_pixbuf_get_has_alpha (pixbuf));
  g_assert (n_channels == 4);

  width = gdk_pixbuf_get_width (pixbuf);
  height = gdk_pixbuf_get_height (pixbuf);

  g_assert (x >= 0 && x < width);
  g_assert (y >= 0 && y < height);

  rowstride = gdk_pixbuf_get_rowstride (pixbuf);
  pixels = gdk_pixbuf_get_pixels (pixbuf);

  p = pixels + y * rowstride + x * n_channels;
  p[0] = red;
  p[1] = green;
  p[2] = blue;
  p[3] = alpha;
}

CCCursorLinux::CCCursorLinux(CCImage* iImage, const CCPoint& iHotspot) {

    GdkDisplay* display = gdk_display_get_default();
    guint maxW, maxH;
    gdk_display_get_maximal_cursor_size(display, &maxW, &maxH);

    guint w = iImage->getWidth() < maxW ? iImage->getWidth() : maxW;
    guint h = iImage->getHeight() < maxH ? iImage->getHeight() : maxH;

    GdkPixbuf* pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, iImage->hasAlpha(), iImage->getBitsPerComponent(), w, h);

    for(int y=0, i=0; y < iImage->getHeight(); y++) {
        for(int x=0; x < iImage->getWidth(); x++) {
            if (x >= w || y >= h) {
                i += 4;
                continue;
            }

            guchar r = iImage->getData()[i++];
            guchar g = iImage->getData()[i++];
            guchar b = iImage->getData()[i++];
            guchar a = iImage->getData()[i++];
            put_pixel(pixbuf, x, y, r, g, b, a);
        }
    }

    cursor = gdk_cursor_new_from_pixbuf(display, pixbuf,
					  iHotspot.x,
					  iHotspot.y);

    gdk_pixmap_unref (pixbuf);
}


CCCursorLinux::~CCCursorLinux() {
    gdk_cursor_unref (cursor);
}

NS_CC_END
