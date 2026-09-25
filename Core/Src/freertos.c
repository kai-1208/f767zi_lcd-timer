/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "lib/apps/app_lcd_timer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for LCDTimerTask */
osThreadId_t LCDTimerTaskHandle;
const osThreadAttr_t LCDTimerTask_attributes = {
  .name = "LCDTimerTask",
  .stack_size = 500 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/* テスト用: 1秒ごとに180からカウントダウンしてQueueに送るタスク */
static void DummyTimerSenderTask(void *argument)
{
  (void)argument;
  int32_t seconds = 180;

  for (;;)
  {
    /* Queueへ現在の秒数を送信（最新値で上書き） */
    app_lcd_timer_submit(seconds);

    /* 1秒（1000ミリ秒）待機 */
    osDelay(1000);

    /* 0秒まで減らし、0秒になったら180秒にリセットしてループ */
    if (seconds > 0) {
      seconds--;
    } else {
      seconds = 180; // テスト用に繰り返す場合
    }
  }
}
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartLCDTimerTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  app_lcd_timer_init();
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

  /* creation of LCDTimerTask */
  LCDTimerTaskHandle = osThreadNew(StartLCDTimerTask, NULL, &LCDTimerTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  const osThreadAttr_t dummyTask_attributes = {
    .name = "DummyTimer",
    .stack_size = 256 * 4,
    .priority = (osPriority_t) osPriorityNormal,
  };
  osThreadNew(DummyTimerSenderTask, NULL, &dummyTask_attributes);
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

/* USER CODE BEGIN Header_StartLCDTimerTask */
/**
* @brief Function implementing the LCDTimerTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLCDTimerTask */
void StartLCDTimerTask(void *argument)
{
  /* USER CODE BEGIN StartLCDTimerTask */
  /* LCDタスク本体を起動 */
  StartLcdTimerTask(argument);
  /* USER CODE END StartLCDTimerTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

