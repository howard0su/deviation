/*
 This project is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Deviation is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Deviation.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "common.h"
#include "pages.h"
#include "gui/gui.h"
#include "config/tx.h"
#include <string.h>

#define OVERRIDE_PLACEMENT
enum {
    BUTTON_WIDTH = 10,

    BUTTON1_X = 2,
    BUTTON1_Y = 2,
    BUTTON2_X = 2,
    BUTTON2_Y = 6,
    BUTTON3_X = 2,
    BUTTON3_Y = 10,

    GRAPH_X_OFFSET = 0,
    GRAPH_Y_OFFSET = 0,
    GRAPH_WIDTH = 108,
    GRAPH_HEIGHT = 48,
};
#include "../128x64x1/scanner_page.c"
