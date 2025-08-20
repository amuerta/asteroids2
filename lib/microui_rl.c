#include "microui.h"
#include "microui.c"

#define MU_RL_FONT_SIZE 10
#define MU_ICON_BYTES   8
#define MU_ICON_SIZE    16

#ifndef MU_SCROLL_SPEED
#   define MU_SCROLL_SPEED -15
#endif

// Raygui icons for default icons
static unsigned int MU_RL_DEFAULT_ICONS_BYTES[MU_ICON_MAX*MU_ICON_BYTES] = {
    // NULL
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    // CLOSE
    0x00000000, 0x10080000, 0x04200810, 0x01800240, 0x02400180, 0x08100420, 0x00001008, 0x00000000,
    // CHECK
    0x00000000, 0x00000000, 0x10000000, 0x04000800, 0x01040200, 0x00500088, 0x00000020, 0x00000000,  
    // DOWN
    0x00000000, 0x00000000, 0x00000000, 0x0ff81ffc, 0x03e007f0, 0x008001c0, 0x00000000, 0x00000000,
    // UP
    0x00000000, 0x00000000, 0x01c00080, 0x07f003e0, 0x1ffc0ff8, 0x00000000, 0x00000000, 0x00000000, 
};


// copy of GuiDrawIcon function from Raygui
void rl_GuiDrawIcon(int iconId, int posX, int posY, int pixelSize, Color color)
{
    #define BIT_CHECK(a,b) ((a) & (1u<<(b)))
    for (int i = 0, y = 0; i < (MU_ICON_SIZE*MU_ICON_SIZE)/32; i++)
    {
        for (int k = 0; k < 32; k++)
        {
            if (BIT_CHECK(MU_RL_DEFAULT_ICONS_BYTES[iconId*MU_ICON_BYTES + i], k))
            {
                DrawRectangleRec((Rectangle){ 
                        (float)posX + (k%MU_ICON_SIZE)*pixelSize, 
                        (float)posY + y*pixelSize, 
                        (float)pixelSize, 
                        (float)pixelSize 
                        }, 
                        color
                );
            }
            if ((k == 15) || (k == 31)) y++;
        }
    }
}

int mu_rl_text_width(mu_Font font, const char *text, int len) {
    (void) font;
    return MeasureText(TextFormat("%.*s",len, text), MU_RL_FONT_SIZE);
}

int mu_rl_text_height(mu_Font font) {
    (void) font;
    return MU_RL_FONT_SIZE;
}

void mu_rl_init(mu_Context* ctx) {
    mu_init(ctx);
    ctx->text_width = mu_rl_text_width;
    ctx->text_height = mu_rl_text_height;
}


void mu_rl_events(mu_Context* ctx) {


    int ch = 0, key = 0;
    (void) key; // ATTENTION:
    Vector2 mv  = GetMousePosition();
    Vector2 md  = GetMouseDelta();
    float ms    = GetMouseWheelMove();

    mu_input_mousemove(ctx, md.x, md.y);
    mu_input_scroll(ctx, 0, ms * MU_SCROLL_SPEED);


    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        mu_input_mousedown(ctx, mv.x, mv.y, MU_MOUSE_LEFT);
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        mu_input_mousedown(ctx, mv.x, mv.y, MU_MOUSE_RIGHT);
    if (IsMouseButtonUp(MOUSE_BUTTON_LEFT))
        mu_input_mouseup(ctx, mv.x, mv.y, MU_MOUSE_LEFT);
    if (IsMouseButtonUp(MOUSE_BUTTON_RIGHT))
        mu_input_mouseup(ctx, mv.x, mv.y, MU_MOUSE_RIGHT);


    if (IsKeyDown(KEY_LEFT_SHIFT)) mu_input_keydown(ctx, MU_KEY_SHIFT);
    if (IsKeyDown(KEY_ENTER)) mu_input_keydown(ctx, MU_KEY_RETURN);
    if (IsKeyDown(KEY_LEFT_CONTROL)) mu_input_keydown(ctx, MU_KEY_CTRL);
    if (IsKeyDown(KEY_BACKSPACE)) mu_input_keydown(ctx, MU_KEY_BACKSPACE);
    if (IsKeyDown(KEY_LEFT_ALT)) mu_input_keydown(ctx, MU_KEY_ALT);

    if (IsKeyUp(KEY_LEFT_SHIFT)) mu_input_keyup(ctx, MU_KEY_SHIFT);
    if (IsKeyUp(KEY_ENTER)) mu_input_keyup(ctx, MU_KEY_RETURN);
    if (IsKeyUp(KEY_LEFT_CONTROL)) mu_input_keyup(ctx, MU_KEY_CTRL);
    if (IsKeyUp(KEY_BACKSPACE)) mu_input_keyup(ctx, MU_KEY_BACKSPACE);
    if (IsKeyUp(KEY_LEFT_ALT)) mu_input_keyup(ctx, MU_KEY_ALT);


    while((ch = GetCharPressed())) 
        mu_input_text(ctx, TextFormat("%c", ch));
}

