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
#include <bblibrary.h>
#include "bblinetool.h"
#include "bbdrawingtool.h"


enum
{
    PROP_0,
    PROP_ITEM,
    PROP_SUBJECT,
    N_PROPERTIES
};


enum
{
    SIG_INVALIDATE_ITEM,
    N_SIGNALS
};


enum
{
    STATE_S0,
    STATE_S1
};


struct _BbLineTool
{
    GObject parent;

    BbGraphicLine *item;

    int state;

    BbToolSubject *subject;
};


static gboolean
bb_line_tool_button_pressed(BbDrawingTool *tool, double x, double y);

static void
bb_line_tool_dispose(GObject *object);

static void
bb_line_tool_draw(BbDrawingTool *tool);

static void
bb_line_tool_drawing_tool_init(BbDrawingToolInterface *iface);

static void
bb_line_tool_finalize(GObject *object);

static void
bb_line_tool_finish(BbLineTool *line_tool);

static BbGraphicLine*
bb_line_tool_get_item(BbLineTool *tool);

static void
bb_line_tool_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static BbToolSubject*
bb_line_tool_get_subject(BbLineTool *tool);

static void
bb_line_tool_invalidate_item_cb(BbSchematicItem *item, BbLineTool *line_tool);

static void
bb_line_tool_key_pressed(BbDrawingTool *tool);

static void
bb_line_tool_key_released(BbDrawingTool *tool);

static gboolean
bb_line_tool_motion_notify(BbDrawingTool *tool, double x, double y);

static void
bb_line_tool_reset(BbLineTool *line_tool);

static void
bb_line_tool_reset_with_point(BbLineTool *tool, double x, double y);

static void
bb_line_tool_set_item(BbLineTool *tool, BbGraphicLine *item);

static void
bb_line_tool_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void
bb_line_tool_set_subject(BbLineTool *tool, BbToolSubject *subject);

static void
bb_line_tool_update_with_point(BbLineTool *line_tool, double x, double y);


static GParamSpec *properties[N_PROPERTIES];
static guint signals[N_SIGNALS];


G_DEFINE_TYPE_WITH_CODE(
    BbLineTool,
    bb_line_tool,
    G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE(BB_TYPE_DRAWING_TOOL, bb_line_tool_drawing_tool_init)
    )


static gboolean
bb_line_tool_button_pressed(BbDrawingTool *tool, gdouble x, gdouble y)
{
    BbLineTool *line_tool = BB_LINE_TOOL(tool);
    g_return_val_if_fail(line_tool != NULL, FALSE);

    switch (line_tool->state)
    {
        case STATE_S0:
            bb_line_tool_reset_with_point(line_tool, x, y);
            break;

        case STATE_S1:
            bb_line_tool_update_with_point(line_tool, x, y);
            bb_line_tool_finish(line_tool);
            break;

        default:
            g_return_val_if_reached(FALSE);
    }

    return TRUE;
}


static void
bb_line_tool_class_init(BbLineToolClass *klasse)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klasse);
    g_return_if_fail(object_class != NULL);

    object_class->dispose = bb_line_tool_dispose;
    object_class->finalize = bb_line_tool_finalize;
    object_class->get_property = bb_line_tool_get_property;
    object_class->set_property = bb_line_tool_set_property;

    properties[PROP_ITEM] = bb_object_class_install_property(
        object_class,
        PROP_ITEM,
        g_param_spec_object(
            "item",
            "",
            "",
            BB_TYPE_GRAPHIC_LINE,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
            )
        );

    properties[PROP_SUBJECT] = bb_object_class_install_property(
        object_class,
        PROP_SUBJECT,
        g_param_spec_object(
            "subject",
            "",
            "",
            BB_TYPE_TOOL_SUBJECT,
            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS
            )
        );

    signals[SIG_INVALIDATE_ITEM] = g_signal_lookup(
        "invalidate-item",
        BB_TYPE_DRAWING_TOOL
        );
}


static void
bb_line_tool_dispose(GObject *object)
{
    BbLineTool *line_tool = BB_LINE_TOOL(object);
    g_return_if_fail(line_tool != NULL);

    bb_line_tool_set_item(line_tool, NULL);
    bb_line_tool_set_subject(line_tool, NULL);
}


static void
bb_line_tool_draw(BbDrawingTool *tool)
{
    g_message("bb_line_tool_draw");
}


static void
bb_line_tool_drawing_tool_init(BbDrawingToolInterface *iface)
{
    g_return_if_fail(iface != NULL);
    
    iface->button_pressed = bb_line_tool_button_pressed;
    iface->draw = bb_line_tool_draw;
    iface->key_pressed = bb_line_tool_key_pressed;
    iface->key_released = bb_line_tool_key_released;
    iface->motion_notify = bb_line_tool_motion_notify;
}


static void
bb_line_tool_finalize(GObject *object)
{
}


