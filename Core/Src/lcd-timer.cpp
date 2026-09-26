#include "lcd-timer.h"
#include "nhd0216k3z_lcd.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "usart.h"

namespace {
constexpr uint32_t UART_TIMEOUT_MS = 100;
constexpr UBaseType_t QUEUE_LENGTH = 1;
}

/**
 * @brief C++によるタイマー表示コントローラークラス
 */
class LcdTimerController {
public:
  LcdTimerController() : queue_(nullptr) {}

  void init() {
    if (queue_ == nullptr) {
      queue_ = xQueueCreate(QUEUE_LENGTH, sizeof(int32_t));
    }
  }

  bool submit(int32_t remaining_seconds) {
    if (queue_ == nullptr) return false;
    return xQueueOverwrite(queue_, &remaining_seconds) == pdPASS;
  }

  [[noreturn]] void run() {
    /* UserLibsのC言語ドライバを初期化 */
    nhd0216k3z_lcd_init(&lcd_, &huart5, UART_TIMEOUT_MS);
    nhd0216k3z_lcd_show_timer(&lcd_, LCD_TIMER_TOTAL_SECONDS, LCD_TIMER_TOTAL_SECONDS);

    for (;;) {
      int32_t remaining_seconds = LCD_TIMER_TOTAL_SECONDS;
      if (xQueueReceive(queue_, &remaining_seconds, portMAX_DELAY) == pdPASS) {
        nhd0216k3z_lcd_show_timer(&lcd_, remaining_seconds, LCD_TIMER_TOTAL_SECONDS);
      }
    }
  }

private:
  QueueHandle_t queue_;
  Nhd0216k3zLcd lcd_{};
};

/* シングルトンインスタンス */
static LcdTimerController s_lcd_controller;

/* --- C言語向け外部公開インターフェース (extern "C") --- */
extern "C" {

void LcdTimer_Init(void) {
  s_lcd_controller.init();
}

bool LcdTimer_Submit(int32_t remaining_seconds) {
  return s_lcd_controller.submit(remaining_seconds);
}

void StartLcdTimerTask(void *argument) {
  (void)argument;
  s_lcd_controller.run();
}

}