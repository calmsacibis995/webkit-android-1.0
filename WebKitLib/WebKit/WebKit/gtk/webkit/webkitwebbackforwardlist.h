/*
 * Copyright (C) 2008 Jan Michael C. Alonzo <jmalonzo@unpluggable.com>
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

#ifndef WEBKIT_WEB_BACK_FORWARD_LIST_H
#define WEBKIT_WEB_BACK_FORWARD_LIST_H

#include <glib.h>
#include <glib-object.h>

#include <webkit/webkitdefines.h>
#include <webkit/webkitwebhistoryitem.h>

G_BEGIN_DECLS

#define WEBKIT_TYPE_WEB_BACK_FORWARD_LIST            (webkit_web_back_forward_list_get_type())
#define WEBKIT_WEB_BACK_FORWARD_LIST(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), WEBKIT_TYPE_WEB_BACK_FORWARD_LIST, WebKitWebBackForwardList))
#define WEBKIT_WEB_BACK_FORWARD_LIST_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  WEBKIT_TYPE_WEB_BACK_FORWARD_LIST, WebKitWebBackForwardListClass))
#define WEBKIT_IS_WEB_BACK_FORWARD_LIST(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), WEBKIT_TYPE_WEB_BACK_FORWARD_LIST))
#define WEBKIT_IS_WEB_BACK_FORWARD_LIST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  WEBKIT_TYPE_WEB_BACK_FORWARD_LIST))
#define WEBKIT_WEB_BACK_FORWARD_LIST_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  WEBKIT_TYPE_WEB_BACK_FORWARD_LIST, WebKitWebBackForwardListClass))

typedef struct _WebKitWebBackForwardListPrivate WebKitWebBackForwardListPrivate;

struct _WebKitWebBackForwardList {
    GObject parent_instance;

    WebKitWebBackForwardListPrivate* priv;
};

struct _WebKitWebBackForwardListClass {
    GObjectClass parent_class;
};

WEBKIT_API GType
webkit_web_back_forward_list_get_type (void);

WEBKIT_API WebKitWebBackForwardList*
webkit_web_back_forward_list_new_with_web_view (WebKitWebView* web_view);

WEBKIT_API void
webkit_web_back_forward_list_go_forward (WebKitWebBackForwardList* web_back_forward_list);

WEBKIT_API void
webkit_web_back_forward_list_go_back (WebKitWebBackForwardList* web_back_forward_list);

WEBKIT_API gboolean
webkit_web_back_forward_list_contains_item (WebKitWebBackForwardList* web_back_forward_list, WebKitWebHistoryItem* history_item);

WEBKIT_API void
webkit_web_back_forward_list_go_to_item (WebKitWebBackForwardList* web_back_forward_list, WebKitWebHistoryItem* history_item);

WEBKIT_API GList*
webkit_web_back_forward_list_get_forward_list_with_limit (WebKitWebBackForwardList* web_back_forward_list, gint limit);

WEBKIT_API GList*
webkit_web_back_forward_list_get_back_list_with_limit (WebKitWebBackForwardList* web_back_forward_list, gint limit);

WEBKIT_API WebKitWebHistoryItem*
webkit_web_back_forward_list_get_back_item (WebKitWebBackForwardList* web_back_forward_list);

WEBKIT_API WebKitWebHistoryItem*
webkit_web_back_forward_list_get_current_item (WebKitWebBackForwardList* web_back_forward_list);

WEBKIT_API WebKitWebHistoryItem*
webkit_web_back_forward_list_get_forward_item (WebKitWebBackForwardList* web_back_forward_list);

WEBKIT_API WebKitWebHistoryItem*
webkit_web_back_forward_list_get_nth_item (WebKitWebBackForwardList* web_back_forward_list, gint index);

WEBKIT_API gint
webkit_web_back_forward_list_get_back_length (WebKitWebBackForwardList* web_back_forward_list);

WEBKIT_API gint
webkit_web_back_forward_list_get_forward_length (WebKitWebBackForwardList* web_back_forward_list);

WEBKIT_API gint
webkit_web_back_forward_list_get_limit (WebKitWebBackForwardList* web_back_forward_list);

WEBKIT_API void
webkit_web_back_forward_list_set_limit (WebKitWebBackForwardList* web_back_forward_list, gint limit);

G_END_DECLS


#endif /* WEBKIT_WEB_BACK_FORWARD_LIST_H */
