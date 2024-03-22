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
#define  N_TASKS                  3

OS_STK        TaskStk[N_TASKS][TASK_STACKSIZE];        /* Tasks stacks                                  */
INT8U         TaskData[N_TASKS];                      /* Parameters to pass to each task               */

INT16U TaskSet[3][2] = {{1,3}, {3,6}, {4,9}};

void  Task (void *pdata)
{
    int start = OSTimeGet();
    int toDelay;
    int id = *(INT8U*)pdata;
    int c = TaskSet[id][0];
    int p = TaskSet[id][1];
    long time; int event; INT8U from, to;

    while (1) {
        while (OSTCBCur->compTime > 0);
        toDelay = p - (OSTimeGet() - start);
        start += p;
        OSTCBCur->compTime = c;
        OSTCBCur->deadline = start + p;
        if (toDelay >= 0) OSTimeDly(toDelay);

        while (pop_info(&time, &event, &from, &to)) {
        	if (event == EXCEED) {
                printf("time:%lu task%hhu exceed deadline\n", time, from);
                OSSchedLock();
                for (;;);
        	} else {
				printf("%lu\t%s\t%2hhu\t%2hhu\n",
					time, event == COMPLETE ? "Complete" : "Preempt", from, to);
        	}
        }
    }
}

/* The main function creates two task and starts multi-tasking */
int main(void)
{

    INT8U i;
    for (i = 0; i < N_TASKS; i++) {
        TaskData[i] = i;
        OSTaskCreate(Task, (void *)&TaskData[i], &TaskStk[i][TASK_STACKSIZE - 1], i + 1);
        OSTCBPrioTbl[i+1]->compTime = TaskSet[i][0];
        OSTCBPrioTbl[i+1]->deadline = TaskSet[i][1];
    }


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
