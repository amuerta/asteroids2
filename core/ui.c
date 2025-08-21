#include "depend.h"

#ifndef __A2_UI_H
#define __A2_UI_H

void a2_ui_set_style(mu_Context* ctx, const char* selector, Color color) {
    static struct { const char *label; int idx; } colors[] = {
        { "text",         MU_COLOR_TEXT        },
        { "border",       MU_COLOR_BORDER      },
        { "windowbg",     MU_COLOR_WINDOWBG    },
        { "titlebg",      MU_COLOR_TITLEBG     },
        { "titletext",    MU_COLOR_TITLETEXT   },
        { "panelbg",      MU_COLOR_PANELBG     },
        { "button",       MU_COLOR_BUTTON      },
        { "buttonhover",  MU_COLOR_BUTTONHOVER },
        { "buttonfocus",  MU_COLOR_BUTTONFOCUS },
        { "base",         MU_COLOR_BASE        },
        { "basehover",    MU_COLOR_BASEHOVER   },
        { "basefocus",    MU_COLOR_BASEFOCUS   },
        { "scrollbase",   MU_COLOR_SCROLLBASE  },
        { "scrollthumb",  MU_COLOR_SCROLLTHUMB },
        { NULL, -1 }
    };
    for(int i = 0; colors[i].label; i++) {
        const char* l = colors[i].label;
        if (l && selector && strcmp(l, selector)==0) {
            memcpy(ctx->style->colors + i,&color, sizeof(color));
        }
    }
}

#endif
