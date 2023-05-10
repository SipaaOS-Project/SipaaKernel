#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

typedef struct task
{
  uint32_t *stack;
  void (*entry)(void);
  bool running;
  struct task *next;
} task_t;

extern task_t *tasks;

void add_task(task_t *task);
void configure_task(task_t *task, size_t memory, void (*entry)(void));
void close_task(task_t *task);
void execute_task(task_t *task);