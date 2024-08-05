
---------------------------------------------------------------------------------------
1. Tasks (Each task is a separate thread of execution, with its own stack and context)
--------------------------------------------------------------------------------------

 xTaskCreate(
        vTaskFunction,          // Task function
        "TaskName",             // Task name
        configMINIMAL_STACK_SIZE, // Stack size in words (number of words the stack can hold)
        NULL,                   // Task parameter
        tskIDLE_PRIORITY + 1,   // Task priority (higher numbers representing higher priorities.
        NULL                    // Task handle, used to reference the task in other API calls
    );

----------------------------------------------------------------------------------------------------
//Task Creation and Management
#include <FreeRTOS.h>
#include <task.h>

void vTaskFunction(void *pvParameters) {
    while(1) {
        // Task code goes here
    }
}

void main() {
    xTaskCreate(vTaskFunction, "Task 1", 1000, NULL, 1, NULL);
    vTaskStartScheduler();
}
-------------------------------------------------------------------------------------------
//Example: Creating a Task as a Member Function

#include "FreeRTOS.h"
#include "task.h"

class MyClass {
public:
    void startTasks() {
        xTaskCreate(taskWrapper, "Task1", 1000, this, 1, NULL);
        xTaskCreate(taskWrapper, "Task2", 1000, this, 1, NULL);
    }

private:
    static void taskWrapper(void *pvParameters) {
        MyClass *instance = static_cast<MyClass*>(pvParameters);
        instance->taskFunction();
    }

    void taskFunction() {
        while (true) {
            // Task code here
        }
    }
};

int main() {
    MyClass myClass;
    myClass.startTasks();
    vTaskStartScheduler();
    for (;;);
}

-------------------------------------------------------------------------------------------



--------------------------------------------------------------------------------
2.	Scheduler: (It decides which task should run next based on priority and task states (ready, running, blocked, suspended).)
--------------------------------------------------------------------------------

---------------------------------------------------------------------------------
#include <FreeRTOS.h>
#include <task.h>

void vHighPriorityTask(void *pvParameters);
void vLowPriorityTask(void *pvParameters);

int main(void) {
    // Create tasks
    xTaskCreate(vLowPriorityTask, "LowPriority", 1000, NULL, 1, NULL);
    xTaskCreate(vHighPriorityTask, "HighPriority", 1000, NULL, 2, NULL);

    // Start the scheduler
    vTaskStartScheduler();

    // Should never reach here
    for(;;);
}

void vHighPriorityTask(void *pvParameters) {
    for (;;) {
        // High-priority task code here
        vTaskDelay(pdMS_TO_TICKS(500)); // Delay for 0.5 seconds
    }
}

