	.include data_model.h

    .global cur_register
    .global commit_c
    .global restore_c

    .def commit
    .def commit_leave
    .def restore
    .def restore_leave

    .text
    .align 2

commit: .asmfunc
    nop
    dint
    nop

    mov_x  r1,  cur_register + 4 * 1
    mov_x  r2,  cur_register + 4 * 2
    mov_x  r3,  cur_register + 4 * 3
    mov_x  r4,  cur_register + 4 * 4
    mov_x  r5,  cur_register + 4 * 5
    mov_x  r6,  cur_register + 4 * 6
    mov_x  r7,  cur_register + 4 * 7
    mov_x  r8,  cur_register + 4 * 8
    mov_x  r9,  cur_register + 4 * 9
    mov_x  r10, cur_register + 4 * 10
    mov_x  r11, cur_register + 4 * 11
    mov_x  r12, cur_register + 4 * 12
    mov_x  r13, cur_register + 4 * 13
    mov_x  r14, cur_register + 4 * 14
    mov_x  r15, cur_register + 4 * 15

    jmp   commit_c

    .endasmfunc

commit_leave: .asmfunc
    nop
    eint
    nop
    ret_x
    .endasmfunc

restore: .asmfunc
    nop
    dint
    nop

    jmp   restore_c

    .endasmfunc

restore_leave: .asmfunc

    mov_x  cur_register + 4 * 1 , r1
    nop
    mov_x  cur_register + 4 * 2 , r2
    nop
    mov_x  cur_register + 4 * 3 , r3
    mov_x  cur_register + 4 * 4 , r4
    mov_x  cur_register + 4 * 5 , r5
    mov_x  cur_register + 4 * 6 , r6
    mov_x  cur_register + 4 * 7 , r7
    mov_x  cur_register + 4 * 8 , r8
    mov_x  cur_register + 4 * 9 , r9
    mov_x  cur_register + 4 * 10, r10
    mov_x  cur_register + 4 * 11, r11
    mov_x  cur_register + 4 * 12, r12
    mov_x  cur_register + 4 * 13, r13
    mov_x  cur_register + 4 * 14, r14
    mov_x  cur_register + 4 * 15, r15

    nop
    eint
    nop

    ret_x
    .endasmfunc
