/*
 * FreeRTOS V202212.01
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
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/******************************************************************************
 * See https://www.freertos.org/freertos-on-qemu-mps2-an385-model.html for
 * instructions.
 *
 * This demo is used to show the use of a new scheduler.
 * Specifically, the new algorithm implements Rate Monotonic Scheduling.
 * It is designed specifically for periodic tasks.
 *
 * Running in QEMU:
 * Use the following commands to start the application running in a way that
 * enables the debugger to connect, omit the "-s -S" to run the project without
 * the debugger:
 * qemu-system-arm -machine mps2-an385 -cpu cortex-m3 -kernel [path-to]/RTOSDemo.out -nographic -serial stdio -semihosting -semihosting-config enable=on,target=native -s -S
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* This project provides two demo applications.  A simple blinky style demo
application, and a more comprehensive test and demo application.  The
mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting is used to select between the two.

If mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is 1 then the blinky demo will be built.
The blinky demo is implemented and described in main_blinky.c.

If mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is not 1 then the comprehensive test and
demo application will be built.  The comprehensive test and demo application is
implemented and described in main_full.c. */
#define mainCREATE_SIMPLE_BLINKY_DEMO_ONLY 1

/* printf() output uses the UART.  These constants define the addresses of the
required UART registers. */
#define UART0_ADDRESS (0x40004000UL)
#define UART0_DATA (*(((volatile uint32_t *)(UART0_ADDRESS + 0UL))))
#define UART0_STATE (*(((volatile uint32_t *)(UART0_ADDRESS + 4UL))))
#define UART0_CTRL (*(((volatile uint32_t *)(UART0_ADDRESS + 8UL))))
#define UART0_BAUDDIV (*(((volatile uint32_t *)(UART0_ADDRESS + 16UL))))
#define TX_BUFFER_MASK (1UL)

// Standard includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

/*********************************************************************************************************
 *												MACROS
 *********************************************************************************************************/
#define mainCREATE_SIMPLE_BLINKY_DEMO_ONLY 1

#define mainDELAY_LOOP_COUNT 100
#define ROUND_ROBIN_TIME_SLICE 1000 // Time slice for round-robin scheduling.

/* Priority levels for tasks.
	Note that: low priority numbers denote low priority tasks. */
#define TASK_PRIORITY tskIDLE_PRIORITY + 1U

/* The period of each task. The times are converted from
milliseconds to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK_FREQUENCY1_MS pdMS_TO_TICKS(20000UL)
#define mainTASK_FREQUENCY2_MS pdMS_TO_TICKS(30000UL)
#define mainTASK_FREQUENCY3_MS pdMS_TO_TICKS(40000UL)
#define mainTIMER_FREQUENCY_MS pdMS_TO_TICKS(120000UL)

/* Dimensions of the buffer that the task being created will use as its stack.
	NOTE:  This is the number of words the stack will hold, not the number of bytes.
	For example, if each stack item is 32-bits, and this is set to 100, then 400 bytes (100 * 32-bits) will be allocated. */
#define STACK_SIZE 200

/*********************************************************************************************************
 *										UART: printf handling
 *********************************************************************************************************/
// printf() output uses the UART.  These constants define the addresses of the required UART registers.
#define UART0_ADDRESS (0x40004000UL)									 // Base address of the UART peripheral
#define UART0_DATA (*(((volatile uint32_t *)(UART0_ADDRESS + 0UL))))	 // Register to write data to for transmission
#define UART0_STATE (*(((volatile uint32_t *)(UART0_ADDRESS + 4UL))))	 // Register that holds the status of the UART
#define UART0_CTRL (*(((volatile uint32_t *)(UART0_ADDRESS + 8UL))))	 // Control register for configuring the UART
#define UART0_BAUDDIV (*(((volatile uint32_t *)(UART0_ADDRESS + 16UL)))) // Register for configuring the baud rate
#define TX_BUFFER_MASK (1UL)											 // Constant used for masking the transmission buffer

// Initialization function to set up the UART peripheral => Printf() output is sent to the serial port
static void prvUARTInit(void);

/*********************************************************************************************************
 *										  APPLICATION GLOBALS
 *********************************************************************************************************/


TaskHandle_t xHandle_1 = NULL;
TaskHandle_t xHandle_2 = NULL;
TaskHandle_t xHandle_3 = NULL;

/* Timer required. */

/*
 * The callback function executed when the software timer expires.
 */
static void timerCallback(TimerHandle_t xTimerHandle);