void vLowPriorityTask(void *pvParameters) {
    for (;;) {
        // Low-priority task code here
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
    }

---------------------------------------------------------------------------------

//Basic Task Creation with C++ Classes
//This example shows how to create tasks using C++ classes with FreeRTOS.
#include <FreeRTOS.h>
#include <task.h>
#include <iostream>

class TaskBase {
public:
    TaskBase(const char* name, UBaseType_t priority, uint16_t stackSize) {
        xTaskCreate(TaskEntry, name, stackSize, this, priority, &taskHandle);
    }

    virtual ~TaskBase() {
        vTaskDelete(taskHandle);
    }

protected:
    virtual void Run() = 0;

private:
    TaskHandle_t taskHandle;

    static void TaskEntry(void* pvParameters) {
        TaskBase* task = static_cast<TaskBase*>(pvParameters);
        task->Run();
    }
};

class Task1 : public TaskBase {
public:
    Task1() : TaskBase("Task1", 1, 1000) {}

protected:
    void Run() override {
        for (;;) {
            std::cout << "Task1 running\n";
            vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
        }
    }
};

class Task2 : public TaskBase {
public:
    Task2() : TaskBase("Task2", 2, 1000) {}

protected:
    void Run() override {
        for (;;) {
            std::cout << "Task2 running\n";
            vTaskDelay(pdMS_TO_TICKS(500)); // Delay for 0.5 seconds
        }
    }
};

int main() {
    Task1 t1;
    Task2 t2;

    vTaskStartScheduler();
    for (;;);
    return 0;
}
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
3.	Queues: (Used for communication between tasks or between tasks and interrupt service routines.)
--------------------------------------------------------------------
// Basic C sample code for the  Queues

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
QueueHandle_t xQueue;

void senderTask(void *pvParameters) {
    int data = 42;
    xQueueSend(xQueue, &data, portMAX_DELAY);
}

void receiverTask(void *pvParameters) {
    int receivedData;
    xQueueReceive(xQueue, &receivedData, portMAX_DELAY);
}

void main() {
    xQueue = xQueueCreate(10, sizeof(int));
    xTaskCreate(senderTask, "Sender", 1000, NULL, 1, NULL);
    xTaskCreate(receiverTask, "Receiver", 1000, NULL, 1, NULL);
    vTaskStartScheduler();
}
--------------------------------------------------------------

//Queues with C++ Classes
//C++ classes for task communication.

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <iostream>

class QueueTaskBase {
public:
    QueueTaskBase(const char* name, UBaseType_t priority, uint16_t stackSize, QueueHandle_t q)
        : queue(q) {
        xTaskCreate(TaskEntry, name, stackSize, this, priority, &taskHandle);
    }

    virtual ~QueueTaskBase() {
        vTaskDelete(taskHandle);
    }

protected:
    QueueHandle_t queue;
    virtual void Run() = 0;

private:
    TaskHandle_t taskHandle;

    static void TaskEntry(void* pvParameters) {
        QueueTaskBase* task = static_cast<QueueTaskBase*>(pvParameters);
        task->Run();
    }
};

class SenderTask : public QueueTaskBase {
public:
    SenderTask(QueueHandle_t q) : QueueTaskBase("Sender", 1, 1000, q) {}

protected:
    void Run() override {
        int count = 0;
        for (;;) {
            std::cout << "Sending value: " << count << '\n';
            xQueueSend(queue, &count, portMAX_DELAY);
            count++;
            vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
        }
    }
};

class ReceiverTask : public QueueTaskBase {
public:
    ReceiverTask(QueueHandle_t q) : QueueTaskBase("Receiver", 2, 1000, q) {}

protected:
    void Run() override {
        int receivedValue;
        for (;;) {
            if (xQueueReceive(queue, &receivedValue, portMAX_DELAY) == pdTRUE) {
                std::cout << "Received value: " << receivedValue << '\n';
            }
        }
    }
};

int main() {
    QueueHandle_t xQueue = xQueueCreate(10, sizeof(int));

    if (xQueue == NULL) {
        std::cerr << "Failed to create queue\n";
        return 1;
    }

    SenderTask sender(xQueue);
    ReceiverTask receiver(xQueue);

    vTaskStartScheduler();
    for (;;);
    return 0;
}


--------------------------------------------------------------------------------
4.	Semaphores and Mutexes:  (Synchronization mechanisms to manage resource sharing and protect critical sections.)
--------------------------------------------------------------------------------
// Sample C code
--------------------------------------
SemaphoreHandle_t xSemaphore;

void task1(void *pvParameters) {
    if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
        // Access shared resource
        xSemaphoreGive(xSemaphore);
    }
}

void main() {
    xSemaphore = xSemaphoreCreateBinary();
    xTaskCreate(task1, "Task 1", 1000, NULL, 1, NULL);
    vTaskStartScheduler();
}
------------------------------------
//Sample C++ example 


#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <iostream>

class SemaphoreTaskBase {
public:
    SemaphoreTaskBase(const char* name, UBaseType_t priority, uint16_t stackSize, SemaphoreHandle_t sem)
        : semaphore(sem) {
        xTaskCreate(TaskEntry, name, stackSize, this, priority, &taskHandle);
    }

    virtual ~SemaphoreTaskBase() {
        vTaskDelete(taskHandle);
    }

protected:
    SemaphoreHandle_t semaphore;
    virtual void Run() = 0;

private:
    TaskHandle_t taskHandle;

