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
    Image dvd;
    dvd.width = DVDVIDEOLOLOL_WIDTH;
    dvd.height = DVDVIDEOLOLOL_HEIGHT;
    dvd.pixels = (uint32_t *)dvdvideololol;

    while (1)
    {
        clear_screen(black);
        set_pixel(mouse_getx(), mouse_gety(), white);
        set_image(dvd, 0, 0);
        flush();
    }
}
