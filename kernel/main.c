#include "../include/vga.h"
#include "../include/io.h"
#include "../include/description_table.h"
#include "../include/multiboot.h"
#include "../include/memory.h"
#include "../include/shell.h"
#include "../include/keyboard.h"
#include "../include/timer.h"
#include "../include/task.h"
#include "../include/cmos.h"
#include "../include/acpi.h"

extern uint32_t end;
uint32_t placement_address = (uint32_t) &end;

static uint32_t get_max_pm_addr(multiboot_t *mboot_ptr){          //qemu默认为128M
    uint32_t max_addr=0;
    for(pm_entry_t * pm_entry_cur = mboot_ptr->mmap_addr;pm_entry_cur<mboot_ptr->mmap_addr+mboot_ptr->mmap_length;pm_entry_cur++){
        printf("0x%h-0x%h-0x%h-%d\n",pm_entry_cur->base_addr_low,pm_entry_cur->length_low,pm_entry_cur->base_addr_low+pm_entry_cur->length_low,pm_entry_cur->type);
        if(pm_entry_cur->type==1&&max_addr<pm_entry_cur->base_addr_low+pm_entry_cur->length_low)
            max_addr=pm_entry_cur->base_addr_low+pm_entry_cur->length_low;
    }
    return max_addr;
}

void kernel_main(void *multiboot_structure, unsigned int magicnumber) {

    vga_install();
    vga_writestring("[Driver]: VGA Load success!\n");
    install_gdt();
    install_idt();

    vga_writestring("[Kernel]: Description table load success!\n");
    setup_error();

    multiboot_t *multiboot = (multiboot_t*)multiboot_structure;
    printf("Max Memory: %d.\n", get_max_pm_addr(multiboot));

    init_page();
    vga_writestring("[Kernel]: Memory page init success!\n");
    init_keyboard();
    vga_writestring("[Driver]: Keyboard Load success!\n");
    //acpi_install();
    vga_writestring("[Kernel]: ACPItable load success! \n");

    init_timer(50);
    init_task();
    print_cpu_id();

    task_t* shell = create_task("CPOS-Shell",setup_shell);
    //task_run(shell);

    setup_shell();

    for (;;)io_hlt();
}