/*
 * task_imu_read.c
 *
 *  Created on: Nov 3, 2019
 *      Author: Jonas
 */

#include "cmsis_os.h"
#include "tasks/task_imu_read.h"
#include "drivers/icm20601.h"
#include "util/recorder.h"
#include "config/globals.h"

static void read_imu(int16_t gyroscope_data[3], int16_t acceleration[3],
                     int16_t *temperature, int32_t id);

//#define CALIBRATE_ACCEL

/**
 * @brief Function implementing the task_baro_read thread.
 * @param argument: Not used
 * @retval None
 */
void task_imu_read(void *argument) {
  uint32_t tick_count, tick_update;

  /* initialize data variables */
  int16_t gyroscope_data[3] = {0}; /* 0 = x, 1 = y, 2 = z */
  int16_t acceleration[3] = {0};   /* 0 = x, 1 = y, 2 = z */
  int16_t temperature;

  int32_t imu_idx = 0;

  /* initialize queue message */
  // imu_data_t queue_data = { 0 };

  /* Infinite loop */
  tick_count = osKernelGetTickCount();
  tick_update = osKernelGetTickFreq() / IMU20601_SAMPLING_FREQ;

  for (;;) {
    tick_count += tick_update;
    read_imu(gyroscope_data, acceleration, &temperature, imu_idx);

    /* Debugging */

    //		UsbPrint("IMU %ld: RAW Gx: %ld, Gy:%ld, Gz:%ld; Ax: %ld, Ay:%ld,
    // Az:%ld, T:%ld; \n", 				imu_idx,
    // gyroscope_data[0], gyroscope_data[1], gyroscope_data[2], acceleration[0],
    // acceleration[1], acceleration[2], temperature);

    global_imu[imu_idx].acc_x = acceleration[0];
    global_imu[imu_idx].acc_y = acceleration[1];
    global_imu[imu_idx].acc_z = acceleration[2];
    global_imu[imu_idx].gyro_x = gyroscope_data[0];
    global_imu[imu_idx].gyro_y = gyroscope_data[1];
    global_imu[imu_idx].gyro_z = gyroscope_data[2];
    /* TODO: maybe replace with this */
    //    memcpy(&global_imu[imu_idx].acc_x, &acceleration, 3*sizeof(int16_t));
    //    memcpy(&global_imu[imu_idx].gyro_x, &gyroscope_data,
    //    3*sizeof(int16_t));
    global_imu[imu_idx].ts = tick_count;

    /* TODO: how fast are we recording here, 300x or 100x per second? */
    record(IMU0 + imu_idx, &(global_imu[imu_idx]));

    imu_idx = (imu_idx + 1) % 3;
    osDelayUntil(tick_count);
  }
}

void read_imu(int16_t gyroscope_data[3], int16_t acceleration[3],
              int16_t *temperature, int32_t id) {
  switch (id) {
    case 0:
      icm20601_read_accel_raw(&ICM1, acceleration);
      icm20601_read_gyro_raw(&ICM1, gyroscope_data);
      icm20601_read_temp_raw(&ICM1, temperature);
      break;
    case 1:
      icm20601_read_accel_raw(&ICM2, acceleration);
      icm20601_read_gyro_raw(&ICM2, gyroscope_data);
      icm20601_read_temp_raw(&ICM2, temperature);
      break;
    case 2:
      icm20601_read_accel_raw(&ICM3, acceleration);
      icm20601_read_gyro_raw(&ICM3, gyroscope_data);
      icm20601_read_temp_raw(&ICM3, temperature);
      break;
    default:
      break;
  }
}
