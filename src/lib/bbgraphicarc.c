/*
 * bbsch
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
#include "bbgraphicarc.h"
#include "bbcoord.h"
#include "bbitemparams.h"


enum
{
    PROP_0,
    PROP_CENTER_X,
    PROP_CENTER_Y,
    PROP_RADIUS,
    PROP_START_ANGLE,
    PROP_SWEEP_ANGLE,
    PROP_WIDTH,
    N_PROPERTIES
};


struct _BbGraphicArc
{
    BbSchematicItem parent;

    BbItemParams *params;

    int center_x;
    int center_y;

    int radius;

    int start_angle;
    int sweep_angle;

    int color;

    BbLineStyle *line_style;
};


G_DEFINE_TYPE(BbGraphicArc, bb_graphic_arc, BB_TYPE_SCHEMATIC_ITEM)


static BbBounds*
bb_graphic_arc_calculate_bounds(BbSchematicItem *item, BbBoundsCalculator *calculator);

static void
bb_graphic_arc_dispose(GObject *object);

static void
bb_graphic_arc_finalize(GObject *object);

static void
bb_graphic_arc_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
bb_graphic_arc_render(BbSchematicItem *item, BbItemRenderer *renderer);

static void
bb_graphic_arc_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void
bb_graphic_arc_translate(BbSchematicItem *item, int dx, int dy);

static gboolean
bb_graphic_arc_write(BbSchematicItem *item, GOutputStream *stream, GCancellable *cancellable, GError **error);

static void
bb_graphic_arc_write_async(
    BbSchematicItem *item,
    GOutputStream *stream,
    int io_priority,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer callback_data
    );

static gboolean
bb_graphic_arc_write_finish(
    BbSchematicItem *item,
    GOutputStream *stream,
    GAsyncResult *result,
    GError **error
    );


GParamSpec *properties[N_PROPERTIES];


static BbBounds*
bb_graphic_arc_calculate_bounds(BbSchematicItem *item, BbBoundsCalculator *calculator)
{
    BbGraphicArc *arc = BB_GRAPHIC_ARC(item);

    g_return_val_if_fail(arc != NULL, NULL);
    g_return_val_if_fail(arc->line_style != NULL, NULL);

    return bb_bounds_calculator_calculate_from_corners(
        calculator,
        arc->center_x - arc->radius,
        arc->center_y - arc->radius,
        arc->center_x + arc->radius,
        arc->center_y + arc->radius,
        arc->line_style->line_width
        );
}


static void
bb_graphic_arc_class_init(BbGraphicArcClass *klasse)
{
    G_OBJECT_CLASS(klasse)->dispose = bb_graphic_arc_dispose;
    G_OBJECT_CLASS(klasse)->finalize = bb_graphic_arc_finalize;
    G_OBJECT_CLASS(klasse)->get_property = bb_graphic_arc_get_property;
    G_OBJECT_CLASS(klasse)->set_property = bb_graphic_arc_set_property;

    BB_SCHEMATIC_ITEM_CLASS(klasse)->calculate_bounds = bb_graphic_arc_calculate_bounds;
    BB_SCHEMATIC_ITEM_CLASS(klasse)->render = bb_graphic_arc_render;
    BB_SCHEMATIC_ITEM_CLASS(klasse)->translate = bb_graphic_arc_translate;
    BB_SCHEMATIC_ITEM_CLASS(klasse)->write = bb_graphic_arc_write;
    BB_SCHEMATIC_ITEM_CLASS(klasse)->write_async = bb_graphic_arc_write_async;
    BB_SCHEMATIC_ITEM_CLASS(klasse)->write_finish = bb_graphic_arc_write_finish;

    properties[PROP_CENTER_X] = g_param_spec_int(
        "center-x",
        "Center X",
        "The x coordinate of the center",
        INT_MIN,
        INT_MAX,
        0,
        G_PARAM_READWRITE
        );

    properties[PROP_CENTER_Y] = g_param_spec_int(
        "center-y",
        "Center Y",
        "The y coordinate of the center",
        INT_MIN,
        INT_MAX,
        0,
        G_PARAM_READWRITE
        );

    properties[PROP_RADIUS] = g_param_spec_int(
        "radius",
        "Radius",
        "The radius of the arc",
        0,
        INT_MAX,
        0,
        G_PARAM_READWRITE
        );

    properties[PROP_START_ANGLE] = g_param_spec_int(
        "start-angle",
        "Start Angle",
        "The staring angle",
        INT_MIN,
        INT_MAX,
        0,
        G_PARAM_READWRITE
        );

    properties[PROP_SWEEP_ANGLE] = g_param_spec_int(
        "sweep-angle",
        "Sweep Angle",
        "The sweep angle",
        INT_MIN,
        INT_MAX,
        90,
        G_PARAM_READWRITE
        );

    properties[PROP_WIDTH] = g_param_spec_int(
        "width",
        "Line Width",
        "The line width",
        0,
        INT_MAX,
        0,
        G_PARAM_READWRITE
        );

    for (int index = PROP_0 + 1; index < N_PROPERTIES; ++index)
    {
        g_object_class_install_property(
            G_OBJECT_CLASS(klasse),
            index,
            properties[index]
            );
    }
}


static void
bb_graphic_arc_dispose(GObject *object)
{
}


static void
bb_graphic_arc_finalize(GObject *object)
{
    BbGraphicArc *arc = BB_GRAPHIC_ARC(object);

    g_return_if_fail(arc != NULL);

    bb_line_style_free(arc->line_style);
}


int
bb_graphic_arc_get_center_x(BbGraphicArc *arc)
{
    g_return_val_if_fail(arc != NULL, 0);

    return arc->center_x;
}


int
bb_graphic_arc_get_center_y(BbGraphicArc *arc)
{
    g_return_val_if_fail(arc != NULL, 0);

    return arc->center_y;
}


static void
bb_graphic_arc_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    switch (property_id)
    {
        case PROP_CENTER_X:
            g_value_set_int(value, bb_graphic_arc_get_center_x(BB_GRAPHIC_ARC(object)));
            break;

        case PROP_CENTER_Y:
            g_value_set_int(value, bb_graphic_arc_get_center_y(BB_GRAPHIC_ARC(object)));
            break;

        case PROP_RADIUS:
            g_value_set_int(value, bb_graphic_arc_get_radius(BB_GRAPHIC_ARC(object)));
            break;

        case PROP_START_ANGLE:
            g_value_set_int(value, bb_graphic_arc_get_start_angle(BB_GRAPHIC_ARC(object)));
            break;

        case PROP_SWEEP_ANGLE:
            g_value_set_int(value, bb_graphic_arc_get_sweep_angle(BB_GRAPHIC_ARC(object)));
            break;

        case PROP_WIDTH:
            g_value_set_int(value, bb_graphic_arc_get_width(BB_GRAPHIC_ARC(object)));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}


int
bb_graphic_arc_get_radius(BbGraphicArc *arc)
{
    g_return_val_if_fail(arc != NULL, 0);

    return arc->radius;
}


int
bb_graphic_arc_get_start_angle(BbGraphicArc *arc)
{
    g_return_val_if_fail(arc != NULL, 0);

    return arc->start_angle;
}


int
bb_graphic_arc_get_sweep_angle(BbGraphicArc *arc)
{
    g_return_val_if_fail(arc != NULL, 0);

    return arc->sweep_angle;
}


int
bb_graphic_arc_get_width(BbGraphicArc *arc)
{
    g_return_val_if_fail(arc != NULL, 0);
    g_return_val_if_fail(arc->line_style != NULL, 0);

    return arc->line_style->line_width;
}


static void
bb_graphic_arc_init(BbGraphicArc *arc)
{
    g_return_if_fail(arc != NULL);

    arc->line_style = bb_line_style_new();
}


__attribute__((constructor)) void
bb_graphic_arc_register()
{
    bb_graphic_arc_get_type();
}


static void
bb_graphic_arc_render(BbSchematicItem *item, BbItemRenderer *renderer)
{
    BbGraphicArc *arc = BB_GRAPHIC_ARC(item);

    bb_item_renderer_set_color(renderer, arc->color);
    bb_item_renderer_set_line_style(renderer, arc->line_style);

    bb_item_renderer_render_graphic_arc(renderer, arc);
}


void
bb_graphic_arc_set_center_x(BbGraphicArc *arc, int x)
{
    g_return_if_fail(arc != NULL);

    if (arc->center_x != x)
    {
        arc->center_x = x;

        g_object_notify_by_pspec(G_OBJECT(arc), properties[PROP_CENTER_X]);
    }
}


void
bb_graphic_arc_set_center_y(BbGraphicArc *arc, int y)
{
    g_return_if_fail(arc != NULL);

    if (arc->center_y != y)
    {
        arc->center_y = y;

        g_object_notify_by_pspec(G_OBJECT(arc), properties[PROP_CENTER_Y]);
    }
}


static void
bb_graphic_arc_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    switch (property_id)
    {
        case PROP_CENTER_X:
            bb_graphic_arc_set_center_x(BB_GRAPHIC_ARC(object), g_value_get_int(value));
            break;

        case PROP_CENTER_Y:
            bb_graphic_arc_set_center_y(BB_GRAPHIC_ARC(object), g_value_get_int(value));
            break;

        case PROP_RADIUS:
            bb_graphic_arc_set_radius(BB_GRAPHIC_ARC(object), g_value_get_int(value));
            break;

        case PROP_START_ANGLE:
            bb_graphic_arc_set_start_angle(BB_GRAPHIC_ARC(object), g_value_get_int(value));
            break;

        case PROP_SWEEP_ANGLE:
            bb_graphic_arc_set_sweep_angle(BB_GRAPHIC_ARC(object), g_value_get_int(value));
            break;

        case PROP_WIDTH:
            bb_graphic_arc_set_width(BB_GRAPHIC_ARC(object), g_value_get_int(value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}


void
bb_graphic_arc_set_radius(BbGraphicArc *arc, int radius)
{
    g_return_if_fail(arc != NULL);

    if (arc->radius != radius)
    {
        arc->radius = radius;

        g_object_notify_by_pspec(G_OBJECT(arc), properties[PROP_RADIUS]);
    }
}


void
bb_graphic_arc_set_start_angle(BbGraphicArc *arc, int angle)
{
    g_return_if_fail(arc != NULL);

    if (arc->start_angle != angle)
    {
        arc->start_angle = angle;

        g_object_notify_by_pspec(G_OBJECT(arc), properties[PROP_START_ANGLE]);
    }
}


void
bb_graphic_arc_set_sweep_angle(BbGraphicArc *arc, int angle)
{
    g_return_if_fail(arc != NULL);

    if (arc->sweep_angle != angle)
    {
        arc->sweep_angle = angle;

        g_object_notify_by_pspec(G_OBJECT(arc), properties[PROP_SWEEP_ANGLE]);
    }
}


void
bb_graphic_arc_set_width(BbGraphicArc *arc, int width)
{
    g_return_if_fail(arc != NULL);
    g_return_if_fail(arc->line_style != NULL);

    if (arc->line_style->line_width != width)
    {
        arc->line_style->line_width = width;

        g_object_notify_by_pspec(G_OBJECT(arc), properties[PROP_WIDTH]);
    }
}


static void
bb_graphic_arc_translate(BbSchematicItem *item, int dx, int dy)
{
    BbGraphicArc *arc = BB_GRAPHIC_ARC(item);
    g_return_if_fail(arc != NULL);

    bb_coord_translate(dx, dy, &arc->center_x, &arc->center_y, 1);

    g_object_notify_by_pspec(G_OBJECT(arc), properties[PROP_CENTER_X]);
    g_object_notify_by_pspec(G_OBJECT(arc), properties[PROP_CENTER_Y]);
}


static gboolean
bb_graphic_arc_write(BbSchematicItem *item, GOutputStream *stream, GCancellable *cancellable, GError **error)
{
    BbGraphicArc *arc = BB_GRAPHIC_ARC(item);
    g_return_val_if_fail(arc != NULL, FALSE);

    bb_item_params_write(
        arc->params,
        stream,
        cancellable,
        error
        );
}


static void
bb_graphic_arc_write_async(
    BbSchematicItem *item,
    GOutputStream *stream,
    int io_priority,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer callback_data
    )
{
    BbGraphicArc *arc = BB_GRAPHIC_ARC(item);

    bb_item_params_write_async(
        arc->params,
        stream,
        io_priority,
        cancellable,
        callback,
        callback_data
        );
}


static gboolean
bb_graphic_arc_write_finish(
    BbSchematicItem *item,
    GOutputStream *stream,
    GAsyncResult *result,
    GError **error
    )
{
    return g_output_stream_write_all_finish(
        stream,
        result,
        NULL,
        error
        );
}