    static void TaskEntry(void* pvParameters) {
        SemaphoreTaskBase* task = static_cast<SemaphoreTaskBase*>(pvParameters);
        task->Run();
    }
};

class ProducerTask : public SemaphoreTaskBase {
public:
    ProducerTask(SemaphoreHandle_t sem) : SemaphoreTaskBase("Producer", 1, 1000, sem) {}

protected:
    void Run() override {
        for (;;) {
            std::cout << "Producing item\n";
            xSemaphoreGive(semaphore); // Release semaphore
            vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
        }
    }
};

class ConsumerTask : public SemaphoreTaskBase {
public:
    ConsumerTask(SemaphoreHandle_t sem) : SemaphoreTaskBase("Consumer", 2, 1000, sem) {}

protected:
    void Run() override {
        for (;;) {
            if (xSemaphoreTake(semaphore, portMAX_DELAY) == pdTRUE) {
                std::cout << "Consuming item\n";
            }
            vTaskDelay(pdMS_TO_TICKS(500)); // Delay for 0.5 seconds
        }
    }
};

int main() {
    SemaphoreHandle_t xSemaphore = xSemaphoreCreateBinary();

    if (xSemaphore == NULL) {
        std::cerr << "Failed to create semaphore\n";
        return 1;
    }

    ProducerTask producer(xSemaphore);
    ConsumerTask consumer(xSemaphore);

    vTaskStartScheduler();
    for (;;);
    return 0;
}
------------------------------------------------------------



--------------------------------------------------------------------------------
5.	Timers: (Useful for periodic tasks or timeouts.)
-------------------------------------------------------------------------------
//Tasks can delay themselves using vTaskDelay, which puts the task in the blocked state for a specified number of ticks.

void task(void *pvParameters) {
    while(1) {
        // Task code
        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay for 1000 milliseconds
    }
}
//Timers can be created to execute a callback function after a specified period

void vTimerCallback(TimerHandle_t xTimer) {
    // Timer callback code
}

void main() {
    TimerHandle_t xTimer = xTimerCreate("Timer", pdMS_TO_TICKS(1000), pdTRUE, 0, vTimerCallback); 
    //pdTRUE : pdTRUE means that the timer is a periodic timer,
    //which will automatically restart itself after each expiration.
    xTimerStart(xTimer, 0);
    vTaskStartScheduler();
}


--------------------------------------------------------------------------------
6.	Event Groups: (A way to manage multiple events using a single variable. 
                Tasks can wait for one or more bits to be set within an event group.)
-------------------------------------------------------------------------------
//Allows tasks to wait for multiple events (bits) to be set before proceeding

//1.	Create Event Group:

EventGroupHandle_t xEventGroup;

void setup() {
    // Create the event group
    xEventGroup = xEventGroupCreate();
}

#define TASK1_COMPLETE_BIT (1 << 0)
#define TASK2_COMPLETE_BIT (1 << 1)

void vTask1(void *pvParameters) {
    // Task1 code
    // ...

    // Set the bit indicating Task1 is complete
    xEventGroupSetBits(xEventGroup, TASK1_COMPLETE_BIT);

    // Suspend the task
    vTaskSuspend(NULL);
}

void vTask2(void *pvParameters) {
    // Task2 code
    // ...

    // Set the bit indicating Task2 is complete
    xEventGroupSetBits(xEventGroup, TASK2_COMPLETE_BIT);

    // Suspend the task
    vTaskSuspend(NULL);
}

void vTask3(void *pvParameters) {
    // Wait for both Task1 and Task2 to complete
    xEventGroupWaitBits(
        xEventGroup,               // The event group being tested
        TASK1_COMPLETE_BIT | TASK2_COMPLETE_BIT, // The bits to wait for
        pdTRUE,                    // Clear the bits on exit
        pdTRUE,                    // Wait for all bits to be set
        portMAX_DELAY              // Wait indefinitely
    );

    // Task3 code
    // ...
}

