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
 * In this demo, 3 tasks are created with different stack sizes.
 * Using a print function, the demo will show the heap size before and after
 * each creation and before and after each deletion.
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

/* Standard includes. */
#include <stdio.h>
#include <string.h>


/* Demo defines. */
#define main_DEMO 1

/* printf() output uses the UART.  These constants define the addresses of the
required UART registers. */
#define UART0_ADDRESS (0x40004000UL)
#define UART0_DATA (*(((volatile uint32_t *)(UART0_ADDRESS + 0UL))))
#define UART0_STATE (*(((volatile uint32_t *)(UART0_ADDRESS + 4UL))))
#define UART0_CTRL (*(((volatile uint32_t *)(UART0_ADDRESS + 8UL))))
#define UART0_BAUDDIV (*(((volatile uint32_t *)(UART0_ADDRESS + 16UL))))
#define TX_BUFFER_MASK (1UL)

/*
 * Printf() output is sent to the serial port.  Initialise the serial hardware.
 */
static void prvUARTInit(void);

/* The period of each task. The times are converted from
milliseconds to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK_FREQUENCY1_MS pdMS_TO_TICKS(4000UL) /* 2 seconds. */
#define mainTASK_FREQUENCY2_MS pdMS_TO_TICKS(6000UL) /* 3 seconds. */
#define mainTASK_FREQUENCY3_MS pdMS_TO_TICKS(8000UL) /* 4 seconds. */

/*-----------------------------------------------------------*/
static char const *const pcText1 = "Task 1 is running";
static char const *const pcText2 = "Task 2 is running";
static char const *const pcText3 = "Task 3 is running";
static char const *const pcText4 = "Task 4 is running";
static char const *const pcText5 = "Task 5 is running";
static char const *const pcText6 = "Task 6 is running";

/* The functions that implement the tasks. */
static void vTask1(void *pvParameters);
static void vTask2(void *pvParameters);
static void vTask3(void *pvParameters);


/*-----------------------------------------------------------*/

// Function to print the current free heap size
void printFreeHeapSize(const char *label)
{
    size_t freeHeapSize = xPortGetFreeHeapSize();
    printf("%s: Free heap size = %u bytes\n", label, (unsigned int)freeHeapSize);
}

void main(void)
{
    /* Hardware initialisation.  printf() output uses the UART for IO. */
    prvUARTInit();

    /* Create the tasks and print heap size after each creation */
    xTaskCreate(vTask1, "Task 1", configMINIMAL_STACK_SIZE * 3, (void *)pcText1, 1, NULL);
    printFreeHeapSize("Heap size after Task 1 creation");

    xTaskCreate(vTask2, "Task 2", configMINIMAL_STACK_SIZE * 3, (void *)pcText2, 2, NULL);
    printFreeHeapSize("Heap size after Task 2 creation");

    xTaskCreate(vTask3, "Task 3", configMINIMAL_STACK_SIZE * 3, (void *)pcText3, 3, NULL);
    printFreeHeapSize("Heap size after Task 3 creation");

    xTaskCreate(vTask1, "Task 4", configMINIMAL_STACK_SIZE * 4, (void *)pcText4, 4, NULL);
    printFreeHeapSize("Heap size after Task 4 creation");

    xTaskCreate(vTask2, "Task 5", configMINIMAL_STACK_SIZE * 4, (void *)pcText5, 5, NULL);
    printFreeHeapSize("Heap size after Task 5 creation");

    xTaskCreate(vTask3, "Task 6", configMINIMAL_STACK_SIZE * 4, (void *)pcText6, 6, NULL);
    printFreeHeapSize("Heap size after Task 6 creation");

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following
    line will never be reached.  If the following line does execute, then
    there was insufficient FreeRTOS heap memory available for the idle and/or
    timer tasks to be created.  See the memory management section on the
    FreeRTOS web site for more details.  NOTE: This demo uses static allocation
    for the idle and timer tasks so this line should never execute. */
    for (;;);
}

/*-----------------------------------------------------------*/
static void vTask1(void *pvParameters)
{
    TickType_t xNextWakeTime;
    const TickType_t xBlockTime = mainTASK_FREQUENCY1_MS;
    const char *pcTaskName;
    pcTaskName = (char *)pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    int i = 0;

    for (;;)
    {
        printFreeHeapSize(pcTaskName);

        if (i == 0){
            vTaskDelayUntil(&xNextWakeTime, xBlockTime);
        }else{
            break;
        }
        i++;
    }
    vTaskDelete(NULL);
}


static void vTask2(void *pvParameters)
{
    TickType_t xNextWakeTime;
    const TickType_t xBlockTime = mainTASK_FREQUENCY2_MS;
    const char *pcTaskName;
    pcTaskName = (char *)pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    int i = 0;

    for (;;)
    {
        printFreeHeapSize(pcTaskName);

        if (i == 0){
            vTaskDelayUntil(&xNextWakeTime, xBlockTime);
        }else{
            break;
        }
        i++;
    }
    vTaskDelete(NULL);
}

static void vTask3(void *pvParameters)
{
    TickType_t xNextWakeTime;
    const TickType_t xBlockTime = mainTASK_FREQUENCY3_MS;
    const char *pcTaskName;
    pcTaskName = (char *)pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    int i = 0;

    for (;;)
    {
        printFreeHeapSize(pcTaskName);

        if (i == 0){
            vTaskDelayUntil(&xNextWakeTime, xBlockTime);
        }else{
            break;
        }
        i++;
    }
    vTaskDelete(NULL);
}

/*-----------------------------------------------------------*/

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
	 * configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	 * added here, but the tick hook is called from an interrupt context, so
	 * code must not attempt to block, and only the interrupt safe FreeRTOS API
	 * functions can be used (those that end in FromISR()). */

	#if (main_DEMO != 1)
		{
			extern void vFullDemoTickHookFunction(void);

			vFullDemoTickHookFunction();
		}
	#endif /* main_DEMO */
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

int __write(int iFile, char *pcString, int iStringLength)
{
	int iNextChar;

	/* Avoid compiler warnings about unused parameters. */
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

	/* This project uses heap_4 so doesn't set up a heap for use by the C
	library - but something is calling the C library malloc().  See
	https://freertos.org/a00111.html for more information. */
	printf("\r\n\r\nUnexpected call to malloc() - should be usine pvPortMalloc()\r\n");
	portDISABLE_INTERRUPTS();
	for (;;)
		;
}