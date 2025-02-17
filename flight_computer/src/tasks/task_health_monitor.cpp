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

#include "tasks/task_health_monitor.hpp"
#include "cmsis_os.h"
#include "config/cats_config.hpp"
#include "config/globals.hpp"
#include "drivers/adc.hpp"
#include "util/actions.hpp"
#include "util/battery.hpp"
#include "util/log.h"
#include "util/task_util.hpp"

#include "tasks/task_cdc.hpp"
#include "tasks/task_cli.hpp"
#include "tasks/task_simulator.hpp"
#include "tasks/task_usb_device.hpp"

#include "target.h"

/** Private Constants **/

/** Private Function Declarations **/

namespace task {

static void init_usb();

static bool cli_task_started = false;
/** Exported Function Definitions **/

[[noreturn]] void HealthMonitor::Run() noexcept {
  // an increase of 1 on the timer means 10 ms
  uint32_t ready_timer = 0;
  uint32_t voltage_logging_timer = 0;
  battery_level_e old_level = BATTERY_OK;

  m_task_buzzer.Beep(Buzzer::BeepCode::kBootup);

  uint32_t tick_count = osKernelGetTickCount();
  constexpr uint32_t tick_update = sysGetTickFreq() / CONTROL_SAMPLING_FREQ;

  while (true) {
    /* Uncomment the code below to enable stack usage monitoring:
     *
     * static uint32_t initial_stack_sz = 0xFFFFFFFF;
     * uint32_t curr_stack_sz = osThreadGetStackSpace(osThreadGetId());
     * if ((initial_stack_sz == 0xFFFFFFFF) || (curr_stack_sz < initial_stack_sz)) {
     *  log_raw("[%lu] [Health Monitor] Remaining stack sz: %lu B", osKernelGetTickCount(), curr_stack_sz);
     *  initial_stack_sz = curr_stack_sz;
     * }
     */

    /* Get new FSM enum */
    bool fsm_updated = GetNewFsmEnum();

    if (global_usb_detection == true && cli_task_started == false) {
      cli_task_started = true;
      Cli::Start();
    }

    if (HAL_GPIO_ReadPin(USB_DET_GPIO_Port, USB_DET_Pin) && usb_communication_complete == false) {
      init_usb();
    }

    if (usb_device_initialized == false) {
      if (HAL_GPIO_ReadPin(USB_DET_GPIO_Port, USB_DET_Pin)) {
        usb_device_initialized = true;
      }
    }
    // Check battery level
    battery_level_e level = battery_level();
    bool level_changed = (old_level != level);

    if ((level == BATTERY_CRIT) && level_changed) {
      clear_error(CATS_ERR_BAT_LOW);
      add_error(CATS_ERR_BAT_CRITICAL);
    } else if ((level == BATTERY_LOW) && level_changed) {
      clear_error(CATS_ERR_BAT_CRITICAL);
      add_error(CATS_ERR_BAT_LOW);
    } else if (level_changed) {
      clear_error(CATS_ERR_BAT_LOW);
      clear_error(CATS_ERR_BAT_CRITICAL);
    }

    /* Record voltage information with 1Hz. */
    if (++voltage_logging_timer >= 100) {
      voltage_logging_timer = 0;
      uint16_t voltage = battery_voltage_short();
      record(osKernelGetTickCount(), VOLTAGE_INFO, &voltage);
    }

    old_level = battery_level();

    /* Check Pyro settings */
    if ((m_check_pyro_1 || m_check_pyro_2) && (m_fsm_enum < THRUSTING)) {
      bool error_encountered = false;
      if (m_check_pyro_1) {
        if (adc_get(ADC_PYRO1) < 500) {
          add_error(CATS_ERR_NO_PYRO);
          error_encountered = true;
        }
      }
      if (m_check_pyro_2) {
        if (adc_get(ADC_PYRO2) < 500) {
          add_error(CATS_ERR_NO_PYRO);
          error_encountered = true;
        }
      }

      if (!error_encountered) {
        clear_error(CATS_ERR_NO_PYRO);
      }
    }

    // Beep out ready buzzer
    if ((m_fsm_enum == READY) && (ready_timer >= 500)) {
      m_task_buzzer.Beep(Buzzer::BeepCode::kReady);
      ready_timer = 0;
    } else if (m_fsm_enum == READY) {
      ++ready_timer;
    }

    // Beep out transitions from calibrating to ready and back
    if (m_fsm_enum == READY && fsm_updated) {
      m_task_buzzer.Beep(Buzzer::BeepCode::kChangedReady);
    }

    tick_count += tick_update;
    osDelayUntil(tick_count);
  }
}

void HealthMonitor::DeterminePyroCheck() {
  // Loop over all events
  for (int ev_idx = 0; ev_idx < NUM_EVENTS; ev_idx++) {
    uint16_t nr_actions = cc_get_num_actions((cats_event_e)ev_idx);
    // If an action is mapped to the event
    if (nr_actions > 0) {
      // Loop over all actions
      for (uint16_t act_idx = 0; act_idx < nr_actions; act_idx++) {
        config_action_t action{};
        if (cc_get_action((cats_event_e)ev_idx, act_idx, &action) == true) {
          switch (action.action_idx) {
            case ACT_HIGH_CURRENT_ONE:
              m_check_pyro_1 = true;
              break;
            case ACT_HIGH_CURRENT_TWO:
              m_check_pyro_2 = true;
              break;
            default:
              break;
          }
        }
      }
    }
  }
}

static void init_usb() {
  MX_USB_OTG_FS_PCD_Init();
  UsbDevice::Start();
  Cdc::Start();

  usb_communication_complete = true;
}

}  // namespace task