void main() {
    xTaskCreate(vTask1, "Task 1", 1000, NULL, 1, NULL);
    xTaskCreate(vTask2, "Task 2", 1000, NULL, 1, NULL);
    xTaskCreate(vTask3, "Task 3", 1000, NULL, 1, NULL);

    // Start the scheduler
    vTaskStartScheduler();
}

-----------------------------------------------
//  C++ sample code

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

class EventGroupMultiTaskExample {
public:
    EventGroupMultiTaskExample();
    void startTasks();

private:
    static void task1Wrapper(void* pvParameters);
    static void task2Wrapper(void* pvParameters);
    static void task3Wrapper(void* pvParameters);

    void task1();
    void task2();
    void task3();

    EventGroupHandle_t eventGroup;
    static const EventBits_t TASK1_BIT = (1 << 0);
    static const EventBits_t TASK2_BIT = (1 << 1);
    static const EventBits_t TASK3_BIT = (1 << 2);
};

//.cpp

EventGroupMultiTaskExample::EventGroupMultiTaskExample() {
    eventGroup = xEventGroupCreate();
}

void EventGroupMultiTaskExample::startTasks() {
    xTaskCreate(task1Wrapper, "Task1", 1000, this, 1, NULL);
    xTaskCreate(task2Wrapper, "Task2", 1000, this, 1, NULL);
    xTaskCreate(task3Wrapper, "Task3", 1000, this, 1, NULL);
    vTaskStartScheduler();
}

void EventGroupMultiTaskExample::task1Wrapper(void* pvParameters) {
    EventGroupMultiTaskExample* instance = static_cast<EventGroupMultiTaskExample*>(pvParameters);
    instance->task1();
}

void EventGroupMultiTaskExample::task2Wrapper(void* pvParameters) {
    EventGroupMultiTaskExample* instance = static_cast<EventGroupMultiTaskExample*>(pvParameters);
    instance->task2();
}

void EventGroupMultiTaskExample::task3Wrapper(void* pvParameters) {
    EventGroupMultiTaskExample* instance = static_cast<EventGroupMultiTaskExample*>(pvParameters);
    instance->task3();
}

void EventGroupMultiTaskExample::task1() {
    while (true) {
        // Wait for Task1 bit to be set
        xEventGroupWaitBits(
            eventGroup,        // The event group being tested
            TASK1_BIT,         // The bit to wait for
            pdTRUE,            // Clear the bit on exit
            pdFALSE,           // Don't wait for all bits, just this one
            portMAX_DELAY      // Wait indefinitely
        );

        // Task1 code
        std::cout << "Task 1 event received, task running" << std::endl;
    }
}

void EventGroupMultiTaskExample::task2() {
    while (true) {
        // Wait for Task2 bit to be set
        xEventGroupWaitBits(
            eventGroup,        // The event group being tested
            TASK2_BIT,         // The bit to wait for
            pdTRUE,            // Clear the bit on exit
            pdFALSE,           // Don't wait for all bits, just this one
            portMAX_DELAY      // Wait indefinitely
        );

        // Task2 code
        std::cout << "Task 2 event received, task running" << std::endl;
    }
}

void EventGroupMultiTaskExample::task3() {
    while (true) {
        // Wait for Task3 bit to be set
        xEventGroupWaitBits(
            eventGroup,        // The event group being tested
            TASK3_BIT,         // The bit to wait for
            pdTRUE,            // Clear the bit on exit
            pdFALSE,           // Don't wait for all bits, just this one
            portMAX_DELAY      // Wait indefinitely
        );

        // Task3 code
        std::cout << "Task 3 event received, task running" << std::endl;
    }
}

int main() {
    EventGroupMultiTaskExample example;
    example.startTasks();

    // Simulate events after some time
    vTaskDelay(pdMS_TO_TICKS(1000));
    xEventGroupSetBits(example.eventGroup, EventGroupMultiTaskExample::TASK1_BIT);

    vTaskDelay(pdMS_TO_TICKS(2000));
    xEventGroupSetBits(example.eventGroup, EventGroupMultiTaskExample::TASK2_BIT);

    vTaskDelay(pdMS_TO_TICKS(3000));
    xEventGroupSetBits(example.eventGroup, EventGroupMultiTaskExample::TASK3_BIT);

    // Will never reach here
    for (;;);
}





