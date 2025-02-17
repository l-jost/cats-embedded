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

#pragma once

#include "drivers/buzzer.hpp"
#include "drivers/servo.hpp"
#include "drivers/spi.hpp"
#include "flash/recorder.hpp"

#include "target.h"
#include "util/types.hpp"

/** Sampling Frequencies **/
#define CONTROL_SAMPLING_FREQ   100  // in Hz
#define TELEMETRY_SAMPLING_FREQ 10   // in Hz

/** Device Handles **/

/** State Estimation **/

extern baro_data_t global_baro_sim[NUM_BARO];
extern imu_data_t global_imu_sim[NUM_BARO];

extern osEventFlagsId_t fsm_flag_id;

/** Timers **/
extern cats_timer_t ev_timers[NUM_TIMERS];

/** Recorder Queue **/
extern osMessageQueueId_t rec_queue;
extern osMessageQueueId_t rec_cmd_queue;
extern osMessageQueueId_t event_queue;

extern volatile bool global_usb_detection;
extern volatile bool usb_device_initialized;
extern volatile bool usb_communication_complete;
extern volatile bool simulation_started;
extern volatile bool imu_initialized[NUM_IMU];

/* recorder status is controlled by output functions, do not set manually! */
extern volatile recorder_status_e global_recorder_status;

extern event_action_map_elem_t* event_action_map;

#ifdef CATS_DEBUG
static constexpr const char* code_version = FIRMWARE_VERSION "-dev";
#else
static constexpr const char* code_version = FIRMWARE_VERSION;
#endif
static constexpr const char* board_name = "CATS Vega";
extern char telemetry_code_version[20];
