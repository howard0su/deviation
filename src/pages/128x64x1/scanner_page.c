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

#ifndef OVERRIDE_PLACEMENT
#include "common.h"
#include "pages.h"
#include "config/model.h"

enum {
    BUTTON_WIDTH = 40,

    BUTTON1_X = 0,
    BUTTON1_Y = 12,
    BUTTON2_X = LCD_WIDTH/2 - BUTTON_WIDTH/2,
    BUTTON2_Y = 12,
    BUTTON3_X = LCD_WIDTH - 40,
    BUTTON3_Y = 12,

    GRAPH_X_OFFSET = 0,
    GRAPH_Y_OFFSET = 24,
    GRAPH_WIDTH = LCD_WIDTH,
    GRAPH_HEIGHT = LCD_HEIGHT - GRAPH_Y_OFFSET,
};

#endif

#if HAS_SCANNER
#include "../common/_scanner_page.c"
#include "_mapped_gfx.h"

static struct scanner_obj * const gui = &gui_objs.u.scanner;
static const char *enablestr_cb(guiObject_t *obj, const void *data)
{
    (void)obj;
    (void)data;
    return sp->enable ? _tr("On") : _tr("Off");
}

static const char *modestr_cb(guiObject_t *obj, const void *data)
{
    (void)obj;
    (void)data;
    return sp->scan_mode ? _tr("Average") : _tr("Peak");
}

static const char *attstr_cb(guiObject_t *obj, const void *data)
{
    (void)obj;
    (void)data;
    return sp->attenuator ? _tr("-20dB") : _tr("0dB");
}

void _draw_page(u8 enable)
{
    (void)enable;
    PAGE_ShowHeader(PAGE_GetName(PAGEID_SCANNER));
    GUI_CreateButtonPlateText(&gui->enable,     BUTTON1_X, BUTTON1_Y, BUTTON_WIDTH, LINE_HEIGHT, &BUTTON_FONT, enablestr_cb, press_enable_cb, NULL);
    GUI_CreateButtonPlateText(&gui->scan_mode,  BUTTON2_X, BUTTON2_Y, BUTTON_WIDTH, LINE_HEIGHT, &BUTTON_FONT, modestr_cb, press_mode_cb, NULL);
    GUI_CreateButtonPlateText(&gui->attenuator, BUTTON3_X, BUTTON3_Y, BUTTON_WIDTH, LINE_HEIGHT, &BUTTON_FONT, attstr_cb, press_attenuator_cb, NULL);

#ifdef HAS_MAPPED_GFX
    LCD_CreateMappedWindow(1, 46, 5, 6, 4);
#endif
}

#ifdef HAS_MAPPED_GFX
void _draw_channels()
{
    _GUI_DrawMappedStart();
    for(u8 i = 0; i < MAX_RADIOCHANNEL - MIN_RADIOCHANNEL + 1; i++)
    {
        // draw a line
        int col = (GRAPH_WIDTH - (MAX_RADIOCHANNEL - MIN_RADIOCHANNEL)) / 2 + i;
        int height = sp->channelnoise[i] * GRAPH_HEIGHT / 0x1F;

        printf("%d %d\n", col, height);
        LCD_DrawFastVLine(GRAPH_X_OFFSET + col, GRAPH_Y_OFFSET + GRAPH_HEIGHT - height, GRAPH_Y_OFFSET + GRAPH_HEIGHT, Display.xygraph.grid_color);
    }
    _GUI_DrawMappedStop();
}
#else
void _draw_channels()
{
    // draw a line
    int col = (GRAPH_WIDTH - (MAX_RADIOCHANNEL - MIN_RADIOCHANNEL)) / 2 + sp->channel;
    int height = sp->channelnoise[sp->channel] * GRAPH_HEIGHT / 0x1F;

    LCD_DrawFastVLine(GRAPH_X_OFFSET + col, GRAPH_Y_OFFSET + 0, GRAPH_Y_OFFSET +  GRAPH_HEIGHT - height, Display.xygraph.bg_color);
    LCD_DrawFastVLine(GRAPH_X_OFFSET + col, GRAPH_Y_OFFSET + GRAPH_HEIGHT - height, GRAPH_Y_OFFSET + GRAPH_HEIGHT, Display.xygraph.grid_color);
}
#endif
#endif //HAS_SCANNER
