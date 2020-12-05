/*
 * bbschem
 * Copyright (C) 2020 Edward C. Hennessy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>
#include <bbextensions.h>
#include "bbpanpointaction.h"
#include "bbzoomsubject.h"

enum
{
    PROP_0,
    PROP_ENABLED,
    PROP_NAME,
    PROP_PARAMETER_TYPE,
    PROP_STATE,
    PROP_STATE_HINT,
    PROP_STATE_TYPE,
    PROP_SUBJECT,
    N_PROPERTIES
};


struct _BbPanPointAction
{
    GObject parent;
    BbDocumentWindow* subject;
};


static void
bb_pan_point_action_action_init(GActionInterface *iface);

static void
bb_pan_point_action_activate(GAction *action, GVariant *parameter);

static void
bb_pan_point_action_change_state(GAction *action, GVariant *value);

static void
bb_pan_point_action_dispose(GObject *object);

static void
bb_pan_point_action_finalize(GObject *object);

static gboolean
bb_pan_point_action_get_enabled(GAction *action);

static const gchar*
bb_pan_point_action_get_name(GAction *action);

static const GVariantType*
bb_pan_point_action_get_parameter_type(GAction *action);

static void
bb_pan_point_action_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static GVariant*
bb_pan_point_action_get_state(GAction *action);

static GVariant*
bb_pan_point_action_get_state_hint(GAction *action);

static const GVariantType*
bb_pan_point_action_get_state_type(GAction *action);

BbDocumentWindow*
bb_pan_point_action_get_subject(BbPanPointAction *action);

static void
bb_pan_point_action_notify(gpointer unused, GParamSpec *pspec, BbPanPointAction *action);

static void
bb_pan_point_action_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

void
bb_pan_point_action_set_subject(BbPanPointAction *action, BbDocumentWindow* subject);


static GParamSpec *properties[N_PROPERTIES];


G_DEFINE_TYPE_WITH_CODE(
    BbPanPointAction,
    bb_pan_point_action,
    G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE(G_TYPE_ACTION, bb_pan_point_action_action_init)
    )


static void
bb_pan_point_action_action_init(GActionInterface *iface)
{
    iface->activate = bb_pan_point_action_activate;
    iface->change_state = bb_pan_point_action_change_state;
    iface->get_enabled = bb_pan_point_action_get_enabled;
    iface->get_name = bb_pan_point_action_get_name;
    iface->get_parameter_type = bb_pan_point_action_get_parameter_type;
    iface->get_state = bb_pan_point_action_get_state;
    iface->get_state_hint = bb_pan_point_action_get_state_hint;
    iface->get_state_type = bb_pan_point_action_get_state_type;
}


static void
bb_pan_point_action_activate(GAction *action, GVariant *parameter)
{
    g_return_if_fail(BB_IS_PAN_POINT_ACTION(action));

    BbDocumentWindow *subject = bb_pan_point_action_get_subject(BB_PAN_POINT_ACTION(action));

    g_return_if_fail(BB_IS_ZOOM_SUBJECT(subject));

    bb_zoom_subject_pan_point(BB_ZOOM_SUBJECT(subject));
}


static void
bb_pan_point_action_change_state(GAction *action, GVariant *value)
{

}


static void
bb_pan_point_action_class_init(BbPanPointActionClass *klasse)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klasse);

    object_class->dispose = bb_pan_point_action_dispose;
    object_class->finalize = bb_pan_point_action_finalize;
    object_class->get_property = bb_pan_point_action_get_property;
    object_class->set_property = bb_pan_point_action_set_property;

    properties[PROP_ENABLED] = bb_object_class_override_property(
        object_class,
        PROP_ENABLED,
        "enabled"
        );

    properties[PROP_NAME] = bb_object_class_override_property(
        object_class,
        PROP_NAME,
        "name"
        );

    properties[PROP_PARAMETER_TYPE] = bb_object_class_override_property(
        object_class,
        PROP_PARAMETER_TYPE,
        "parameter-type"
        );

    properties[PROP_STATE] = bb_object_class_override_property(
        object_class,
        PROP_STATE,
        "state"
        );

    properties[PROP_STATE_TYPE] = bb_object_class_override_property(
        object_class,
        PROP_STATE_TYPE,
        "state-type"
        );

    properties[PROP_SUBJECT] = bb_object_class_install_property(
        object_class,
        PROP_SUBJECT,
        g_param_spec_object(
            "subject",
            "",
            "",
            BB_TYPE_DOCUMENT_WINDOW,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
        )
    );

}


static void
bb_pan_point_action_dispose(GObject *object)
{
    bb_pan_point_action_set_subject(BB_PAN_POINT_ACTION(object), NULL);
}


static void
bb_pan_point_action_finalize(GObject *object)
{
}


static gboolean
bb_pan_point_action_get_enabled(GAction *action)
{
    g_warn_if_fail(BB_IS_PAN_POINT_ACTION(action));

    return TRUE;
}


static const gchar*
bb_pan_point_action_get_name(GAction *action)
{
    g_warn_if_fail(BB_IS_PAN_POINT_ACTION(action));

    return "pan-point";
}


static const GVariantType*
bb_pan_point_action_get_parameter_type(GAction *action)
{
    g_return_val_if_fail(BB_IS_PAN_POINT_ACTION(action), NULL);

    return NULL;
}


static void
bb_pan_point_action_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    switch (property_id)
    {
        case PROP_ENABLED:
            g_value_set_boolean(value, bb_pan_point_action_get_enabled(G_ACTION(object)));
            break;

        case PROP_NAME:
            g_value_set_string(value, bb_pan_point_action_get_name(G_ACTION(object)));
            break;

        case PROP_PARAMETER_TYPE:
            g_value_set_boxed(value, bb_pan_point_action_get_parameter_type(G_ACTION(object)));
            break;

        case PROP_STATE:
            g_value_set_variant(value, bb_pan_point_action_get_state(G_ACTION(object)));
            break;

        case PROP_STATE_HINT:
            g_value_set_variant(value, bb_pan_point_action_get_state_hint(G_ACTION(object)));
            break;

        case PROP_STATE_TYPE:
            g_value_set_boxed(value, bb_pan_point_action_get_state_type(G_ACTION(object)));
            break;

        case PROP_SUBJECT:
            g_value_set_object(value, bb_pan_point_action_get_subject(BB_PAN_POINT_ACTION(object)));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}


static GVariant *
bb_pan_point_action_get_state(GAction *action)
{
    g_return_val_if_fail(BB_IS_PAN_POINT_ACTION(action), NULL);

    return NULL;
}


static GVariant*
bb_pan_point_action_get_state_hint(GAction *action)
{
    g_return_val_if_fail(BB_IS_PAN_POINT_ACTION(action), NULL);

    return NULL;
}


static const GVariantType*
bb_pan_point_action_get_state_type(GAction *action)
{
    g_return_val_if_fail(BB_IS_PAN_POINT_ACTION(action), NULL);

    return NULL;
}


BbDocumentWindow*
bb_pan_point_action_get_subject(BbPanPointAction *action)
{
    g_return_val_if_fail(BB_IS_PAN_POINT_ACTION(action), NULL);

    BbPanPointAction *pan_point_action = BB_PAN_POINT_ACTION(action);
    g_return_val_if_fail(pan_point_action != NULL, NULL);

    return pan_point_action->subject;
}


static void
bb_pan_point_action_init(BbPanPointAction *action)
{
    g_signal_connect(
        action,
        "notify::subject",
        G_CALLBACK(bb_pan_point_action_notify),
        action
        );
}


BbPanPointAction*
bb_pan_point_action_new(BbMainWindow *window)
{
    BbPanPointAction *pan_point_action = BB_PAN_POINT_ACTION(g_object_new(
        BB_TYPE_PAN_POINT_ACTION,
        NULL
        ));

    g_object_bind_property(
        window,
        "current-document-window",
        pan_point_action,
        "subject",
        G_BINDING_SYNC_CREATE
        );

    return pan_point_action;
}


static void
bb_pan_point_action_notify(gpointer unused, GParamSpec *pspec, BbPanPointAction *action)
{
    g_return_if_fail(BB_IS_PAN_POINT_ACTION(action));

    g_object_notify_by_pspec(G_OBJECT(action), properties[PROP_ENABLED]);
}


static void
bb_pan_point_action_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    switch (property_id)
    {
        case PROP_SUBJECT:
            bb_pan_point_action_set_subject(BB_PAN_POINT_ACTION(object), g_value_get_object(value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}


void
bb_pan_point_action_set_subject(BbPanPointAction *action, BbDocumentWindow* subject)
{
    g_return_if_fail(BB_IS_PAN_POINT_ACTION(action));

    if (action->subject != subject)
    {
        if (action->subject != NULL)
        {
            g_signal_handlers_disconnect_by_func(
                subject,
                "notify::can-zoom-out",
                G_CALLBACK(bb_pan_point_action_notify)
                );

            g_object_unref(action->subject);
        }

        action->subject = subject;

        if (action->subject != NULL)
        {
            g_object_ref(action->subject);

            g_signal_connect(
                subject,
                "notify::can-zoom-out",
                G_CALLBACK(bb_pan_point_action_notify),
                action
                );
        }

        g_object_notify_by_pspec(G_OBJECT(action), properties[PROP_SUBJECT]);
    }
}
