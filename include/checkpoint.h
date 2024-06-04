#ifndef INC_CHECKPOINT_H
#define INC_CHECKPOINT_H

#ifndef INC_FREERTOS_H
    #error "include FreeRTOS.h must appear in source files before include checkpoint.h"
#endif

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */

extern void commit();
extern void commit_leave();
extern void restore();
extern void restore_leave();

void power_off();
void commit_c();
void restore_c();

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */

#endif INC_CHECKPOINT_H
