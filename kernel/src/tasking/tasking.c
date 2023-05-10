#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include "tasking.h"

task_t* tasks = NULL;

void add_task(task_t *task)
{
    task->next = tasks;
    tasks = task;
}

void remove_task(task_t *task)
{
    task_t *prev = NULL;
    task_t *curr = tasks;
    while (curr != NULL)
    {
        if (curr == task)
        {
            if (prev == NULL)
            {
                tasks = curr->next;
            }
            else
            {
                prev->next = curr->next;
            }
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void close_task(task_t *task)
{
    remove_task(task);
    free(task->stack);
}

void configure_task(task_t *task, size_t memory, void (*entry)(void))
{
    task->stack = malloc(memory);
    task->entry = entry;
    task->running = false;

    add_task(task);
}

void execute_task(task_t *task)
{

    if (!task->running)
    {
        task->running = true;
        task->entry();
        task->running = false;
    }
}