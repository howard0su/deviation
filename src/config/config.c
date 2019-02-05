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
#include "gui/gui.h"
#include "config.h"
#include "display.h"
#include "mixer.h"

#include <stdlib.h>
#include <string.h>

#define MATCH_KEY(s)     strcasecmp(name,    s) == 0
#define MATCH_VALUE(s)   strcasecmp(value,   s) == 0

extern u8 FONT_GetFromString(const char *);

static u16 get_color(const char *value) {
    u8 r, g, b;
    u32 color = strtol(value, NULL, 16);
    r = 0xff & (color >> 16);
    g = 0xff & (color >> 8);
    b = 0xff & (color >> 0);
    return RGB888_to_RGB565(r, g, b);
}

static u8 get_button(const char *value)
{
    u8 i;
    for (i = 0; i <= NUM_TX_BUTTONS; i++) {
        if (strcasecmp(INPUT_ButtonName(i), value) == 0) {
            return i;
        }
    }
    printf("Could not parse Button %s\n", value);
    return 0;
}


static s8 mapstrcasecmp(const char *s1, const char *s2)
{
    int i = 0;
    while (1) {
        if (s1[i] == s2[i]
           || (s2[i] >= 'a' && s1[i] + ('a'-'A') == s2[i])
           || (s1[i] >= 'a' && s2[i] + ('a'-'A') == s1[i])
           || (s2[i] == ' ' && s1[i] == '_')
           || (s1[i] == ' ' && s2[i] == '_'))
        {
            if (s1[i] == '\0')
                return 0;
            i++;
            continue;
        }
        return (s1[i] < s2[i] ? -1 : 1);
    }
}

u8 get_source(const char *value)
{
    unsigned i;
    unsigned val;
    const char *ptr = (value[0] == '!') ? value + 1 : value;
    const char *tmp;
    char cmp[10];
    for (i = 0; i <= NUM_SOURCES; i++) {
        if (mapstrcasecmp(INPUT_SourceNameReal(cmp, i), ptr) == 0) {
            #if defined(HAS_SWITCHES_NOSTOCK) && HAS_SWITCHES_NOSTOCK
            #define SWITCH_NOSTOCK ((1 << INP_HOLD0) | (1 << INP_HOLD1) | \
                                    (1 << INP_FMOD0) | (1 << INP_FMOD1))
            if ((Transmitter.ignore_src & SWITCH_NOSTOCK) == SWITCH_NOSTOCK) {
                if (mapstrcasecmp("FMODE0", ptr) == 0 ||
                    mapstrcasecmp("FMODE1", ptr) == 0 ||
                    mapstrcasecmp("HOLD0", ptr) == 0 ||
                    mapstrcasecmp("HOLD1", ptr) == 0)
                    break;
            }
            #endif  // HAS_SWITCHES_NOSTOCK
            return ((ptr == value) ? 0 : 0x80) | i;
        }
    }
    for (i = 0; i < 4; i++) {
        if (mapstrcasecmp(tx_stick_names[i], ptr) == 0) {
            return ((ptr == value) ? 0 : 0x80) | (i + 1);
        }
    }
    i = 0;
    while ((tmp = INPUT_MapSourceName(i++, &val))) {
        if (mapstrcasecmp(tmp, ptr) == 0) {
            return ((ptr == value) ? 0 : 0x80) | val;
        }
    }
    printf("Could not parse Source %s\n", value);
    return 0;
}

int assign_int(void* ptr, const struct struct_map *map, int map_size, const char* name, const char* value) {
    for (int i = 0; i < map_size; i++) {
        if (MATCH_KEY(map[i].str)) {
            int size = map[i].offset >> 12;
            int offset = map[i].offset & 0xFFF;
            switch (size) {
                case TYPE_S8:
                    *((s8 *)((u8*)ptr + offset)) = atoi(value); break;
                case TYPE_S16:
                    *((s16 *)((u8*)ptr + offset)) = atoi(value); break;
                case TYPE_S32:
                    *((s32 *)((u8*)ptr + offset)) = atoi(value); break;

                case TYPE_U8:
                    *((u8 *)((u8*)ptr + offset)) = atoi(value); break;
                case TYPE_U16:
                    *((u16 *)((u8*)ptr + offset)) = atoi(value); break;
                case TYPE_U32:
                    *((u32 *)((u8*)ptr + offset)) = atoi(value); break;

                case TYPE_COLOR:
                    *((u16 *)((u8*)ptr + offset)) = get_color(value); break;
                case TYPE_FONT:
                    *((u8 *)((u8*)ptr + offset)) = FONT_GetFromString(value); break;
                case TYPE_BUTTON:
                    *((u8 *)((u8*)ptr + offset)) = get_button(value); break;
                case TYPE_SOURCE:
                    *((u8 *)((u8*)ptr + offset)) = get_source(value); break;

                case TYPE_STR_LIST: {
                    // get the list
                    i++;  // next entry is additional info for string list
                    const char* const *list = (const char* const *)map[i].str;
                    unsigned length = map[i].offset;
                    for (unsigned j = 0; j < length; j++) {
                        if (list[j] && MATCH_VALUE(list[j]))
                            *((u8 *)((u8*)ptr + offset)) = j;
                    }
                    break;
                }
                default:
                    printf("Unknown type: %d\n", size);
            }
            return 1;
        }
    }
    return 0;
}

int write_int(void* ptr, const struct struct_map *map, int map_size, FILE* fh) {
    for (int i = 0; i < map_size; i++) {
        int size = map[i].offset >> 12;
        int offset = map[i].offset & 0xFFF;
        int value;

        switch (size) {
            case TYPE_S8:
                value = *((s8 *)((u8*)ptr + offset)); break;
            case TYPE_S16:
                value = *((s16 *)((u8*)ptr + offset)); break;
            case TYPE_S32:
                value = *((s32 *)((u8*)ptr + offset)); break;

            case TYPE_U8:
                value = *((u8 *)((u8*)ptr + offset)); break;
            case TYPE_U16:
                value = *((u16 *)((u8*)ptr + offset)); break;
            case TYPE_U32:
                value = *((u32 *)((u8*)ptr + offset)); break;
            default:
                if (size == TYPE_STR_LIST)
                {
                    i++;  // next entry is additional info for string list
                    const char* const *list = (const char* const *)map[i].str;
                    u8 index = *((u8 *)((u8*)ptr + offset));
                    fprintf(fh, "%s=%s\n", map[i - 1].str, list[index]);
                }
                continue;
        }
        fprintf(fh, "%s=%d\n", map[i].str, value);
    }

    return 1;
}


#define TESTNAME config
#include "tests.h"
