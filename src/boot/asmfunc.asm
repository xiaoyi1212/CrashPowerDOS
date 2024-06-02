global paging_load_directory

paging_load_directory: ;void paging_load_directory(uint32_t *directory);
    push ebp
    mov ebp, esp
    mov eax, [ebp+8]
    mov cr3, eax
    pop ebp
    ret