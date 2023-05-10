#include <multiboot/multiboot.h>
#include <stdlib/stdio.h>
#include <arch/gdt.h>
#include <arch/idt.h>
#include <arch/tss.h>
#include <memory/memory.h>
#include <mouse/mouse.h>
#include <res/res.h>
#include <arch/ioport.h>
#include <siui/button.h>
#include <tasking/tasking.h>

siui_button_t btn;
Color white;
Color black;
Color transparentwhite;
Font f;

void clear_task()
{
   clear_screen(black);
}
void mouse_task()
{
   set_pixel(mouse_getx(), mouse_gety(), white);
}
void button_task()
{
   render_button(btn);
   update_button(btn);
}
void clicked()
{
   set_string("Clicked!", 300, 300, f, white);
}

void _kstart(multiboot_info_t *mboot_info)
{
   init_gdt();
   init_idt();
   init_tss();
   init_mm(mboot_info);
   init_stdio();
   init_mouse((int)mboot_info->framebuffer_width, (int)mboot_info->framebuffer_height);
   init_libasg(mboot_info);

   white = from_argb(255, 255, 255, 255);
   black = from_argb(255, 0, 0, 0);
   transparentwhite = from_argb(147, 255, 255, 255);

   f.charwidth = 8;
   f.charheight = 16;
   f.pixels = (uint8_t *)deffont;

   Image* img;
   img->width = DVDVIDEOLOLOL_WIDTH;
   img->height = DVDVIDEOLOLOL_HEIGHT;
   img->pixels = (uint32_t *)dvdvideololol;
   
   btn.text = "Test";
   btn.background = white;
   btn.foreground = black;
   btn.radius = 0;
   btn.x = 100;
   btn.y = 100;
   btn.clicked = clicked;
   btn.width = 150;
   btn.height = 40;
   btn.font = f;
   
   task_t buttonTask, clearTask, mouseTask;
   configure_task(&buttonTask, 1024, button_task);
   configure_task(&mouseTask, 1024, mouse_task);
   configure_task(&clearTask, 1024, clear_task);


   while (1)
   {
      execute_task(&clearTask);
      //execute_task(&buttonTask);
      set_image(img, 100, 100);
      //set_rect(100, 100, DVDVIDEOLOLOL_WIDTH, DVDVIDEOLOLOL_HEIGHT, transparentwhite);
      blur(100, 100, DVDVIDEOLOLOL_WIDTH, DVDVIDEOLOLOL_HEIGHT, 30);
      execute_task(&mouseTask);
      flush();
   }
}

void blur(int x, int y, int width, int height, int intensity)
{
        for (int w = x; w < x + width; w++)
        {
            for (int h = y; h < x + height; h++)
            {
                int r = 0, g = 0, b = 0, a = 0;
                int counter = 0;

                for (int ww = w - intensity; ww < w + intensity; ww++)
                {
                    for (int hh = h - intensity; hh < h + intensity; hh++)
                    {
                        if (ww >= 0 && hh >= 0 && ww < mb_info->framebuffer_width && hh < mb_info->framebuffer_height)
                        {
                            Color color = get_pixel(ww,hh);

                            r += color.r;
                            g += color.g;
                            b += color.b;
                            a += color.a;

                            counter++;
                        }
                    }
                }

                r /= counter;
                g /= counter;
                b /= counter;
                a /= counter;

                set_pixel(w, h, from_argb(a, r, g, b));
                //SetPixel(w, h, Color.FromArgb((int)a, (int)r, (int)g, (int)b).ToArgb());
            }
        }
    }