#ifndef __BBUNDORECEIVER__
#define __BBUNDORECEIVER__
/*
 * bbschem
 * Copyright (C) 2022 Edward C. Hennessy
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

#define BB_TYPE_UNDO_RECEIVER bb_undo_receiver_get_type()
G_DECLARE_INTERFACE(BbUndoReceiver, bb_undo_receiver, BB, UNDO_RECEIVER, GObject)

struct _BbUndoReceiverInterface
{
    GTypeInterface g_iface;

    gboolean (*can_undo)(BbUndoReceiver *receiver);
    void (*undo)(BbUndoReceiver *receiver);

};

gboolean
bb_undo_receiver_can_undo(BbUndoReceiver *receiver);

void
bb_undo_receiver_undo(BbUndoReceiver *receiver);

#endif
