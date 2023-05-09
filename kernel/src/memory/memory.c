#include "memory.h"

static uint8_t *mem_start_addr;
static uint32_t mem_size;
static struct mem_block *free_list;

void init_mm(multiboot_info_t *mb_info)
{
    uint32_t mem_upper = mb_info->mem_upper * 1024;
    uint32_t mem_lower = mb_info->mem_lower * 1024;

    mem_size = mem_upper;

    if (mem_upper > 0)
    {
        mem_start_addr = (uint8_t *)0x100000; // Start of memory is at 1MB
        uint32_t num_pages = mem_upper / PAGE_SIZE;

        free_list = (struct mem_block *)mem_start_addr;
        free_list->next = NULL;
        free_list->size = num_pages * PAGE_SIZE - sizeof(struct mem_block);
        free_list->used = false;
    }
}

static void *alloc_mem_block(struct mem_block *block, size_t size)
{
    size_t leftover_size = block->size - size;

    if (leftover_size < sizeof(struct mem_block))
    {
        block->used = true;
        return (void *)(block + 1);
    }

    struct mem_block *leftover_block = (struct mem_block *)((uint8_t *)block + sizeof(struct mem_block) + size);
    leftover_block->next = block->next;
    leftover_block->size = leftover_size - sizeof(struct mem_block);
    leftover_block->used = false;

    block->next = leftover_block;
    block->size = size;
    block->used = true;

    return (void *)(block + 1);
}

void *malloc(size_t size)
{
    size_t alloc_size = size + sizeof(struct mem_block);
    struct mem_block *prev_block = NULL;
    struct mem_block *curr_block = free_list;

    while (curr_block)
    {
        if (!curr_block->used && curr_block->size >= alloc_size)
        {
            return alloc_mem_block(curr_block, size);
        }

        prev_block = curr_block;
        curr_block = curr_block->next;
    }

    return NULL;
}

void free(void *ptr)
{
    if (!ptr)
    {
        return;
    }

    struct mem_block *block = (struct mem_block *)ptr - 1;
    block->used = false;

    if (block->next && !block->next->used)
    {
        block->size += sizeof(struct mem_block) + block->next->size;
        block->next = block->next->next;
    }

    if (block->prev && !block->prev->used)
    {
        block->prev->size += sizeof(struct mem_block) + block->size;
        block->prev->next = block->next;
        if (block->next)
        {
            block->next->prev = block->prev;
        }
        block = block->prev;
    }

    if (!block->next && block->prev)
    {
        block->prev->next = NULL;
    }
}