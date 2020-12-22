#ifndef __BBITEMRENDERER__
#define __BBITEMRENDERER__
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
#include "bbfillstyle.h"
#include "bblinestyle.h"

/* forward declarations */

#define BB_TYPE_ITEM_RENDERER bb_item_renderer_get_type()
G_DECLARE_INTERFACE(BbItemRenderer, bb_item_renderer, BB, ITEM_RENDERER, GObject)

struct _BbItemRendererInterface
{
    GTypeInterface parent;

    void (*close_path)(BbItemRenderer *renderer);
    void (*render_absolute_line_to)(BbItemRenderer *renderer, int x, int y);
    void (*render_absolute_move_to)(BbItemRenderer *renderer, int x, int y);
    void (*render_arc)(BbItemRenderer *renderer, int x, int y, int radius, int start, int sweep);
    void (*render_relative_line_to)(BbItemRenderer *renderer, int dx, int dy);
    void (*render_relative_move_to)(BbItemRenderer *renderer, int dx, int dy);

    void (*render_text)(
        BbItemRenderer *renderer,
        int insert_x,
        int insert_y,
        char *text
        );

    void (*set_color)(BbItemRenderer *renderer, int color);
    void (*set_fill_style)(BbItemRenderer *renderer, BbFillStyle *style);
    void (*set_line_style)(BbItemRenderer *renderer, BbLineStyle *style);
};


void
bb_item_renderer_close_path(BbItemRenderer *renderer);


void
bb_item_renderer_render_absolute_line_to(BbItemRenderer *renderer, int x, int y);


void
bb_item_renderer_render_absolute_move_to(BbItemRenderer *renderer, int x, int y);


void
bb_item_renderer_render_arc(BbItemRenderer *renderer, int x, int y, int radius, int start, int sweep);


void
bb_item_renderer_render_relative_line_to(BbItemRenderer *renderer, int dx, int dy);


void
bb_item_renderer_render_relative_move_to(BbItemRenderer *renderer, int dx, int dy);


void
bb_item_renderer_render_text(
    BbItemRenderer *renderer,
    int insert_x,
    int insert_y,
    char *text
    );

void
bb_item_renderer_set_color(BbItemRenderer *renderer, int color);


void
bb_item_renderer_set_fill_style(BbItemRenderer *renderer, BbFillStyle *style);


void
bb_item_renderer_set_line_style(BbItemRenderer *renderer, BbLineStyle *style);


#endif
