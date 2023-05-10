#include <stdint.h>

#include <libasg/libasg.h>
typedef struct
{
    char *text;
    int x;
    int y;
    int width;
    int height;
    int radius;
    void (*clicked)(void);
    Color background;
    Color foreground;
    Font font;
} siui_button_t;

void render_button(siui_button_t button);
void update_button(siui_button_t button);