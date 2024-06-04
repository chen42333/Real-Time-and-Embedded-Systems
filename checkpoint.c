/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "checkpoint.h"

#define UCHEAP_SIZE configTOTAL_HEAP_SIZE

#pragma PERSISTENT(backup_ucHeap)
uint8_t backup_ucHeap[2][UCHEAP_SIZE] = {0};

#pragma PERSISTENT(backup_idx)
int8_t backup_idx = 2;

#pragma PERSISTENT(backup_register)
uint32_t backup_register[2][16] = {0};

uint32_t cur_register[16] = {0};

void power_off() {
}

void commit_c() {
    if (backup_idx == 2)
        backup_idx = 0;
    commit_leave();
}

void restore_c() {
    if (backup_idx == 2)
        return;
    restore_leave();
}
