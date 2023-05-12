#include "window.h"
#include <mouse/mouse.h>
#include <multiboot/boot_info.h>
#include <res/res.h>
#include <libasg/libasg.h>

void init_window(Window *window, char *name, int x, int y, int w, int h)
{
    window->name = name;
    window->x = x;
    window->y = y + 32; 
    window->w = w;
    window->h = h;
}

void window_draw(Window *window)
{
    Font f;
    f.charwidth = 8;
    f.charheight = 16;
    f.pixels = (uint8_t *)deffont;
    set_round_rect(window->x, window->y - 32, window->w, window->h, 6, from_argb(255, 12, 12, 12));
    set_round_rect(window->x, window->y - 32, window->w, 32, 6, from_argb(255,24,24,24));
    set_string(window->name, window->x + 10, window->y + 10 - 32, f, from_argb(255, 255, 255, 255));

    Rectangle r;
    r.size.width = window->w;
    r.size.height = window->h;
    r.pos.x = window->x;
    r.pos.y = window->y;

    window->draw(r);
}

void window_update(Window *window)
{
    int mouse_x = mouse_getx();
    int mouse_y = mouse_gety();
    if (getMouseStateByte() & 0x01)
    {
        if (mouse_x >= window->x && mouse_x <= window->x + window->w &&
            mouse_y >= window->y - 16 && mouse_y <= window->y)
        {
            if (window->moving == 0)
            {
                window->moving = 1;
                window->offX = mouse_x - window->x;
                window->offY = mouse_y - window->y;
            }
        }
    }
    else
    {
        if (window->moving == 1)
            window->moving = 0;
    }

    if (window->moving == 1)
    {
        int new_x = mouse_x - window->offX;
        int new_y = mouse_y - window->offY;

        if (new_x >= 0 && new_x + window->w <= mb_info->framebuffer_width &&
            new_y >= 32 && new_y + window->h <= mb_info->framebuffer_height)
        {
            window->x = new_x;
            window->y = new_y;
        }
    }
}