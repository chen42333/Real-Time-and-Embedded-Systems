/*
 * FreeRTOS V202107.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/******************************************************************************
 * NOTE 1:  This project provides two demo applications.  A simple blinky
 * style project, and a more comprehensive test and demo application.  The
 * mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting in main.c is used to select
 * between the two.  See the notes on using mainCREATE_SIMPLE_BLINKY_DEMO_ONLY
 * in main.c.  This file implements the simply blinky style version.
 *
 * NOTE 2:  This file only contains the source code that is specific to the
 * basic demo.  Generic functions, such FreeRTOS hook functions, and functions
 * required to configure the hardware are defined in main.c.
 ******************************************************************************
 *
 * main_blinky() creates one queue, and two tasks.  It then starts the
 * scheduler.
 *
 * The Queue Send Task:
 * The queue send task is implemented by the prvQueueSendTask() function in
 * this file.  prvQueueSendTask() sits in a loop that causes it to repeatedly
 * block for 200 milliseconds, before sending the value 100 to the queue that
 * was created within main_blinky().  Once the value is sent, the task loops
 * back around to block for another 200 milliseconds...and so on.
 *
 * The Queue Receive Task:
 * The queue receive task is implemented by the prvQueueReceiveTask() function
 * in this file.  prvQueueReceiveTask() sits in a loop where it repeatedly
 * blocks on attempts to read data from the queue that was created within
 * main_blinky().  When data is received, the task checks the value of the
 * data, and if the value equals the expected 100, toggles an LED.  The 'block
 * time' parameter passed to the queue receive function specifies that the
 * task should be held in the Blocked state indefinitely to wait for data to
 * be available on the queue.  The queue receive task will only leave the
 * Blocked state when the queue send task writes to the queue.  As the queue
 * send task writes to the queue every 200 milliseconds, the queue receive
 * task leaves the Blocked state every 200 milliseconds, and therefore toggles
 * the LED every 200 milliseconds.
 */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Standard demo includes. */
#include "partest.h"
#include <stdio.h>

#define LAB 4

#ifdef LAB == 4

#define TASKSET 0
#if TASKSET == 0
	#define N_TASKS 2
	int TaskSet[N_TASKS][2] = {{1,3}, {3,5}};
#elif TASKSET == 1
	#define N_TASKS 3
	int TaskSet[N_TASKS][2] = {{1,4}, {2,5}, {2,10}};
#endif

#endif


/*-----------------------------------------------------------*/

/*
 * Called by main when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 1 in
 * main.c.
 */
void main_blinky( void );

/*
 * The tasks as described in the comments at the top of this file.
 */
#ifdef LAB == 4
static void periodicTask( void *pvParameters );
static void idleTask(void*);
#endif

void main_blinky( void )
{

#ifdef LAB == 4
	printf("Taskset %d\n", TASKSET);

	int i;
    char name[N_TASKS][3];
	for (i = 0; i < N_TASKS; i++) {
        name[i][0] = 'T';
        name[i][1] = '1' + i;
        name[i][2] = 0;
        printf("T%d c:%d p:%d start\n", i + 1, TaskSet[i][0], TaskSet[i][1]);
		xTaskCreate(
			periodicTask,
			name[i],
			configMINIMAL_STACK_SIZE,
			(void*)i,
			EDF_TASK_PRIO,
			NULL
		);
	}

    xTaskCreate(
        idleTask,
        "Idle",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL
    );
#endif

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the Idle and/or
	timer tasks to be created.  See the memory management section on the
	FreeRTOS web site for more details on the FreeRTOS heap
	http://www.freertos.org/a00111.html. */
	for( ;; );
}
/*-----------------------------------------------------------*/

#ifdef LAB == 4
static void periodicTask (void *pdata)
{
	int toDelay;
	int id = (int)(long)pdata;
	int c = TaskSet[id][0];
	int p = TaskSet[id][1];

	task_set_deadline(p);
	task_set_comptime(c);
    vTaskDelay(0);

	while (1) {
		while (task_get_comptime() > 0);
        // printf("%d: t%d c %d %d %d\n", xTaskGetTickCount(), id+1, id+1, 12345, 5);
		toDelay = task_get_deadline() - xTaskGetTickCount();
        // printf("T%d complete / delay %d / time %d\n", id + 1, toDelay, xTaskGetTickCount());
		task_set_comptime(c);
		task_set_deadline(task_get_deadline() + p);
		if (toDelay >= 0) vTaskDelay(toDelay);
	}
}

static void idleTask(void* x) {
	int time, event; char *from, *to;
    while (1) {
		while (pop_info(&time, &event, &from, &to)) {
            switch (event) {
            case PREEMPT:
            case COMPLETE:
                printf("%d\t%s\t%s\t%s\n", time, event == PREEMPT ? "Preempt\t" : "Complete", from, to);
                break;
            case EXCEED:
				printf("time:%d %s exceed deadline\n", time, from);
				for (;;);
            }
		}
    }
}
#endif