/* A software timer that is started from the tick hook. */
static TimerHandle_t xTimer = NULL;

/*********************************************************************************************************
 *									   LOCAL FUNCTION PROTOTYPES
 *********************************************************************************************************/
// Function prototypes for task functions and timer callback
static void vTask1(void *pvParameters);
static void vTask2(void *pvParameters);
static void vTask3(void *pvParameters);
/**********************************************************************************************************/
void main(void)
{
	prvUARTInit();

	BaseType_t xReturned_1, xReturned_2, xReturned_3;

	// Task creation
	xReturned_1 = xTaskCreate(vTask1,		 // Pointer to the task entry function (name of function)
							  "vTask1",		 // Name for the task
							  STACK_SIZE,	 // The number of words (not bytes!) to allocate for use as the task's stack
							  (void *)1,	 // A value that is passed as the paramater to the created task
							  TASK_PRIORITY, // Priority at which the task is created
							  &xHandle_1,	 // Used to pass out the created task's handle
							  8,			 // Estimation of task's CPU Burst in seconds
							  2);			 // Period of a task. It is expressed in tenths of a second and it is limited to 100.

	xReturned_2 = xTaskCreate(vTask2, "vTask2", STACK_SIZE, (void *)1, TASK_PRIORITY, &xHandle_2, 8, 3);
	xReturned_3 = xTaskCreate(vTask3, "vTask3", STACK_SIZE, (void *)1, TASK_PRIORITY, &xHandle_3, 7, 4);

	if (xReturned_1 == pdPASS && xReturned_2 == pdPASS && xReturned_3 == pdPASS)
	{
		xTimer = xTimerCreate("Timer",				  /* The text name assigned to the software timer - for debug only as it is not used by the kernel. */
							  mainTIMER_FREQUENCY_MS, /* The period of the software timer in ticks. */
							  pdTRUE,				  /* xAutoReload is set to pdTRUE, so this is an auto-reload timer. */
							  NULL,					  /* The timer's ID is not used. */
							  timerCallback);		  /* The function executed when the timer expires. */
		xTimerStart(xTimer, 0);						  /* The scheduler has not started so use a block time of 0. */
		vTaskStartScheduler();
	}
	else
		printf("Error creating tasks. Code 1: %ld, Code 2: %ld, Code 3: %ld \r\n", xReturned_1, xReturned_2, xReturned_3);

	for (;;)
		;
}

/**********************************************************************************************************/
void vTask1(void *pvParameters)
{
	TickType_t xNextWakeTime;

	xNextWakeTime = xTaskGetTickCount();

	const TickType_t xBlockTime = pdMS_TO_TICKS(10000 * uxTaskPeriodGet(NULL));

	int timeSpent = 0;

	int runTime = uxTaskCpuBurstGet(NULL);

	// Obtain the tick count corresponding to one second
	const TickType_t xOneSecondTicks = pdMS_TO_TICKS(1000);

	for (;;)
	{

		// Run until the specified time has elapsed
		printf("%s is running. Start time: %d\n", uxTaskNameGet(NULL), xTaskGetTickCount()/1000);

		while (timeSpent < runTime)
		{
			TickType_t xStartTick = xTaskGetTickCount();

			// Busy-wait for one second (tick count equivalent to one second)
			while ((xTaskGetTickCount() - xStartTick) < xOneSecondTicks)
			{
				// Ensure the task does not yield the CPU during this period
				// (This loop will keep running until one second has passed)
			}

			// One second has passed, increment timeSpent
			timeSpent++;
		}

		printf("%s finished at time %d.\n", uxTaskNameGet(NULL), xTaskGetTickCount()/1000);

		/* Place this task in the blocked state until it is time to run again.
		The block time is specified in ticks, pdMS_TO_TICKS() was used to
		convert a time specified in milliseconds into a time specified in ticks.
		While in the Blocked state this task will not consume any CPU time. */
		vTaskDelayUntil(&xNextWakeTime, ((int)pvParameters * xBlockTime) - xNextWakeTime);

		xTaskCreate(vTask1, "vTask1", STACK_SIZE, (void *)pvParameters + 1, TASK_PRIORITY, &xHandle_1, 8, 2);

		break;
	}

	vTaskDelete(NULL);
}
/*-----------------------------------------------------------*/

