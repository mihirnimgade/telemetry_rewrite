/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "can.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define KERNEL_LED_DELAY 200

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

osThreadId_t kernelLEDTaskHandle;
osThreadId_t readCANTaskHandle;

// stores CAN data of most recent retrieved CAN message
// uint8_t current_can_data[8];

// <----- Attribute definitions ----->

const osThreadAttr_t kernelLEDTask_attributes = {
  .name = "kernelLEDTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

const osThreadAttr_t readCANTask_attributes = {
  .name = "readCANTask",
  .stack_size = 128 * 8,
  .priority = (osPriority_t) osPriorityNormal,
};

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void kernelLEDTask (void *argument);
void readCANTask(void *argument);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */

  // kernelLEDTaskHandle = osThreadNew(kernelLEDTask, NULL, &kernelLEDTask_attributes);
  readCANTaskHandle = osThreadNew(readCANTask, NULL, &readCANTask_attributes);

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

__NO_RETURN void kernelLEDTask (void *argument) {
  osKernelState_t kernel_status;

  while (1) {
    kernel_status = osKernelGetState();

    if (kernel_status == osKernelRunning) {
      HAL_GPIO_TogglePin(KERNEL_LED_GPIO_Port, KERNEL_LED_Pin);
    }

    osDelay(KERNEL_LED_DELAY);
  }
}

__NO_RETURN void readCANTask(void *argument) {
  static HAL_StatusTypeDef rx_status;

    while (1) {
      // wait for CAN RX ISR to set thread flags
      osThreadFlagsWait(CAN_READY, osFlagsWaitAll, osWaitForever);

      if (HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) != 0) {
        // there are multiple CAN IDs being passed through the filter, pull out the current message
        rx_status = HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &can_rx_header, current_can_data);
        HAL_GPIO_TogglePin(KERNEL_LED_GPIO_Port, KERNEL_LED_Pin);
      }
        //
        // // motor ID
        // if (can_rx_header.StdId == 0x50B) {
        //     can_id = MOTOR_ID;
        // }
        // // battery SOC ID
        // else if (can_rx_header.StdId == 0x626) {
        //     can_id = BATTERY_ID;
        // } else {
        //     // Major error. A CAN ID was received even though the filter only accepts 0x50B and 0x626
        //     can_id = INVALID;
        // }
        //
        // osEventFlagsSet(canIdEventFlagsHandle, can_id);
        // HAL_StatusTypeDef status = HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
        // assert_param(status == HAL_OK);

        osDelay(100);
    }
}

/* USER CODE END Application */

