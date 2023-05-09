
#include <stddef.h>
#include <stdint.h>
#include "boot_info.h"

multiboot_info_t* mb_info;

void set_bootinfo(multiboot_info_t *mbi)
{
    mb_info = mbi;
}