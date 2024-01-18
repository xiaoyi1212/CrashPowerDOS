#ifndef CPOS_TASK_H
#define CPOS_TASK_H

#include <stdint.h>
#include "memory.h"

#define EFLAGES_DEFAULT ( 1 << 1 )
#define EFLAGES_IF (1 << 9)
#define MAX_TASKS 1000
#define TASK_GDT0 3

#define MAX_FILES_OPEN_PER_PROC 8

/*
        通用寄存器(EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI)
        段寄存器(ES，CS，SS，DS，FS，GS)
        状态寄存器(EFLAGS)
        指令指针(EIP)
        前一个执行的任务的TSS段的选择子(只有当要返回时才更新)
*/

struct tss_entry {
    int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
    int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    int es, cs, ss, ds, fs, gs;
    int ldtr, iomap;
} __attribute__ ((packed));

typedef struct {
    uint32_t eax, ebx, ecx, edx, esi, edi, ebp, esp, eip, eflags;
} RegisterState;

typedef enum {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessStatus;

typedef struct task
{
    struct tss_entry tss;
    uint32_t pid;
    int sel, flags;
    int fd_table[MAX_FILES_OPEN_PER_PROC];
    char* task_name;
    ProcessStatus status;
}task_t;

typedef struct TASKCTL {
    int running, now;
    task_t *tasks[MAX_TASKS];
    task_t tasks0[MAX_TASKS];
} taskctl_t;

void tss_install();
void task_switch(registers_t *reg);
void task_run(task_t *task);
task_t *task_alloc();
task_t *task_init();
void task_exit(task_t *task);
task_t *task_now();
void init_task();

#endif