----------------------------------------------

// ISR exmaples 

#define ISR_EVENT_BIT (1 << 0)

void ISR_Handler(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Set the bit in the event group from ISR
    xEventGroupSetBitsFromISR(xEventGroup, ISR_EVENT_BIT, &xHigherPriorityTaskWoken);

    // Perform a context switch if required
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

--------------------------------------------------------------------------------

7.	Memory Management: (Provides functions for dynamic memory allocation and deallocation)
// This is taken care at system level.
--------------------------------------------------------------------------------
//1.	Static Allocation: ()


//2.	Dynamic Memory Allocation  : The choice of heap scheme (heap_1, heap_2, heap_3, etc.) affects how memory is allocated and deallocated.

    //heap_1: Simplistic, no deallocation.
    //heap_2: Best-fit with fragmentation handling, allows deallocation.
    //heap_3: Standard C library allocator, less predictable.
    //heap_4: Combines best features of heap_2 and heap_3.
    //heap_5: Advanced with splitting and merging of memory regions.
//Select the heap implementation:
//FreeRTOS provides different heap management schemes in separate source files: heap_1.c, heap_2.c, heap_3.c, heap_4.c, and heap_5.c.
//Choose the appropriate heap implementation for your project by including the correct heap source file.

-------------------------------------------------------------------------
 //heap_1: No Memory Deallocation

 #include "FreeRTOS.h"
#include "task.h"

// Allocate memory
void* allocateMemory(size_t size) {
    return pvPortMalloc(size);
}

// Example usage
void exampleHeap1Usage() {
    char* buffer = (char*)allocateMemory(100);
    if (buffer != NULL) {
        // Use the allocated memory
    }
    // No call to vPortFree() as memory cannot be freed
}
---------------------------------------------------------------------
//heap_2: Best-fit with fragmentation handling, allows deallocation.

#include "FreeRTOS.h"
#include "task.h"

// Allocate memory
void* allocateMemory(size_t size) {
    return pvPortMalloc(size);
}

// Free allocated memory
void deallocateMemory(void* ptr) {
    vPortFree(ptr);
}

// Example usage
void exampleHeap2Usage() {
    char* buffer = (char*)allocateMemory(100);
    if (buffer != NULL) {
        // Use the allocated memory
        deallocateMemory(buffer);
    }
}

-----------------------------------------------------------------
//heap_3: Standard C library allocator, less predictable.

#include <stdlib.h>

// Allocate memory
void* allocateMemory(size_t size) {
    return malloc(size);
}

// Free allocated memory
void deallocateMemory(void* ptr) {
    free(ptr);
}

// Example usage
void exampleHeap3Usage() {
    char* buffer = (char*)allocateMemory(100);
    if (buffer != NULL) {
        // Use the allocated memory
        deallocateMemory(buffer);
    }
}
------------------------------------------------------------------


//heap_4: Combines best features of heap_2 and heap_3.
#include "FreeRTOS.h"
#include "task.h"

// Allocate memory
void* allocateMemory(size_t size) {
    return pvPortMalloc(size);
}

// Free allocated memory
void deallocateMemory(void* ptr) {
    vPortFree(ptr);
}

// Example usage
void exampleHeap4Usage() {
    char* buffer = (char*)allocateMemory(100);
    if (buffer != NULL) {
        // Use the allocated memory
        deallocateMemory(buffer);
    }
}

---------------------------------------------------------------
//heap_5: Advanced with splitting and merging of memory regions

#include "FreeRTOS.h"
#include "task.h"

// Allocate memory
void* allocateMemory(size_t size) {
    return pvPortMalloc(size);
}

// Free allocated memory
void deallocateMemory(void* ptr) {
    vPortFree(ptr);
}

// Example usage
void exampleHeap5Usage() {
    char* buffer = (char*)allocateMemory(100);
    if (buffer != NULL) {
        // Use the allocated memory
        deallocateMemory(buffer);
    }
}
--------------------------------------------