void vTask2(void *pvParameters)
{
	TickType_t xNextWakeTime;

	xNextWakeTime = xTaskGetTickCount();

	const TickType_t xBlockTime = pdMS_TO_TICKS(10000 * uxTaskPeriodGet(NULL));

	int timeSpent = 0;

	int runTime = uxTaskCpuBurstGet(NULL);

	// Obtain the tick count corresponding to one second
	const TickType_t xOneSecondTicks = pdMS_TO_TICKS(1000);

	for (;;)
	{

		// Run until the specified time has elapsed
		printf("%s is running. Start time: %d\n", uxTaskNameGet(NULL), xTaskGetTickCount()/1000);

		while (timeSpent < runTime)
		{
			TickType_t xStartTick = xTaskGetTickCount();

			// Busy-wait for one second (tick count equivalent to one second)
			while ((xTaskGetTickCount() - xStartTick) < xOneSecondTicks)
			{
				// Ensure the task does not yield the CPU during this period
				// (This loop will keep running until one second has passed)
			}

			// One second has passed, increment timeSpent
			timeSpent++;
		}

		printf("%s finished at time %d.\n", uxTaskNameGet(NULL), xTaskGetTickCount()/1000);

		/* Place this task in the blocked state until it is time to run again.
		The block time is specified in ticks, pdMS_TO_TICKS() was used to
		convert a time specified in milliseconds into a time specified in ticks.
		While in the Blocked state this task will not consume any CPU time. */

		vTaskDelayUntil(&xNextWakeTime, ((int)pvParameters * xBlockTime) - xNextWakeTime);

		xTaskCreate(vTask2, "vTask2", STACK_SIZE, (void *)pvParameters + 1, TASK_PRIORITY, &xHandle_2, 8, 3);

		break;
	}

	vTaskDelete(NULL);
}
/*-----------------------------------------------------------*/

void vTask3(void *pvParameters)
{
	TickType_t xNextWakeTime;

	xNextWakeTime = xTaskGetTickCount();

	const TickType_t xBlockTime = pdMS_TO_TICKS(10000 * uxTaskPeriodGet(NULL));

	int timeSpent = 0;

	int runTime = uxTaskCpuBurstGet(NULL);

	// Obtain the tick count corresponding to one second
	const TickType_t xOneSecondTicks = pdMS_TO_TICKS(1000);

	for (;;)
	{

		// Run until the specified time has elapsed
		printf("%s is running. Start time: %d\n", uxTaskNameGet(NULL), xTaskGetTickCount()/1000);

		while (timeSpent < runTime)
		{
			TickType_t xStartTick = xTaskGetTickCount();

			// Busy-wait for one second (tick count equivalent to one second)
			while ((xTaskGetTickCount() - xStartTick) < xOneSecondTicks)
			{
				// Ensure the task does not yield the CPU during this period
				// (This loop will keep running until one second has passed)
			}

			// One second has passed, increment timeSpent
			timeSpent++;
		}

		printf("%s finished at time %d.\n", uxTaskNameGet(NULL), xTaskGetTickCount()/1000);

		/* Place this task in the blocked state until it is time to run again.
		The block time is specified in ticks, pdMS_TO_TICKS() was used to
		convert a time specified in milliseconds into a time specified in ticks.
		While in the Blocked state this task will not consume any CPU time. */

		vTaskDelayUntil(&xNextWakeTime, ((int)pvParameters * xBlockTime) - xNextWakeTime);

		xTaskCreate(vTask3, "vTask3", STACK_SIZE, (void *)pvParameters + 1, TASK_PRIORITY, &xHandle_3, 7, 4);

		break;
	}

	vTaskDelete(NULL);
}
/* -------------------------------------------------- */
static void timerCallback(TimerHandle_t xTimerHandle)
{
	/* This is the software timer callback function.  The software timer has a
	period of two seconds and is reset each time a key is pressed.  This
	callback function will execute if the timer expires, which will only happen
	if a key is not pressed for two seconds. */

	/* Avoid compiler warnings resulting from the unused parameter. */
	(void)xTimerHandle;
	printf("From here on, the order of execution is always the same.\n");
}

/**********************************************************************************************************
 *											Already defined Functions
 *********************************************************************************************************/
