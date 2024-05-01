/*************************************************************************
* Copyright (c) 2004 Altera Corporation, San Jose, California, USA.      *
* All rights reserved. All use of this software and documentation is     *
* subject to the License Agreement located at the end of this file below.*
**************************************************************************
* Description:                                                           *
* The following is a simple hello world program running MicroC/OS-II.The * 
* purpose of the design is to be a very simple application that just     *
* demonstrates MicroC/OS-II running on NIOS II.The design doesn't account*
* for issues such as checking system call return codes. etc.             *
*                                                                        *
* Requirements:                                                          *
*   -Supported Example Hardware Platforms                                *
*     Standard                                                           *
*     Full Featured                                                      *
*     Low Cost                                                           *
*   -Supported Development Boards                                        *
*     Nios II Development Board, Stratix II Edition                      *
*     Nios Development Board, Stratix Professional Edition               *
*     Nios Development Board, Stratix Edition                            *
*     Nios Development Board, Cyclone Edition                            *
*   -System Library Settings                                             *
*     RTOS Type - MicroC/OS-II                                           *
*     Periodic System Timer                                              *
*   -Know Issues                                                         *
*     If this design is run on the ISS, terminal output will take several*
*     minutes per iteration.                                             *
**************************************************************************/


#include <stdio.h>
#include "includes.h"

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048

#define SCENARIO 2
#if SCENARIO == 1
#define N_TASKS 3
#elif SCENARIO == 2
#define N_TASKS 2
#endif
#define N_RESOURCES 2
OS_EVENT*        Mutex[N_RESOURCES];

OS_STK        TaskStk[N_TASKS+1][TASK_STACKSIZE];        /* Tasks stacks                                  */
INT8U         TaskData[N_TASKS];                      /* Parameters to pass to each task               */

/* Definition of Task Priorities */

void print() {
  long time; int event; INT8U from, to, resource;
  while (pop_info(&time, &event, &from, &to, &resource)) {
    if (event == EXCEED) {
      printf("time:%lu task%hhu exceed deadline\n", time, from);
      OSSchedLock();
      for (;;);
    }
    else if (event == LOCK || event == UNLOCK) printf("%lu\t%s\tR%hhu\t(Prio=%hhu changes to=%hhu)\n",
        time, event == LOCK ? "Lock" : "Unlock", resource, from, to);
    else {
      printf("%lu\t%s\t%2hhu\t%2hhu\n",
              time, event == COMPLETE ? "Complete" : "Preempt", from, to);
    }
  }
}

void S1_H (void* pdata) {
  OSTCBCur->compTime = 6;
  INT8U err;
  OSTimeDly(8); // start time
  while(OSTCBCur->compTime > 4);

  OSMutexPend(Mutex[0], 128, &err);
  while(OSTCBCur->compTime > 2) print();

  OSMutexPend(Mutex[1], 128, &err);
  while(OSTCBCur->compTime > 0) print();

  OSMutexPost(Mutex[1]);
  OSMutexPost(Mutex[0]);
  print();
  OSTimeDly(128);
}

void S1_L1 (void* pdata) {
  OSTCBCur->compTime = 6;
  INT8U err;
  OSTimeDly(4); // start time
  while(OSTCBCur->compTime > 4) print();

  OSMutexPend(Mutex[1], 128, &err);
  while(OSTCBCur->compTime > 0) print();

  OSMutexPost(Mutex[1]);
  print();
  OSTimeDly(128);
}

void S1_L2 (void* pdata) {
  OSTCBCur->compTime = 9;
  INT8U err;
  while(OSTCBCur->compTime > 7) print();

  OSMutexPend(Mutex[0], 128, &err);
  while(OSTCBCur->compTime > 0) print();

  OSMutexPost(Mutex[0]);
  print();
  OSTimeDly(128);
}

void S2_H (void* pdata) {
  OSTCBCur->compTime = 11;
  INT8U err;
  OSTimeDly(5); // start time
  while(OSTCBCur->compTime > 9) print();

  OSMutexPend(Mutex[1], 128, &err);
  while(OSTCBCur->compTime > 6) print();

  OSMutexPend(Mutex[0], 128, &err);
  while(OSTCBCur->compTime > 3) print();;

  OSMutexPost(Mutex[0]);
  while(OSTCBCur->compTime > 0) print();;

  OSMutexPost(Mutex[1]);
  print();
  OSTimeDly(128);
}

void S2_L (void* pdata) {
  OSTCBCur->compTime = 12;
  INT8U err;
  while(OSTCBCur->compTime > 10) print();

  OSMutexPend(Mutex[0], 128, &err);
  while(OSTCBCur->compTime > 4) print();

  OSMutexPend(Mutex[1], 128, &err);
  while(OSTCBCur->compTime > 2) print();

  OSMutexPost(Mutex[1]);
  while(OSTCBCur->compTime > 0) print();

  OSMutexPost(Mutex[0]);
  print();
  OSTimeDly(128);
}

/* The main function creates two task and starts multi-tasking */
int main(void)
{
  OSInit();
  INT8U err;
  Mutex[0] = OSMutexCreate(1, &err);
  Mutex[1] = OSMutexCreate(2, &err);
#if SCENARIO == 1
  OSTaskCreate(S1_H, (void *)&TaskData[0], &TaskStk[0][TASK_STACKSIZE - 1], 3);
  OSTaskCreate(S1_L1, (void *)&TaskData[1], &TaskStk[1][TASK_STACKSIZE - 1], 4);
  OSTaskCreate(S1_L2, (void *)&TaskData[2], &TaskStk[2][TASK_STACKSIZE - 1], 5);
#elif SCENARIO == 2
  OSTaskCreate(S2_H, (void *)&TaskData[0], &TaskStk[0][TASK_STACKSIZE - 1], 3);
  OSTaskCreate(S2_L, (void *)&TaskData[1], &TaskStk[1][TASK_STACKSIZE - 1], 4);
#endif
  OSTaskCreate(print, (void *)0, &TaskStk[N_TASKS][TASK_STACKSIZE - 1], 10);

  OSTimeSet(0);
  OSStart();
  return 0;
}

/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2004 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
* Altera does not recommend, suggest or require that this reference design    *
* file be used in conjunction or combination with any other product.          *
******************************************************************************/
