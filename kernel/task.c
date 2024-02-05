#include "../include/task.h"
#include "../include/memory.h"
#include "../include/io.h"
#include "../include/vga.h"
#include "../include/description_table.h"

taskctl_t *taskctl;

extern void load_tr(int);

extern void farjmp(int, int);

task_t *curror_task;
task_t *kernel_task;
struct tss_entry tss_e;
uint32_t pid_index, task_index;

task_t *task_init() //0号系统进程初始化
{
    int i;
    task_t *task;
    taskctl = (taskctl_t *) kmalloc(sizeof(taskctl_t));

    /*
    for (i = 0; i < MAX_TASKS; i++) {
        taskctl->tasks0[i].flags = 0;
        taskctl->tasks0[i].sel = (TASK_GDT0 + i) * 8;
        taskctl->tasks0[i].tss.ldtr = (TASK_GDT0 + MAX_TASKS + i) * 8;
    }
     */

    task = task_alloc();
    task->flags = 2;
    task->pid = pid_index++;
    task->task_name = "CPOS-System";
    task->status = RUNNING;

    taskctl->running = 1;
    taskctl->now = 0;
    taskctl->tasks[0] = task;

    load_tr(task->sel);
    return task;
}

void task_switch(registers_t *reg) {

    if (taskctl->running >= 2) {
        taskctl->now++;
        if (taskctl->now == taskctl->running) taskctl->now = 0;
        farjmp(0, taskctl->tasks[taskctl->now]->sel);
    }
}

task_t *task_alloc() {
    int i;
    task_t *task;
    for (i = 0; i < MAX_TASKS; i++) {
        if (taskctl->tasks0[i].flags == 0) {
            task = &taskctl->tasks0[i];
            task->flags = 1;
            task->tss.eflags = 0x1202;
            task->tss.eax = task->tss.ecx = task->tss.edx = task->tss.ebx = 0;
            task->tss.ebp = task->tss.esi = task->tss.edi = 0;
            task->tss.es = task->tss.ds = task->tss.fs = task->tss.gs = 0;
            task->tss.ldtr = 0;
            task->tss.iomap = 0x40000000;

            task->fd_table[0] = 0;
            task->fd_table[1] = 1;
            task->fd_table[2] = 2;

            uint8_t fd_idx = 3;
            while (fd_idx < MAX_FILES_OPEN_PER_PROC) {
                task->fd_table[fd_idx] = -1;
                fd_idx++;
            }
            return task;
        }
    }
    return 0;
}

task_t *create_task(char* name,void* func){
    task_t *proc = task_alloc();
    proc->task_name = name;
    proc->pid = pid_index++;

    proc->tss.esp = (uint32_t) kmalloc(64 * 1024) + 64 * 1024 - 4;
    proc->tss.eip = (int) &func;
    proc->tss.es = proc->tss.ss = proc->tss.ds = proc->tss.fs = proc->tss.gs = 2 * 8;
    proc->tss.cs = 1 * 8;

    return proc;
}

void task_run(task_t *task) {
    task->flags = 2;
    taskctl->tasks[taskctl->running] = task;
    taskctl->running++;
}

task_t *task_now() {
    return taskctl->tasks[taskctl->now];
}

void task_exit(task_t *task) {
    int i;
    char do_switch = 0;
    if (task->flags == 2) {
        // 在运行中/可运行
        if (task == task_now()) {
            // 自己要退出？
            do_switch = 1; // 要进行切换，标记一下
        }
        // 找到task位置
        for (i = 0; i < taskctl->running; i++) {
            if (taskctl->tasks[i] == task) {
                // 找到了
                break;
            }
        }
        // 给running减1
        taskctl->running--;
        // 在当前任务前面
        if (i < taskctl->now) {
            taskctl->now--; // 当前任务也前移
        }
        for (; i < taskctl->running; i++) {
            taskctl->tasks[i] = taskctl->tasks[i + 1]; // 把所有的任务前移一个
        }
        task->flags = 1; // 这个任务不工作了
        if (do_switch) {
            // 还需要切换
            if (taskctl->now >= taskctl->running) taskctl->now = 0; // 修正错误的taskctl->now
            farjmp(0, taskctl->tasks[taskctl->now]->sel); // 直接进行任务切换
        }
    }
}

taskctl_t *get_manager(){
    return taskctl;
}

void init_task() {
    io_cli();
    kernel_task = task_init();
    curror_task = kernel_task;
    io_sti();
}