void vApplicationMallocFailedHook(void)
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created using the dynamic allocation (as opposed to
	static allocation) option.  It is also called by various parts of the
	demo application.  If heap_1.c, heap_2.c or heap_4.c is being used, then the
	size of the	heap available to pvPortMalloc() is defined by
	configTOTAL_HEAP_SIZE in FreeRTOSConfig.h, and the xPortGetFreeHeapSize()
	API function can be used to query the size of free heap space that remains
	(although it does not provide information on how the remaining heap might be
	fragmented).  See http://www.freertos.org/a00111.html for more
	information. */
	printf("\r\n\r\nMalloc failed\r\n");
	portDISABLE_INTERRUPTS();
	for (;;)
		;
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void)
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If application tasks make use of the
	vTaskDelete() API function to delete themselves then it is also important
	that vApplicationIdleHook() is permitted to return to its calling function,
	because it is the responsibility of the idle task to clean up memory
	allocated by the kernel to any task that has since deleted itself. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
	(void)pcTaskName;
	(void)pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	printf("\r\n\r\nStack overflow in %s\r\n", pcTaskName);
	portDISABLE_INTERRUPTS();
	for (;;)
		;
}
/*-----------------------------------------------------------*/

void vApplicationTickHook(void)
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()). */

#if (mainCREATE_SIMPLE_BLINKY_DEMO_ONLY != 1)
	{
		extern void vFullDemoTickHookFunction(void);

		vFullDemoTickHookFunction();
	}
#endif /* mainCREATE_SIMPLE_BLINKY_DEMO_ONLY */
}
/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook(void)
{
	/* This function will be called once only, when the daemon task starts to
	execute (sometimes called the timer task).  This is useful if the
	application includes initialisation code that would benefit from executing
	after the scheduler has been started. */
}
/*-----------------------------------------------------------*/

void vAssertCalled(const char *pcFileName, uint32_t ulLine)
{
	volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;

	/* Called if an assertion passed to configASSERT() fails.  See
	http://www.freertos.org/a00110.html#configASSERT for more information. */

	printf("ASSERT! Line %d, file %s\r\n", (int)ulLine, pcFileName);

	taskENTER_CRITICAL();
	{
		/* You can step out of this function to debug the assertion by using
		the debugger to set ulSetToNonZeroInDebuggerToContinue to a non-zero
		value. */
		while (ulSetToNonZeroInDebuggerToContinue == 0)
		{
			__asm volatile("NOP");
			__asm volatile("NOP");
		}
	}
	taskEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
	/* If the buffers to be provided to the Idle task are declared inside this
	function then they must be declared static - otherwise they will be allocated on
	the stack and so not exists after this function exits. */
	static StaticTask_t xIdleTaskTCB;
	static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

	/* Pass out a pointer to the StaticTask_t structure in which the Idle task's
	state will be stored. */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Pass out the array that will be used as the Idle task's stack. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
{
	/* If the buffers to be provided to the Timer task are declared inside this
	function then they must be declared static - otherwise they will be allocated on
	the stack and so not exists after this function exits. */
	static StaticTask_t xTimerTaskTCB;
	static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

	/* Pass out a pointer to the StaticTask_t structure in which the Timer
	task's state will be stored. */
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

	/* Pass out the array that will be used as the Timer task's stack. */
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;

	/* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
/*-----------------------------------------------------------*/

static void prvUARTInit(void)
{
	UART0_BAUDDIV = 16;
	UART0_CTRL = 1;
}
/*-----------------------------------------------------------*/

int _write(int file, char *ptr, int len)
{
	(void)file; // Ignore the file parameter

	int i;
	for (i = 0; i < len; i++)
	{
		while (!(UART0_STATE & (1 << 5)))
			;				 // Wait for UART0 TX buffer to be empty
		UART0_DATA = ptr[i]; // Send character to UART0
	}

	return i; // Return number of characters written
}

int __write(int iFile, char *pcString, int iStringLength)
{
	int iNextChar;

	/* Avoid compiler warnings about unused parameters.*/
	(void)iFile;

	/* Output the formatted string to the UART. */
	for (iNextChar = 0; iNextChar < iStringLength; iNextChar++)
	{
		while ((UART0_STATE & TX_BUFFER_MASK) != 0)
			;
		UART0_DATA = *pcString;
		pcString++;
	}

	return iStringLength;
}
/*-----------------------------------------------------------*/

void *malloc(size_t size)
{
	(void)size;
	return pvPortMalloc(size);

	/* This project uses heap_4 so doesn't set up a heap for use by the C
	library - but something is calling the C library malloc().  See
	https://freertos.org/a00111.html for more information. */
	// printf( "\r\n\r\nUnexpected call to malloc() - should be usine pvPortMalloc()\r\n" );
	// portDISABLE_INTERRUPTS();
	// for( ;; );
}