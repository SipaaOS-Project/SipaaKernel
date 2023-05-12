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
#include <siui/window.h>
#include <tasking/tasking.h>
#include <multiboot/boot_info.h>
#include <debug/debug.h>

Window win;
siui_button_t btn;
Image cursorImage;
Color white;
Color black;
Color transparentwhite;
Font f;
task_t buttonTask, clearTask, mouseTask;

void clear_task()
{
   clear_screen(black);
}
void mouse_task()
{
   set_alphaImage(&cursorImage, mouse_getx(), mouse_gety(), from_argb(255, 1, 1, 1));
}
void button_task()
{
   window_draw(&win);
   window_update(&win);
}
void okbtn_clicked()
{
   buttonTask.disabled = true; // this should make the multitasking not run the window render because it execute the code only if the task isn't running.
}
void windraw(Rectangle winbounds)
{
   set_string("Did you know that SipaaKernel wallpapers\ncame from Venezuela?", winbounds.pos.x + 20, winbounds.pos.y + 20, f, from_argb(255, 255, 255, 255));
   btn.x = winbounds.pos.x + winbounds.size.width - btn.width - 20;
   btn.y = winbounds.pos.y + (winbounds.size.height - 32) - btn.height - 20;
   
   render_button(btn);
   update_button(btn);
}

void _kstart(multiboot_info_t *mboot_info)
{
   writedbg("[Core] Setting up Multiboot infos... ");
   set_bootinfo(mboot_info);
   writedbg("OK\n");
   writedbg("[Core] Initializing Global Descriptor Table... ");
   init_gdt();
   writedbg("OK\n");
   writedbg("[Core] Initializing Interrupt Descriptor Table... ");
   init_idt();
   writedbg("OK\n");
   writedbg("[Core] Initializing Task State Segment... ");
   init_tss();
   writedbg("OK\n");
   writedbg("[Core] Initializing Memory Manager... ");
   init_mm(mboot_info);
   writedbg("OK\n");
   writedbg("[Core] Initializing Standard I/O Library... ");
   init_stdio();
   writedbg("OK\n");
   writedbg("[Core] Initializing Mouse... ");
   init_mouse((int)mboot_info->framebuffer_width, (int)mboot_info->framebuffer_height);
   writedbg("OK\n");
   init_libasg(mboot_info);

   white = from_argb(255, 255, 255, 255);
   black = from_argb(255, 0, 0, 0);
   transparentwhite = from_argb(147, 255, 255, 255);

   f.charwidth = 8;
   f.charheight = 16;
   f.pixels = (uint8_t *)deffont;

   cursorImage.width = CURSOR_WIDTH;
   cursorImage.height = CURSOR_HEIGHT;
   cursorImage.pixels = (uint32_t *)cursor;
   
   btn.width = 150;
   btn.height = 40;
   btn.radius = 6;
   btn.text = "OK";
   btn.background = from_argb(255, 0, 0, 0);
   btn.foreground = from_argb(255, 255, 255, 255);
   btn.clicked = okbtn_clicked;
   btn.font = f;
   
   init_window(&win, "Fact of the day", mb_info->framebuffer_width / 2 - 400 / 2, mb_info->framebuffer_height / 2 - 200 / 2, 400, 200);
   win.draw = windraw;
   configure_task(&buttonTask, 1024, button_task);
   configure_task(&mouseTask, 1024, mouse_task);
   configure_task(&clearTask, 1024, clear_task);


   while (1)
   {
      execute_task(&clearTask);
      set_string(get_debuglogs(), 10, 10, f, white);
      execute_task(&buttonTask);
      //ApplyAA();
      execute_task(&mouseTask);
      flush();
   }
}