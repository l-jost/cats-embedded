/// CATS Flight Software
/// Copyright (C) 2022 Control and Telemetry Systems
///
/// This program is free software: you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published by
/// the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "Gps.hpp"
#include "Main.hpp"

#include <stdio.h>

uint8_t c1;
uint32_t lr1;

static uint8_t ublox_request_115200_baud[] = {
    0xb5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xd0, 0x08, 0x00, 0x00, 0x00, 0xc2, 0x01, 0x00, 0x07,
    0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc4, 0x96, 0xb5, 0x62, 0x06, 0x00, 0x01, 0x00, 0x01, 0x08, 0x22};
static uint8_t ublox_request_5Hz[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xC8,
                                      0x00, 0x01, 0x00, 0x01, 0x00, 0xDE, 0x6A};

extern UART_HandleTypeDef huart1;

void gpsSetup() {
  uint8_t command[20];

  // Check hardware version
  if (HAL_GPIO_ReadPin(HARDWARE_ID_GPIO_Port, HARDWARE_ID_Pin)) {
    // Flight computer
    HAL_UART_Transmit(&huart1, ublox_request_115200_baud, sizeof(ublox_request_115200_baud), 100);
  } else {
    // Groundstation
    /* Request UART speed of 115200 */
    snprintf((char *)command, 15, "$PCAS01,5*19\r\n");
    HAL_UART_Transmit(&huart1, command, 14, 100);
  }

  HAL_Delay(200);

  /* Change bus speed to 115200 */
  USART1->CR1 &= ~(USART_CR1_UE);
  USART1->BRR = 417;  // Set baud to 115200
  USART1->CR1 |= USART_CR1_UE;

  HAL_Delay(200);

  // Check hardware version
  if (HAL_GPIO_ReadPin(HARDWARE_ID_GPIO_Port, HARDWARE_ID_Pin)) {
    // Flight computer
    /* Request 5 Hz mode */
    HAL_UART_Transmit(&huart1, ublox_request_5Hz, sizeof(ublox_request_5Hz), 100);
    /* Request airbourne, not working yet */
    // HAL_UART_Transmit(&huart1, ublox_request_airbourne,
    // sizeof(ublox_request_airbourne), 100);
  } else {
    // Groundstation
    /* Request 10Hz update rate */
    snprintf((char *)command, 17, "$PCAS02,100*1E\r\n");
    HAL_UART_Transmit(&huart1, command, 16, 100);
  }
}
