#ifndef LCD_TIMER_H
#define LCD_TIMER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_TIMER_TOTAL_SECONDS (180)

/* freertos.c から呼び出す関数 */
void LcdTimer_Init(void);
bool LcdTimer_Submit(int32_t remaining_seconds);
void StartLcdTimerTask(void *argument);

#ifdef __cplusplus
}
#endif

#endif /* LCD_TIMER_H */