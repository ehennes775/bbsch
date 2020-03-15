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
#include <src/lib/bbgraphiccircle.h>
#include "bbcirclebuilder.h"


enum
{
    PROP_0,
    PROP_X0,
    PROP_X1,
    PROP_Y0,
    PROP_Y1,
    N_PROPERTIES
};


struct _BbCircleBuilder
{
    BbItemBuilder parent;

    BbGraphicCircle *prototype;

    int x[2];
    int y[2];
};


static GSList*
bb_circle_builder_create_items(BbItemBuilder *builder);

static void
bb_circle_builder_dispose(GObject *object);

static void
bb_circle_builder_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

static void
bb_circle_builder_put_point(BbItemBuilder *builder, int index, int x, int y);

static void
bb_circle_builder_render_items(BbItemBuilder *builder, BbItemRenderer *renderer);

static void
bb_circle_builder_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);


G_DEFINE_TYPE(BbCircleBuilder, bb_circle_builder, BB_TYPE_ITEM_BUILDER)


static GParamSpec *properties[N_PROPERTIES];


static void
bb_circle_builder_class_init(BbCircleBuilderClass *class)
{
    g_return_if_fail(G_OBJECT_CLASS(class) != NULL);
    g_return_if_fail(BB_ITEM_BUILDER_CLASS(class) != NULL);

    G_OBJECT_CLASS(class)->dispose = bb_circle_builder_dispose;
    G_OBJECT_CLASS(class)->get_property = bb_circle_builder_get_property;
    G_OBJECT_CLASS(class)->set_property = bb_circle_builder_set_property;

    BB_ITEM_BUILDER_CLASS(class)->create_items = bb_circle_builder_create_items;
    BB_ITEM_BUILDER_CLASS(class)->put_point = bb_circle_builder_put_point;
    BB_ITEM_BUILDER_CLASS(class)->render_items = bb_circle_builder_render_items;

    properties[PROP_X0] = g_param_spec_int(
        "x0",
        "x0",
        "The starting X coordinate",
        G_MININT,
        G_MAXINT,
        0,
        G_PARAM_READWRITE
        );

    properties[PROP_X1] = g_param_spec_int(
        "x1",
        "x1",
        "The ending X coordinate",
        G_MININT,
        G_MAXINT,
        0,
        G_PARAM_READWRITE
        );

    properties[PROP_Y0] = g_param_spec_int(
        "y0",
        "y0",
        "The starting Y coordinate",
        G_MININT,
        G_MAXINT,
        0,
        G_PARAM_READWRITE
        );

    properties[PROP_Y1] = g_param_spec_int(
        "y1",
        "y1",
        "The ending Y coordinate",
        G_MININT,
        G_MAXINT,
        0,
        G_PARAM_READWRITE
        );

    for (int index = PROP_0 + 1; index < N_PROPERTIES; ++index)
    {
        g_object_class_install_property(
            G_OBJECT_CLASS(class),
            index,
            properties[index]
            );
    }
}


static GSList*
bb_circle_builder_create_items(BbItemBuilder *builder)
{
    GSList *items = NULL;
    BbCircleBuilder *circle_builder = BB_CIRCLE_BUILDER(builder);

    g_return_val_if_fail(circle_builder != NULL, items);

    if (bb_schematic_item_is_significant(BB_SCHEMATIC_ITEM(circle_builder->prototype)))
    {
        items = g_slist_append(
            items,
            bb_schematic_item_clone(BB_SCHEMATIC_ITEM(circle_builder->prototype))
        );
    }

    return items;
}


static void
bb_circle_builder_dispose(GObject *object)
{
    BbCircleBuilder *circle_builder = BB_CIRCLE_BUILDER(object);

    g_return_if_fail(circle_builder != NULL);

    g_clear_object(&circle_builder->prototype);
}


int
bb_circle_builder_get_x0(BbCircleBuilder *builder)
{
    g_return_val_if_fail(builder != NULL, 0);

    return builder->x[0];
}


int
bb_circle_builder_get_x1(BbCircleBuilder *builder)
{
    g_return_val_if_fail(builder != NULL, 0);

    return builder->x[1];
}


