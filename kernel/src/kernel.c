#include <multiboot/multiboot.h>
#include <stdlib/stdio.h>
#include <arch/gdt.h>
#include <arch/idt.h>
#include <memory/memory.h>
#include <libasg/libasg.h>
#include <mouse/mouse.h>
#include <res/res.h>

void _kstart(multiboot_info_t *mboot_info)
{
    init_gdt();
    init_idt();
    init_mm(mboot_info);
    init_stdio();
    init_mouse((int)mboot_info->framebuffer_width, (int)mboot_info->framebuffer_height);
    init_libasg(mboot_info);
    
    Color white = { 255, 255, 255, 255 };
    Color black = { 255, 0, 0, 0 };
    Color transparentwhite = { 147, 255, 255, 255 };

    Image* img;
    img->width = DVDVIDEOLOLOL_WIDTH;
    img->height = DVDVIDEOLOLOL_HEIGHT;
    img->pixels = (uint32_t *)dvdvideololol;

    while (1)
    {
        clear_screen(black);
        set_image(img, 60, 60);
        set_rect(60, 60, DVDVIDEOLOLOL_WIDTH, DVDVIDEOLOLOL_HEIGHT, transparentwhite);
        set_pixel(mouse_getx(), mouse_gety(), white);
        flush();
    }
}
