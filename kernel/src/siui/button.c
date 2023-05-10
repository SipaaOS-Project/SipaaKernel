#include "button.h"
#include <stdlib/stdio.h>
#include <mouse/mouse.h>

void render_button(siui_button_t button)
{
    int strwidth = strlen(button.text) * button.font.charwidth;
    int x = button.width / 2 - strwidth / 2;
    int y = button.height / 2 - button.font.charheight / 2;

    if (button.radius > 0)
    {
        set_round_rect(button.x, button.y, button.width, button.height, button.radius, button.background);   
    }
    else
    {
        set_rect(button.x, button.y, button.width, button.height, button.background);
    }
    set_string(button.text, button.x + x, button.y + y, button.font, button.foreground);
}

void update_button(siui_button_t button)
{
    int x = mouse_getx();
    int y = mouse_gety();
    if (getMouseStateByte() & 0x01 && x > button.x && x < button.x + button.width && y > button.y && y < button.y + button.height)
    {
        button.clicked();
    }
}