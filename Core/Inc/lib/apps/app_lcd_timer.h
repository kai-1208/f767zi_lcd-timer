#ifndef APP_LCD_TIMER_H
#define APP_LCD_TIMER_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define APP_LCD_TIMER_TOTAL_SECONDS (180)
#define APP_LCD_TIMER_QUEUE_LENGTH (1U)

void app_lcd_timer_init (void);

bool app_lcd_timer_submit (int32_t remaining_seconds);

void StartLcdTimerTask (void *argument);

#ifdef __cplusplus
}
#endif

#endif /* APP_LCD_TIMER_H */