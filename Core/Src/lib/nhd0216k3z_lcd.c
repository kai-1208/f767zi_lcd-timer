#include "lib/nhd0216k3z_lcd.h"

#include <stdio.h>
#include <string.h>

#define NHD0216K3Z_LCD_LINE1_ADDRESS (0x00U)
#define NHD0216K3Z_LCD_LINE2_ADDRESS (0x40U)

#define NHD0216K3Z_LCD_BAR_WIDTH (10U)

/*
 * NHD-0216K3Zの標準ASCII文字として確実に表示できる'#'を使用する。
 * Unicodeの'█'はLCDの内蔵フォントに存在しないため使用しない。
 */
#define NHD0216K3Z_LCD_BAR_FILLED_CHAR '#'
#define NHD0216K3Z_LCD_BAR_EMPTY_CHAR '-'

static bool nhd0216k3z_lcd_tx (Nhd0216k3zLcd *lcd,
                               const uint8_t *data,
                               uint16_t size) {
  if (lcd == NULL || lcd->uart == NULL || data == NULL || size == 0U) {
    return false;
  }

  return HAL_UART_Transmit (lcd->uart,
                            (uint8_t *)data,
                            size,
                            lcd->tx_timeout_ms) == HAL_OK;
}

static bool nhd0216k3z_lcd_tx_byte (Nhd0216k3zLcd *lcd, uint8_t data) {
  return nhd0216k3z_lcd_tx (lcd, &data, 1U);
}

bool nhd0216k3z_lcd_init (Nhd0216k3zLcd *lcd,
                          UART_HandleTypeDef *uart,
                          uint32_t tx_timeout_ms) {
  if (lcd == NULL || uart == NULL) {
    return false;
  }

  lcd->uart = uart;
  lcd->tx_timeout_ms = tx_timeout_ms;

  /*
   * 電源投入直後のLCD内部初期化時間を確保する。
   * この関数はLCD専用タスクから呼ぶため、ここで待っても
   * micro-ROS executorはブロックしない。
   */
  HAL_Delay (50U);

  return nhd0216k3z_lcd_clear (lcd);
}

bool nhd0216k3z_lcd_clear (Nhd0216k3zLcd *lcd) {
  const uint8_t command[] = {
      NHD0216K3Z_LCD_CMD_PREFIX,
      NHD0216K3Z_LCD_CMD_CLEAR,
  };

  return nhd0216k3z_lcd_tx (lcd, command, sizeof (command));
}

bool nhd0216k3z_lcd_set_cursor (Nhd0216k3zLcd *lcd, uint8_t address) {
  const uint8_t command[] = {
      NHD0216K3Z_LCD_CMD_PREFIX,
      NHD0216K3Z_LCD_CMD_SET_CURSOR,
      address,
  };

  return nhd0216k3z_lcd_tx (lcd, command, sizeof (command));
}

bool nhd0216k3z_lcd_write (Nhd0216k3zLcd *lcd, const char *text) {
  if (text == NULL) {
    return false;
  }

  return nhd0216k3z_lcd_tx (lcd, (const uint8_t *)text, (uint16_t)strlen (text));
}

bool nhd0216k3z_lcd_write_line (Nhd0216k3zLcd *lcd,
                                uint8_t row,
                                const char text[NHD0216K3Z_LCD_COLUMNS + 1U]) {
  const uint8_t address =
      row == 0U ? NHD0216K3Z_LCD_LINE1_ADDRESS : NHD0216K3Z_LCD_LINE2_ADDRESS;

  if (row >= NHD0216K3Z_LCD_ROWS || text == NULL) {
    return false;
  }

  if (!nhd0216k3z_lcd_set_cursor (lcd, address)) {
    return false;
  }

  return nhd0216k3z_lcd_tx (lcd,
                             (const uint8_t *)text,
                             NHD0216K3Z_LCD_COLUMNS);
}

bool nhd0216k3z_lcd_show_timer (Nhd0216k3zLcd *lcd,
                                int32_t remaining_seconds,
                                int32_t total_seconds) {
  char line1[NHD0216K3Z_LCD_COLUMNS + 1U];
  char line2[NHD0216K3Z_LCD_COLUMNS + 1U];

  if (lcd == NULL || total_seconds <= 0) {
    return false;
  }

  if (remaining_seconds < 0) {
    remaining_seconds = 0;
  }

  if (remaining_seconds > total_seconds) {
    remaining_seconds = total_seconds;
  }

  const int32_t minutes = remaining_seconds / 60;
  const int32_t seconds = remaining_seconds % 60;

  /*
   * remaining=total のとき0個、remaining=0のとき10個。
   * 丸め誤差が出ないよう整数演算で計算する。
   */
  const uint32_t filled =
      (uint32_t)(((total_seconds - remaining_seconds) *
                      (int32_t)NHD0216K3Z_LCD_BAR_WIDTH +
                  total_seconds / 2) /
                 total_seconds);

  (void)snprintf (line1,
                  sizeof (line1),
                  remaining_seconds == 0 ? "TIME UP! %02ld:%02ld"
                                         : "REMAIN:%5ld:%02ld",
                  (long)minutes,
                  (long)seconds);

  /*
   * line1は必ず16文字に揃える。
   * "REMAIN:    03:00" = 16文字
   * "TIME UP!   00:00" = 16文字
   */
  if (remaining_seconds == 0) {
    (void)snprintf (line1,
                    sizeof (line1),
                    "TIME UP!   %02ld:%02ld",
                    (long)minutes,
                    (long)seconds);
  }

  line1[NHD0216K3Z_LCD_COLUMNS] = '\0';

  line2[0] = '[';
  for (uint32_t index = 0U; index < NHD0216K3Z_LCD_BAR_WIDTH; ++index) {
    line2[index + 1U] =
        index < filled ? NHD0216K3Z_LCD_BAR_FILLED_CHAR
                        : NHD0216K3Z_LCD_BAR_EMPTY_CHAR;
  }
  line2[11] = ']';
  (void)snprintf (&line2[12], sizeof (line2) - 12U, "%3lu%%", (unsigned long)((filled * 100U) / NHD0216K3Z_LCD_BAR_WIDTH));
  line2[NHD0216K3Z_LCD_COLUMNS] = '\0';

  if (!nhd0216k3z_lcd_write_line (lcd, 0U, line1)) {
    return false;
  }

  return nhd0216k3z_lcd_write_line (lcd, 1U, line2);
}