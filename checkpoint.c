/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "checkpoint.h"

#define UCHEAP_SIZE configTOTAL_HEAP_SIZE

extern uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];

#pragma PERSISTENT(backup_ucHeap)
uint8_t backup_ucHeap[2][UCHEAP_SIZE] = {0};

#pragma PERSISTENT(backup_idx)
uint8_t backup_idx = 2;

uint32_t cur_register[16] = {0};

#pragma PERSISTENT(backup_register)
uint32_t backup_register[2][16] = {0};

#include "inc/hw_regaccess.h"
#include "inc/hw_memmap.h"

void power_off() {
    asm volatile (
        "nop\n"
        "dint\n"
    );
    HWREG8(PMM_BASE + OFS_PMMCTL0_H) = PMMPW_H;
    HWREG8(PMM_BASE + OFS_PMMCTL0_L) |= PMMREGOFF;
    HWREG8(PMM_BASE + OFS_PMMCTL0_L) &= ~SVSHE;
    HWREG8(PMM_BASE + OFS_PMMCTL0_H) = 0x00;
    LPM4;
}

void commit_c() {
    int cur_w_idx = backup_idx == 2 ? 0 : backup_idx ^ 1;
    int i;

    for (i = 0; i < 16; i++)
        backup_register[cur_w_idx][i] = cur_register[i];
    for (i = 0; i < UCHEAP_SIZE; i++)
        backup_ucHeap[cur_w_idx][i] = ucHeap[i];

    backup_idx = cur_w_idx;
    commit_leave();
}

void restore_c() {
    int i;

    if (backup_idx == 2)
        return;

    for (i = 0; i < 16; i++)
        cur_register[i] = backup_register[backup_idx][i];
    for (i = 0; i < UCHEAP_SIZE; i++)
        ucHeap[i] = backup_ucHeap[backup_idx][i];

    restore_leave();
}
