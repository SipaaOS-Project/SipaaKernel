#include <multiboot/multiboot.h>
#include <stdint.h>

typedef struct
{
    uint8_t a;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

typedef struct
{
    uint32_t *pixels;
    uint32_t width;
    uint32_t height;
} Image;

typedef struct
{
    uint8_t *pixels;
    int charwidth;
    int charheight;
} Font;

typedef struct
{
    int x;
    int y;
} Position;

typedef struct
{
    int width;
    int height;
} Size;

typedef struct
{
    Size size;
    Position pos;
} Rectangle;


typedef struct
{
    uint8_t magic[2];
    uint8_t mode;
    uint8_t height;
} FontHeader;

Color from_argb(uint8_t a, uint8_t r, uint8_t g, uint8_t b);
void init_libasg(multiboot_info_t *info);
void clear_screen(Color color);
void set_pixel(int x, int y, Color color);
Color get_pixel(int x, int y);
void set_rect(int x, int y, int width, int height, Color color);
void set_image(Image *image, int x, int y);
void set_alphaImage(Image *image, int x, int y, Color alphaColor);
Image create_image(uint32_t *data, uint32_t width, uint32_t height);
void set_round_rect(int x, int y, int width, int height, int radius, Color color);
void set_circle(int x0, int y0, int radius, Color color);
void set_char(char c, int x, int y, Font f, Color col);
Position set_string(char *string, int x, int y, Font f, Color col);
void flush();