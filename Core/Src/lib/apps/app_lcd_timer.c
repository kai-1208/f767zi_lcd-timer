#include "lib/apps/app_lcd_timer.h"

#include "FreeRTOS.h"
#include "lib/nhd0216k3z_lcd.h"
#include "queue.h"
#include "task.h"
#include "usart.h"

#define APP_LCD_TIMER_UART_TIMEOUT_MS (100U)

static QueueHandle_t remaining_seconds_queue_;
static Nhd0216k3zLcd lcd_;

void app_lcd_timer_init (void) {
  if (remaining_seconds_queue_ == NULL) {
    remaining_seconds_queue_ =
        xQueueCreate (APP_LCD_TIMER_QUEUE_LENGTH, sizeof (int32_t));
  }
}

bool app_lcd_timer_submit (int32_t remaining_seconds) {
  if (remaining_seconds_queue_ == NULL) {
    return false;
  }

  /*
   * Queue長は1にして、古い残り時間を最新値で上書きする。
   * 1Hzのスナップショット値なので、古い値を順番に描画する必要はない。
   */
  return xQueueOverwrite (remaining_seconds_queue_, &remaining_seconds) == pdPASS;
}

void StartLcdTimerTask (void *argument) {
  (void)argument;

  nhd0216k3z_lcd_init (&lcd_, &huart5, APP_LCD_TIMER_UART_TIMEOUT_MS);

  /*
   * micro-ROSから最初の値を受け取るまで、起動時表示として180秒を表示する。
   */
  (void)nhd0216k3z_lcd_show_timer (&lcd_,
                                   APP_LCD_TIMER_TOTAL_SECONDS,
                                   APP_LCD_TIMER_TOTAL_SECONDS);

  for (;;) {
    int32_t remaining_seconds = APP_LCD_TIMER_TOTAL_SECONDS;

    if (xQueueReceive (remaining_seconds_queue_,
                       &remaining_seconds,
                       portMAX_DELAY) == pdPASS) {
      (void)nhd0216k3z_lcd_show_timer (&lcd_,
                                       remaining_seconds,
                                       APP_LCD_TIMER_TOTAL_SECONDS);
    }
  }
}