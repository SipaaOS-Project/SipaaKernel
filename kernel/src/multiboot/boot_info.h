#include <stddef.h>
#include <stdint.h>
#include "multiboot.h"

extern multiboot_info_t* mb_info;

void set_bootinfo(multiboot_info_t *mbi);