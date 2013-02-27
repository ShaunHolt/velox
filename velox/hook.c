/* velox: velox/hook.c
 *
 * Copyright (c) 2009, 2010 Michael Forney <mforney@mforney.org>
 *
 * This file is a part of velox.
 *
 * velox is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License version 2, as published by the Free
 * Software Foundation.
 *
 * velox is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along
 * with velox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "velox.h"
#include "hook.h"

const uint16_t velox_hook_types = 10;

/* Manage hooks */
void handle_floating(union velox_argument argument);
void handle_fullscreen(union velox_argument argument);

struct velox_list * hooks;

void setup_hooks()
{
    uint32_t index;

    hooks = (struct velox_list *) malloc(velox_hook_types * sizeof(struct velox_list));

    for (index = 0; index < velox_hook_types; ++index)
    {
        list_init(&hooks[index]);
    }

    // TODO: Should these be a part of some plugin instead?
    add_hook(&handle_floating, VELOX_HOOK_MANAGE_PRE);
    add_hook(&handle_fullscreen, VELOX_HOOK_MANAGE_PRE);
}

void cleanup_hooks()
{
    uint16_t index = 0;

    if (hooks != NULL)
    {
        struct velox_hook_entry * hook;
        struct velox_link * tmp;

        for (index = 0; index < velox_hook_types; ++index)
        {
            list_for_each_entry_safe(&hooks[index], hook, tmp)
            {
                free(hook);
            }
        }

        free(hooks);
    }
}

void add_hook(velox_function_t hook, enum velox_hook_type type)
{
    struct velox_hook_entry * entry;

    entry = (struct velox_hook_entry *) malloc(sizeof(struct velox_hook_entry));
    entry->hook = hook;
    list_append(&hooks[type], entry);
}

void run_hooks(union velox_argument arg, enum velox_hook_type type)
{
    struct velox_hook_entry * entry;

    list_for_each_entry(&hooks[type], entry)
    {
        entry->hook(arg);
    }
}

/* Manage hooks */
void handle_floating(union velox_argument argument)
{
    struct velox_window * window = (struct velox_window *) argument.pointer;

    /* TODO: Make download konqueror windows floating */
    if (strcmp(window->name, "MPlayer") == 0)
    {
        window->floating = true;
    }
    else if (strcmp(window->name, "xclock") == 0)
    {
        window->floating = true;
    }
}

void handle_fullscreen(union velox_argument argument)
{
    struct velox_window * window = (struct velox_window *) argument.pointer;

    if (window->width == screen_area.width && window->height == screen_area.height)
    {
        window->x = 0;
        window->y = 0;
        window->border_width = 0;
        window->floating = true;
    }
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

