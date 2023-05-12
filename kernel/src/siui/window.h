#include <stdint.h>

typedef struct
{
    char *name;
    int x;
    int y;
    int w;
    int h;
    int offX;
    int offY;
    int moving;
    void (*draw)(Rectangle);
    void (*update)(Rectangle);
} Window;

void init_window(Window *window, char *name, int x, int y, int w, int h);
void window_draw(Window *window);
void window_update(Window *window);
