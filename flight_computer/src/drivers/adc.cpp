/*
 * CATS Flight Software
 * Copyright (C) 2023 Control and Telemetry Systems
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "drivers/adc.hpp"
#include "target.h"

static uint32_t adc_value[ADC_NUM_CHANNELS];

void adc_init() { HAL_ADC_Start_DMA(&ADC_HANDLE, adc_value, ADC_NUM_CHANNELS); }

uint32_t adc_get(adc_channels_e channel) {
  // If data of the first and last adc channel is frozen, reset the hardware
  if ((adc_value[0] | adc_value[ADC_BATTERY]) == 0) {
    HAL_ADC_Stop_DMA(&ADC_HANDLE);
    HAL_ADC_Start_DMA(&ADC_HANDLE, adc_value, ADC_NUM_CHANNELS);
  }

  if ((channel < 0) || (channel > (ADC_NUM_CHANNELS - 1))) return 0;

  return adc_value[channel];
}
