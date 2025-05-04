#include <TFT_eSPI.h>
#include <SPI.h>
#include "ArtificialHorizon.h"
#include "IMU.h"
extern "C"{
  #include "attitude_kalman_filter.h"
  #include "attitude_comp_filter.h"
  #include "buses_definition.h"
}

// Define filter input and output bus
inp_Bus input_bus;
out_Bus output_bus;

// Define IMU sensor object
IMU IMU_sensor;

// TFT Screen
TFT_eSPI tft = TFT_eSPI();

// Artificial horizon display
ArtificialHorizon horizon(tft);


void task100Hz(void *pvParameters) {
  const TickType_t xFrequency = pdMS_TO_TICKS(10);
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (true) {
    
    // Read sensor
    IMU_sensor.read();

    // Assign to input bus
    input_bus.rot_K_meas_OB_B_radDs[0] = IMU_sensor.sensor_meas.rot_K_meas_IB_B_radDs[0];
    input_bus.rot_K_meas_OB_B_radDs[1] = IMU_sensor.sensor_meas.rot_K_meas_IB_B_radDs[1];
    input_bus.rot_K_meas_OB_B_radDs[2] = IMU_sensor.sensor_meas.rot_K_meas_IB_B_radDs[2];

    input_bus.sfor_meas_B_mDs2[0] = IMU_sensor.sensor_meas.sfor_meas_B_mDs2[0];
    input_bus.sfor_meas_B_mDs2[1] = IMU_sensor.sensor_meas.sfor_meas_B_mDs2[1];
    input_bus.sfor_meas_B_mDs2[2] = IMU_sensor.sensor_meas.sfor_meas_B_mDs2[2];

    input_bus.stop_correction_bool = false;

    /* Step Kalman Filter */
    attitude_kalman_filter_U.inp = input_bus;
    attitude_kalman_filter_step();

    /* Step complementary Filter */
    attitude_comp_filter_U.inp = input_bus;
    attitude_comp_filter_step();

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void setup(void)
{

  // Initialize IMU
  IMU_sensor.init();

  // Initialize Kalman filter
  attitude_kalman_filter_initialize();

  // Initialize complementary filter
  attitude_comp_filter_initialize();

  // Initialize display
  tft.init();
  tft.setRotation(1); 
  tft.fillScreen(TFT_BLACK);

  // Initialize attitude indicator
  horizon.begin();

  xTaskCreatePinnedToCore(
    task100Hz,
    "100Hz Task",
    4096,
    NULL,
    2,      // Higher priority
    NULL,
    0       // Run on core 1
  );
}

void loop(void)
{

  // Get output from estimation filter
  output_bus = attitude_comp_filter_Y.out;
  output_bus = attitude_kalman_filter_Y.out;

  // Update display
  horizon.update(
    output_bus.Eul_rad[0] * RAD_TO_DEG,
    output_bus.Eul_rad[1] * RAD_TO_DEG, 
    output_bus.Eul_rad[2] * RAD_TO_DEG
  );
  horizon.draw();
}