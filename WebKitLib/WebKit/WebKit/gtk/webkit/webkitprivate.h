/*
 * Copyright (C) 2007 Holger Hans Peter Freyther
 * Copyrifht (C) 2008 Jan Michael C. Alonzo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef WEBKIT_PRIVATE_H
#define WEBKIT_PRIVATE_H

/*
 * This file knows the shared secret of WebKitWebView and WebKitWebFrame.
 * They are using WebCore which musn't be exposed to the outer world.
 */

#include <webkit/webkitdefines.h>
#include <webkit/webkitwebview.h>
#include <webkit/webkitwebframe.h>
#include <webkit/webkitwebsettings.h>
#include <webkit/webkitnetworkrequest.h>
#include <webkit/webkitwebbackforwardlist.h>

#include "BackForwardList.h"
#include "HistoryItem.h"
#include "Settings.h"
#include "Page.h"
#include "Frame.h"
#include "FrameLoaderClient.h"

#include <glib.h>

namespace WebKit {
    WebKitWebView* getViewFromFrame(WebKitWebFrame*);

    WebCore::Frame* core(WebKitWebFrame*);
    WebKitWebFrame* kit(WebCore::Frame*);

    WebCore::Page* core(WebKitWebView*);
    WebKitWebView* kit(WebCore::Page*);

    WebCore::HistoryItem* core(WebKitWebHistoryItem*);
    WebKitWebHistoryItem* kit(WebCore::HistoryItem*);

    WebCore::BackForwardList* core(WebKitWebBackForwardList*);
}

extern "C" {
    void webkit_init();

#define WEBKIT_PARAM_READABLE ((GParamFlags)(G_PARAM_READABLE|G_PARAM_STATIC_NAME|G_PARAM_STATIC_NICK|G_PARAM_STATIC_BLURB))
#define WEBKIT_PARAM_READWRITE ((GParamFlags)(G_PARAM_READWRITE|G_PARAM_STATIC_NAME|G_PARAM_STATIC_NICK|G_PARAM_STATIC_BLURB))

    #define WEBKIT_WEB_VIEW_GET_PRIVATE(obj)    (G_TYPE_INSTANCE_GET_PRIVATE((obj), WEBKIT_TYPE_WEB_VIEW, WebKitWebViewPrivate))
    typedef struct _WebKitWebViewPrivate WebKitWebViewPrivate;
    struct _WebKitWebViewPrivate {
        WebCore::Page* corePage;
        WebKitWebSettings* webSettings;

        WebKitWebFrame* mainFrame;
        WebCore::String applicationNameForUserAgent;
        WebCore::String* userAgent;

        WebKitWebBackForwardList* backForwardList;

        gint lastPopupXPosition;
        gint lastPopupYPosition;

        HashSet<GtkWidget*> children;
        bool editable;
        GtkIMContext* imContext;

        GtkTargetList* copy_target_list;
        GtkTargetList* paste_target_list;
    };

    #define WEBKIT_WEB_FRAME_GET_PRIVATE(obj)    (G_TYPE_INSTANCE_GET_PRIVATE((obj), WEBKIT_TYPE_WEB_FRAME, WebKitWebFramePrivate))
    typedef struct _WebKitWebFramePrivate WebKitWebFramePrivate;
    struct _WebKitWebFramePrivate {
        WebCore::Frame* coreFrame;
        WebCore::FrameLoaderClient* client;
        WebKitWebView* webView;

        gchar* name;
        gchar* title;
        gchar* uri;
    };

    #define WEBKIT_NETWORK_REQUEST_GET_PRIVATE(obj)    (G_TYPE_INSTANCE_GET_PRIVATE((obj), WEBKIT_TYPE_NETWORK_REQUEST, WebKitNetworkRequestPrivate))
    typedef struct _WebKitNetworkRequestPrivate WebKitNetworkRequestPrivate;
    struct _WebKitNetworkRequestPrivate {
        gchar* uri;
    };

    WebKitWebFrame*
    webkit_web_frame_init_with_web_view(WebKitWebView*, WebCore::HTMLFrameOwnerElement*);

    WebKitWebHistoryItem*
    webkit_web_history_item_new_with_core_item(WebCore::HistoryItem*);

    // FIXME: Move these to webkitwebframe.h once their API has been discussed.

    WEBKIT_API GSList*
    webkit_web_frame_get_children (WebKitWebFrame* frame);

    WEBKIT_API gchar*
    webkit_web_frame_get_inner_text (WebKitWebFrame* frame);

    WEBKIT_API void
    webkit_web_frame_print (WebKitWebFrame* frame);

    WEBKIT_API gchar*
    webkit_web_view_get_selected_text (WebKitWebView* web_view);
}

#endif
