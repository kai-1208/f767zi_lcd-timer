#include "nhd0216k3z_lcd.h"
#include <stdio.h>
#include <string.h>

#define NHD0216K3Z_LCD_CMD_PREFIX    (0xFEU)
#define NHD0216K3Z_LCD_CMD_CLEAR     (0x51U)
#define NHD0216K3Z_LCD_CMD_SET_CURSOR (0x45U)

#define NHD0216K3Z_LCD_LINE1_ADDRESS (0x00U)
#define NHD0216K3Z_LCD_LINE2_ADDRESS (0x40U)
#define NHD0216K3Z_LCD_BAR_WIDTH     (10U)
#define NHD0216K3Z_LCD_BAR_FILLED_CHAR '#'
#define NHD0216K3Z_LCD_BAR_EMPTY_CHAR  '-'

static bool nhd0216k3z_lcd_tx(Nhd0216k3zLcd *lcd, const uint8_t *data, uint16_t size) {
  if (!lcd || !lcd->uart || !data || size == 0U) return false;
  return HAL_UART_Transmit(lcd->uart, (uint8_t *)data, size, lcd->tx_timeout_ms) == HAL_OK;
}

bool nhd0216k3z_lcd_init(Nhd0216k3zLcd *lcd, UART_HandleTypeDef *uart, uint32_t tx_timeout_ms) {
  if (!lcd || !uart) return false;
  lcd->uart = uart;
  lcd->tx_timeout_ms = tx_timeout_ms;
  HAL_Delay(50U);
  return nhd0216k3z_lcd_clear(lcd);
}

bool nhd0216k3z_lcd_clear(Nhd0216k3zLcd *lcd) {
  const uint8_t cmd[] = { NHD0216K3Z_LCD_CMD_PREFIX, NHD0216K3Z_LCD_CMD_CLEAR };
  return nhd0216k3z_lcd_tx(lcd, cmd, sizeof(cmd));
}

bool nhd0216k3z_lcd_set_cursor(Nhd0216k3zLcd *lcd, uint8_t address) {
  const uint8_t cmd[] = { NHD0216K3Z_LCD_CMD_PREFIX, NHD0216K3Z_LCD_CMD_SET_CURSOR, address };
  return nhd0216k3z_lcd_tx(lcd, cmd, sizeof(cmd));
}

bool nhd0216k3z_lcd_write_line(Nhd0216k3zLcd *lcd, uint8_t row, const char text[NHD0216K3Z_LCD_COLUMNS + 1U]) {
  const uint8_t address = (row == 0U) ? NHD0216K3Z_LCD_LINE1_ADDRESS : NHD0216K3Z_LCD_LINE2_ADDRESS;
  if (row >= NHD0216K3Z_LCD_ROWS || !text) return false;
  if (!nhd0216k3z_lcd_set_cursor(lcd, address)) return false;
  return nhd0216k3z_lcd_tx(lcd, (const uint8_t *)text, NHD0216K3Z_LCD_COLUMNS);
}

bool nhd0216k3z_lcd_show_timer(Nhd0216k3zLcd *lcd, int32_t remaining_seconds, int32_t total_seconds) {
  char line1[NHD0216K3Z_LCD_COLUMNS + 1U];
  char line2[NHD0216K3Z_LCD_COLUMNS + 1U];

  if (!lcd || total_seconds <= 0) return false;
  if (remaining_seconds < 0) remaining_seconds = 0;
  if (remaining_seconds > total_seconds) remaining_seconds = total_seconds;

  const int32_t minutes = remaining_seconds / 60;
  const int32_t seconds = remaining_seconds % 60;
  const uint32_t filled = (uint32_t)(((total_seconds - remaining_seconds) * (int32_t)NHD0216K3Z_LCD_BAR_WIDTH + total_seconds / 2) / total_seconds);

  if (remaining_seconds == 0) {
    snprintf(line1, sizeof(line1), "TIME UP!   %02ld:%02ld", (long)minutes, (long)seconds);
  } else {
    snprintf(line1, sizeof(line1), "REMAIN:    %02ld:%02ld", (long)minutes, (long)seconds);
  }
  line1[NHD0216K3Z_LCD_COLUMNS] = '\0';

  line2[0] = '[';
  for (uint32_t i = 0U; i < NHD0216K3Z_LCD_BAR_WIDTH; ++i) {
    line2[i + 1U] = (i < filled) ? NHD0216K3Z_LCD_BAR_FILLED_CHAR : NHD0216K3Z_LCD_BAR_EMPTY_CHAR;
  }
  line2[11] = ']';
  snprintf(&line2[12], sizeof(line2) - 12U, "%3lu%%", (unsigned long)((filled * 100U) / NHD0216K3Z_LCD_BAR_WIDTH));
  line2[NHD0216K3Z_LCD_COLUMNS] = '\0';

  if (!nhd0216k3z_lcd_write_line(lcd, 0U, line1)) return false;
  return nhd0216k3z_lcd_write_line(lcd, 1U, line2);
}