void mu_rl_flush(mu_Context* ctx) {
    const mu_Rect unclipped_rect = { 0, 0, 0x1000000, 0x1000000 };
    mu_Command *cmd = NULL;
    while (mu_next_command(ctx, &cmd)) {
        switch(cmd->type) {
            case MU_COMMAND_RECT:
                {   
                    mu_Rect r = cmd->rect.rect;
                    DrawRectangle(r.x,r.y,r.w,r.h, *(Color*)&cmd->rect.color );
                } break;

            case MU_COMMAND_TEXT:
                {   
                    mu_Vec2 v = cmd->text.pos;
                    DrawText(cmd->text.str,v.x,v.y, MU_RL_FONT_SIZE, *(Color*)&cmd->text.color );
                } break;

            case MU_COMMAND_CLIP:
                {   
                    mu_Rect r = cmd->rect.rect;
                    if (memcmp(&unclipped_rect, &cmd->clip.rect, sizeof(mu_Rect))==0) 
                        EndScissorMode();
                    else 
                        BeginScissorMode(r.x,r.y,r.w,r.h);
                } break;

            case MU_COMMAND_ICON:
                {   
                    mu_Rect r = cmd->icon.rect;
                    int pixel_scale = (r.h / 16);
                    int diff = r.h - pixel_scale*16;
                    mu_Vec2 centered = {
                        r.x + diff/2,
                        r.y + diff/2,
                    };
                    rl_GuiDrawIcon(
                            cmd->icon.id, 
                            centered.x, 
                            centered.y, 
                            pixel_scale, 
                            *(Color*)&cmd->icon.color
                    );
                } break;
            default: break;
        }
    }
}

#if 0
int main(void) {
    mu_Context ctx = {0};
    mu_rl_init(&ctx);

    SetTargetFPS(60);
    InitWindow(800,600,"a window");
    
    float   slider = 0;
    float   number = 0;
    static char buffer[1024*64];

    while(!WindowShouldClose()) {
        mu_rl_events(&ctx);
        
        mu_begin(&ctx);
        if (mu_begin_window(&ctx, "Window", mu_rect(15, 15, 400, 300))) {
            mu_layout_row(&ctx, 2, (int[]) { 120, -1 }, 0);

            mu_label(&ctx, "Button:");
            if (mu_button(&ctx, "Button1")) {
                printf("Button1 pressed\n");
            }

            mu_label(&ctx, "Button the Second:");
            if (mu_button(&ctx, "Button2")) {
                mu_open_popup(&ctx, "My Popup");
            }

            if (mu_begin_popup(&ctx, "My Popup")) {
                mu_label(&ctx, "Hello world!");
                mu_end_popup(&ctx);
            }

            mu_label(&ctx, "Slider:");
            mu_slider(&ctx, &slider,0, 1337);


            mu_label(&ctx, "Number:");
            mu_number(&ctx, &number, 2);

            mu_layout_row(&ctx, 1, (int[]){-1}, 0);
            
            if(mu_header(&ctx,"Header: see yapping")) {
                mu_text(&ctx, 
                        "A very long string of text that definitely could not fit in one single line"
                        " and would requite a linebreak of some sort, also is very boring to read (_._)zzZ."
                );
            }

            mu_label(&ctx, "textbox:");
            mu_textbox(&ctx, buffer, 1024*64);
            
            //mu_layout_row(&ctx, 1, (int[]){-1}, 64);
            mu_layout_height(&ctx, 64);
            mu_begin_panel(&ctx, "pan");
            mu_text(&ctx, buffer);
            mu_end_panel(&ctx);

            mu_end_window(&ctx);
        }
        mu_end(&ctx);

        BeginDrawing(); {
            ClearBackground(GetColor(0x141414FF));
            mu_rl_flush(&ctx);
        } EndDrawing();

    }
    CloseWindow();
}
#endif
