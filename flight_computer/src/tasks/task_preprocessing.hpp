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

#include "task.hpp"

#include "task_sensor_read.hpp"
#include "util/error_handler.hpp"
#include "util/log.h"
#include "util/types.hpp"

namespace task {

class Preprocessing final : public Task<Preprocessing, 512> {
 public:
  explicit Preprocessing(const SensorRead& task_sensor_read) : m_task_sensor_read(task_sensor_read) {}
  [[nodiscard]] state_estimation_input_t GetEstimationInput() const noexcept;
  [[nodiscard]] SI_data_t GetSIData() const noexcept;

 private:
  [[noreturn]] void Run() noexcept override;

  void AvgToSi() noexcept;
  void MedianFilter() noexcept;
  void TransformData() noexcept;
  void CheckSensors() noexcept;
  cats_error_e CheckSensorBounds(uint8_t index, const sens_info_t* sens_info) noexcept;
  cats_error_e CheckSensorFreezing(uint8_t index, const sens_info_t* sens_info) noexcept;

  const SensorRead& m_task_sensor_read;

  imu_data_t m_imu_data[NUM_IMU]{};
  baro_data_t m_baro_data[NUM_BARO]{};

  SI_data_t m_si_data = {};
  SI_data_t m_si_data_old = {.acc = {.x = GRAVITY, .y = 0.0F, .z = 0.0F}, .pressure = P_INITIAL};

#ifdef USE_MEDIAN_FILTER
  median_filter_t m_filter_data = {};
#endif
  sensor_elimination_t m_sensor_elimination = {};

  /* Calibration Data including the gyro calibration as the first three values and then the angle and axis are for
   * the linear acceleration calibration */
  calibration_data_t m_calibration = {.gyro_calib = {.x = 0, .y = 0, .z = 0}, .angle = 1, .axis = 2};
  state_estimation_input_t m_state_est_input = {.acceleration_z = 0.0F, .height_AGL = 0.0F};
  float32_t m_height_0 = 0.0F;

  /* Variable to keep track of the calibration health */
  int32_t faulty_calibration_counter = 0;
  /* with a sampling frequency of 100 Hz this yields 1s */
  static constexpr int32_t kMaxFaultyCalib = 100;

  /* Gyro Calib tag */
  bool m_gyro_calibrated = false;
};

}  // namespace task
