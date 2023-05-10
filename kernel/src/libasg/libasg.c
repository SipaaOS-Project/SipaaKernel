#include <libasg/libasg.h>

multiboot_info_t *mbinfo;

// private methods
void *memcpy(char *dst, char *src, int n)
{
    char *p = dst;
    while (n--)
        *dst++ = *src++;
    return p;
}

double pow(double base, double exponent)
{
    if (exponent == 0)
    {
        return 1;
    }

    double result = base;
    int i;

    for (i = 1; i < exponent; i++)
    {
        result *= base;
    }

    return result;
}

// public methods
Color from_argb(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
    Color c;
    c.a = a;
    c.r = r;
    c.g = g;
    c.b = b;

    return c;
}
Color alpha_blend(Color src, Color new)
{
    if (new.a == 255)
    {
        return new;
    }
    if (new.a == 0)
    {
        return src;
    }

    Color c;
    //c.a = ((src.a * (255 - new.a) / 255) + new.a);
    //c.r = ((src.r * (255 - new.a) / 255) + new.r);
    //c.g = ((src.g * (255 - new.a) / 255) + new.g);
    //c.b = ((src.b * (255 - new.a) / 255) + new.b);
    c.a = (((new.a * new.a) + (src.a * (255 - new.a))) >> 8);
    c.r = (((new.r * new.a) + (src.r * (255 - new.a))) >> 8);
    c.g = (((new.g * new.a) + (src.g * (255 - new.a))) >> 8);
    c.b = (((new.b * new.a) + (src.b * (255 - new.a))) >> 8);
    return c;
}

void init_libasg(multiboot_info_t *info)
{
    mbinfo = info;
}

void clear_screen(Color color)
{
    for (int y = 0; y < mbinfo->framebuffer_height; y++)
    {
        for (int x = 0; x < mbinfo->framebuffer_width; x++)
        {
            set_pixel(x, y, color);
        }
    }
}

void set_pixel(int x, int y, Color color)
{
    Color c = color;

    if (c.a < 255 && c.a > 0)
    {
        c = alpha_blend(get_pixel(x, y), color);
    }

    unsigned *backbuffer = (unsigned *)(mbinfo->framebuffer_addr + (y + mbinfo->framebuffer_height) * mbinfo->framebuffer_pitch);
    backbuffer[x] = (c.a << 24) | (c.r << 16) | (c.g << 8) | c.b;
}
Color get_pixel(int x, int y)
{
    unsigned *backbuffer = (unsigned *)(mbinfo->framebuffer_addr + (y + mbinfo->framebuffer_height) * mbinfo->framebuffer_pitch);
    unsigned color = backbuffer[x];
    return (Color){(color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF};
}

void set_rect(int x, int y, int width, int height, Color color)
{
    for (int i = y; i < y + height; i++)
    {
        for (int _i = x; _i < x + width; _i++)
        {
            set_pixel(_i, i, color);
        }
    }
}

void set_image(Image *image, int x, int y)
{
    for (int j = 0; j < image->height; j++)
    {
        for (int i = 0; i < image->width; i++)
        {
            Color color;
            uint32_t pixel = image->pixels[j * image->width + i];
            color.a = (pixel >> 24) & 0xFF;
            color.r = (pixel >> 16) & 0xFF;
            color.g = (pixel >> 8) & 0xFF;
            color.b = pixel & 0xFF;
            
            set_pixel(x + i, y + j, color);
        }
    }
}

Image create_image(uint32_t *data, uint32_t width, uint32_t height)
{
    Image img;
    img.pixels = (uint32_t *)malloc(width * height * sizeof(uint32_t));
    memcpy(img.pixels, data, width * height * sizeof(uint32_t));
    img.width = width;
    img.height = height;
    return img;
}

void set_char(char c, int x, int y, Font f, Color col)
{
    uint8_t *offset = f.pixels + sizeof(FontHeader) + 16 * c;

    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (offset[i] & (1 << j))
            {
                set_pixel(x + 8 - j, y + i, col);
            }
        }
    }
}

Position set_string(char *string, int x, int y, Font f, Color col)
{
    int cursorx = x;
    int cursory = y;

    for (int i = 0; string[i] != '\0'; i++)
    {
        if (string[i] == '\n')
        {
            cursorx = x;
            cursory += f.charheight;
        }
        else
        {
            set_char(string[i], cursorx, cursory, f, col);
            cursorx += f.charwidth;
        }
    }

    Position p;
    p.x = cursorx;
    p.y = cursory;

    return p;
}

void set_round_rect(int x, int y, int width, int height, int radius, Color color)
{
    /**
    for (int j = y; j < (y + height); j++)
    {
        for (int i = x; i < (x + width); i++)
        {
            int dx = i - x;
            int dy = j - y;
            if ((dx < radius && dy < radius && pow(dx - radius, 2) + pow(dy - radius, 2) > pow(radius, 2)) ||
                (dx < radius && dy > height - radius && pow(dx - radius, 2) + pow(dy - (height - radius), 2) > pow(radius, 2)) ||
                (dx > width - radius && dy < radius && pow(dx - (width - radius), 2) + pow(dy - radius, 2) > pow(radius, 2)) ||
                (dx > width - radius && dy > height - radius && pow(dx - (width - radius), 2) + pow(dy - (height - radius), 2) > pow(radius, 2)))
            {
            }
            else
            {
                set_pixel(i, j, color);
            }
        }
    }**/
    
    int x_end = x + width - 1;
    int y_end = y + height - 1;
    int r = radius;

    set_rect(x + r, y, width - 2 * r, height, color);
    set_rect(x, y + r, r, height - 2 * r, color);
    set_rect(x + width - r, y + r, r, height - 2 * r, color);

    for (int j = -r; j <= r; j++)
    {
        for (int i = -r; i <= r; i++)
        {
            int distance_sq = i * i + j * j;
            if (distance_sq <= r * r)
            {
                int pixel_x, pixel_y;

                // Top-left corner
                pixel_x = x + r + i;
                pixel_y = y + r + j;
                set_pixel(pixel_x, pixel_y, color);

                // Top-right corner
                pixel_x = x_end - r + i;
                pixel_y = y + r + j;
                set_pixel(pixel_x, pixel_y, color);

                // Bottom-left corner
                pixel_x = x + r + i;
                pixel_y = y_end - r + j;
                set_pixel(pixel_x, pixel_y, color);

                // Bottom-right corner
                pixel_x = x_end - r + i;
                pixel_y = y_end - r + j;
                set_pixel(pixel_x, pixel_y, color);
            }
        }
    }
}

void set_circle(int x0, int y0, int radius, Color color)
{
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (x <= y)
    {
        for (int i = x0 - y; i <= x0 + y; i++)
        {
            set_pixel(i, y0 + x, color);
            set_pixel(i, y0 - x, color);
        }
        for (int i = x0 - x; i <= x0 + x; i++)
        {
            set_pixel(i, y0 + y, color);
            set_pixel(i, y0 - y, color);
        }
        x++;
        if (d < 0)
        {
            d += 4 * x + 6;
        }
        else
        {
            d += 4 * (x - y) + 10;
            y--;
        }
    }
}

void flush()
{
    memcpy(mbinfo->framebuffer_addr, mbinfo->framebuffer_addr + mbinfo->framebuffer_height * mbinfo->framebuffer_pitch, mbinfo->framebuffer_pitch * mbinfo->framebuffer_height);
}