int
bb_circle_builder_get_y0(BbCircleBuilder *builder)
{
    g_return_val_if_fail(builder != NULL, 0);

    return builder->y[0];
}


int
bb_circle_builder_get_y1(BbCircleBuilder *builder)
{
    g_return_val_if_fail(builder != NULL, 0);

    return builder->y[1];
}


static void
bb_circle_builder_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    switch (property_id)
    {
        case PROP_X0:
            g_value_set_int(value, bb_circle_builder_get_x0(BB_CIRCLE_BUILDER(object)));
            break;

        case PROP_X1:
            g_value_set_int(value, bb_circle_builder_get_x1(BB_CIRCLE_BUILDER(object)));
            break;

        case PROP_Y0:
            g_value_set_int(value, bb_circle_builder_get_y0(BB_CIRCLE_BUILDER(object)));
            break;

        case PROP_Y1:
            g_value_set_int(value, bb_circle_builder_get_y1(BB_CIRCLE_BUILDER(object)));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}


static void
bb_circle_builder_init(BbCircleBuilder *builder)
{
    g_return_if_fail(builder != NULL);

    builder->prototype = bb_graphic_circle_new();
}


static void
bb_circle_builder_put_point(BbItemBuilder *builder, int index, int x, int y)
{
    BbCircleBuilder *circle_builder = BB_CIRCLE_BUILDER(builder);

    g_return_if_fail(circle_builder != NULL);

    switch (index)
    {
        case 0:
            bb_circle_builder_set_x0(circle_builder, x);
            bb_circle_builder_set_y0(circle_builder, y);
            break;

        case 1:
            bb_circle_builder_set_x1(circle_builder, x);
            bb_circle_builder_set_y1(circle_builder, y);
            break;

        default:
            g_error("index out of range");
    }
}


__attribute__((constructor)) void
bb_circle_builder_register()
{
}


static void
bb_circle_builder_render_items(BbItemBuilder *builder, BbItemRenderer *renderer)
{
    BbCircleBuilder *circle_builder = BB_CIRCLE_BUILDER(builder);

    g_return_if_fail(circle_builder != NULL);

    bb_schematic_item_render(
        BB_SCHEMATIC_ITEM(circle_builder->prototype),
        renderer
        );
}


void
bb_circle_builder_set_x0(BbCircleBuilder *builder, int x)
{
    g_return_if_fail(builder != NULL);

    if (builder->x[0] != x)
    {
        builder->x[0] = x;

        g_object_notify_by_pspec(G_OBJECT(builder), properties[PROP_X0]);
    }
}


void
bb_circle_builder_set_x1(BbCircleBuilder *builder, int x)
{
    g_return_if_fail(builder != NULL);

    if (builder->x[1] != x)
    {
        builder->x[1] = x;

        g_object_notify_by_pspec(G_OBJECT(builder), properties[PROP_X1]);
    }
}


void
bb_circle_builder_set_y0(BbCircleBuilder *builder, int y)
{
    g_return_if_fail(builder != NULL);

    if (builder->y[0] != y)
    {
        builder->y[0] = y;

        g_object_notify_by_pspec(G_OBJECT(builder), properties[PROP_Y0]);
    }
}


void
bb_circle_builder_set_y1(BbCircleBuilder *builder, int y)
{
    g_return_if_fail(builder != NULL);

    if (builder->y[1] != y)
    {
        builder->y[1] = y;

        g_object_notify_by_pspec(G_OBJECT(builder), properties[PROP_Y1]);
    }
}


static void
bb_circle_builder_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    switch (property_id)
    {
        case PROP_X0:
            bb_circle_builder_set_x0(BB_CIRCLE_BUILDER(object), g_value_get_int(value));
            break;

        case PROP_X1:
            bb_circle_builder_set_x1(BB_CIRCLE_BUILDER(object), g_value_get_int(value));
            break;

        case PROP_Y0:
            bb_circle_builder_set_y0(BB_CIRCLE_BUILDER(object), g_value_get_int(value));
            break;

        case PROP_Y1:
            bb_circle_builder_set_y1(BB_CIRCLE_BUILDER(object), g_value_get_int(value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}