static void
bb_line_tool_finish(BbLineTool *line_tool)
{
    g_return_if_fail(line_tool != NULL);

    BbSchematicItem *item = bb_schematic_item_clone(BB_SCHEMATIC_ITEM(line_tool->item));

    bb_tool_subject_add_item(line_tool->subject, item);

    g_clear_object(&item);

    bb_line_tool_reset(line_tool);
}


static BbGraphicLine*
bb_line_tool_get_item(BbLineTool *tool)
{
    g_return_val_if_fail(tool != NULL, NULL);

    return tool->item;
}


static void
bb_line_tool_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    switch (property_id)
    {
        case PROP_ITEM:
            g_value_set_object(value, bb_line_tool_get_item(BB_LINE_TOOL(object)));
            break;

        case PROP_SUBJECT:
            g_value_set_object(value, bb_line_tool_get_subject(BB_LINE_TOOL(object)));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}


static BbToolSubject*
bb_line_tool_get_subject(BbLineTool *tool)
{
    g_return_val_if_fail(tool != NULL, NULL);

    return tool->subject;
}


static void
bb_line_tool_init(BbLineTool *line_tool)
{
    bb_line_tool_reset(line_tool);
}


static void
bb_line_tool_invalidate_item_cb(BbSchematicItem *item, BbLineTool *line_tool)
{
    g_return_if_fail(line_tool != NULL);
    g_return_if_fail(line_tool->item != NULL);
    g_return_if_fail(line_tool->item == BB_GRAPHIC_LINE(item));

    g_signal_emit(line_tool, signals[SIG_INVALIDATE_ITEM], 0, item);
}


static void
bb_line_tool_key_pressed(BbDrawingTool *tool)
{
    g_message("bb_line_tool_key_pressed");
}


static void
bb_line_tool_key_released(BbDrawingTool *tool)
{
    g_message("bb_line_tool_key_released");
}


BbLineTool*
bb_line_tool_new(BbToolSubject *subject)
{
    return BB_LINE_TOOL(g_object_new(
        BB_TYPE_LINE_TOOL,
        "item", g_object_new(BB_TYPE_GRAPHIC_LINE, NULL),
        "subject", subject,
        NULL
        ));
}


static void
bb_line_tool_reset(BbLineTool *line_tool)
{
    g_return_if_fail(line_tool != NULL);

    line_tool->state = STATE_S0;
}


static void
bb_line_tool_reset_with_point(BbLineTool *line_tool, gdouble x, gdouble y)
{
    g_return_if_fail(line_tool != NULL);

    bb_graphic_line_set_x0(line_tool->item, x);
    bb_graphic_line_set_y0(line_tool->item, y);

    bb_graphic_line_set_x1(line_tool->item, x);
    bb_graphic_line_set_y1(line_tool->item, y);

    line_tool->state = STATE_S1;
}


static gboolean
bb_line_tool_motion_notify(BbDrawingTool *tool, gdouble x, gdouble y)
{
    BbLineTool *line_tool = BB_LINE_TOOL(tool);
    g_return_val_if_fail(line_tool != NULL, FALSE);

    bb_line_tool_update_with_point(line_tool, x, y);

    return TRUE;
}


static void
bb_line_tool_set_item(BbLineTool *tool, BbGraphicLine *item)
{
    g_return_if_fail(tool != NULL);

    if (tool->item != item)
    {
        if (tool->item != NULL)
        {
            g_signal_handlers_disconnect_by_data(item, tool);

            g_object_unref(tool->item);
        }

        tool->item = item;

        if (tool->item != NULL)
        {
            g_object_ref(tool->item);

            g_signal_connect_object(
                item,
                "invalidate-item",
                G_CALLBACK(bb_line_tool_invalidate_item_cb),
                tool,
                0
                );
        }

        g_object_notify_by_pspec(G_OBJECT(tool), properties[PROP_ITEM]);
    }
}


static void
bb_line_tool_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    switch (property_id)
    {
        case PROP_ITEM:
            bb_line_tool_set_item(BB_LINE_TOOL(object), BB_GRAPHIC_LINE(g_value_get_object(value)));
            break;

        case PROP_SUBJECT:
            bb_line_tool_set_subject(BB_LINE_TOOL(object), BB_TOOL_SUBJECT(g_value_get_object(value)));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}


static void
bb_line_tool_set_subject(BbLineTool *tool, BbToolSubject *subject)
{
    g_return_if_fail(tool != NULL);

    if (tool->subject != subject)
    {
        if (tool->subject != NULL)
        {
            g_object_unref(tool->subject);
        }

        tool->subject = subject;

        if (tool->subject != NULL)
        {
            g_object_ref(tool->subject);
        }

        g_object_notify_by_pspec(G_OBJECT(tool), properties[PROP_SUBJECT]);
    }
}


static void
bb_line_tool_update_with_point(BbLineTool *line_tool, gdouble x, gdouble y)
{
    g_return_if_fail(line_tool != NULL);
    g_return_if_fail(line_tool->item != NULL);

    if (line_tool->state == STATE_S1)
    {
        bb_graphic_line_set_x1(line_tool->item, x);
        bb_graphic_line_set_y1(line_tool->item, y);
    }
}