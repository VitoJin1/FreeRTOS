#include "led.h"
#include "delay.h"
#include "timer.h"
#include "sys.h"
#include "usart.h"

#include "FreeRTOS.h"
#include "task.h"


/************************************************
//STM32F103ZE核心板
 PWM输出实验  

************************************************/

#define START_TASK_PRIO 1
#define START_STK_SIZE  256
TaskHandle_t StartTask_Handler;
void start_task (void * pvParameters);

#define INTERRUPT_TASK_PRIO 2
#define INTERRUPT_STK_SIZE 256
TaskHandle_t INTERRUPTTask_Handler;
void interrupt_task(void *p_arg);

#define LEDGREEN_TASK_PRIO 3
#define LEDGREEN_STK_SIZE 50
TaskHandle_t LEDGREENTASK_Handler;
void LEDGreen_task(void *pvParameters);



int main(void)
{		
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    delay_init();
    LED_Init();	
    uart_init(115200);
    TIM3_Int_Init(10000-1,7200-1);//1hz
    TIM2_Int_Init(10000-1,7200-1);//1hz
    
    xTaskCreate((TaskFunction_t)start_task  ,
                (const char*)"start_task"   ,
                (uint16_t)START_STK_SIZE    ,
                (void*)NULL                 ,
                (UBaseType_t)START_TASK_PRIO,
                (TaskHandle_t *)&StartTask_Handler);
    vTaskStartScheduler();
}
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();
    xTaskCreate((TaskFunction_t)interrupt_task,
                 (const char*)"interrupt_task",
                 (uint16_t)INTERRUPT_STK_SIZE,
                 (void*)NULL,
                 (UBaseType_t)INTERRUPT_TASK_PRIO,
                 (TaskHandle_t *)&INTERRUPTTask_Handler);
    vTaskDelete(StartTask_Handler);
    taskEXIT_CRITICAL();
}


void interrupt_task(void *pvParameters){
    static u32 total_num=0;
    while(1)
    {
        total_num+=1;
        if(total_num==5)
        {
            printf("shut down serial port\r\n");
            portDISABLE_INTERRUPTS();
            delay_xms(5000);
            printf("open serial port\r\n");
            portENABLE_INTERRUPTS();
        }
        LED_G=!LED_G;
        vTaskDelay(1000);
    }
}

void LEDBlue_task(void *pvParameters){
    while(1){
        LED_B=~LED_B;
        vTaskDelay(500);
    }
}

void LEDGreen_task(void *pvParameters){
    while(1){
        LED_G=0;
        vTaskDelay(200);
        LED_G=1;
        vTaskDelay(800);
    }
}
