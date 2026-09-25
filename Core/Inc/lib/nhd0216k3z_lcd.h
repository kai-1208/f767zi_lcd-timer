#ifndef NHD0216K3Z_LCD_H
#define NHD0216K3Z_LCD_H

#include "main.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NHD0216K3Z_LCD_COLUMNS (16U)
#define NHD0216K3Z_LCD_ROWS (2U)

#define NHD0216K3Z_LCD_CMD_PREFIX (0xFEU)
#define NHD0216K3Z_LCD_CMD_CLEAR (0x51U)
#define NHD0216K3Z_LCD_CMD_SET_CURSOR (0x45U)

typedef struct {
  UART_HandleTypeDef *uart;
  uint32_t tx_timeout_ms;
} Nhd0216k3zLcd;

bool nhd0216k3z_lcd_init (Nhd0216k3zLcd *lcd,
                          UART_HandleTypeDef *uart,
                          uint32_t tx_timeout_ms);

bool nhd0216k3z_lcd_clear (Nhd0216k3zLcd *lcd);

bool nhd0216k3z_lcd_set_cursor (Nhd0216k3zLcd *lcd, uint8_t address);

bool nhd0216k3z_lcd_write (Nhd0216k3zLcd *lcd, const char *text);

bool nhd0216k3z_lcd_write_line (Nhd0216k3zLcd *lcd,
                                uint8_t row,
                                const char text[NHD0216K3Z_LCD_COLUMNS + 1U]);

bool nhd0216k3z_lcd_show_timer (Nhd0216k3zLcd *lcd,
                                int32_t remaining_seconds,
                                int32_t total_seconds);

#ifdef __cplusplus
}
#endif

#endif /* NHD0216K3Z_LCD_H */