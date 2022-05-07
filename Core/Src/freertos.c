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
uint8_t current_can_data[8];

// <----- Attribute definitions ----->

const osThreadAttr_t kernelLEDTask_attributes = {
  .name = "kernelLEDTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

const osThreadAttr_t readCANTask_attributes = {
  .name = "readCANTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void kernelLEDTask (void *argument);

/* USER CODE END FunctionPrototypes */

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Create the thread(s) */

  /* USER CODE BEGIN RTOS_THREADS */

  kernelLEDTaskHandle = osThreadNew(kernelLEDTask, NULL, &kernelLEDTask_attributes);

  /* USER CODE END RTOS_THREADS */
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
    while (1) {
        // wait for CAN RX ISR to set thread flags
        osThreadFlagsWait(CAN_READY, osFlagsWaitAll, osWaitForever);

        // there are multiple CAN IDs being passed through the filter, pull out the current message
        HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &can_rx_header, current_can_data);
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
    }
}

/* USER CODE END Application */

