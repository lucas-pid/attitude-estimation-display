#include <TFT_eSPI.h>
#include <SPI.h>
#include "ArtificialHorizon.h"
#include "IMU.h"
#include "OneButton.h"
extern "C"{
  #include "attitude_filter_wrapper.h"
  #include "buses_definition.h"
}

// Define GIO pin for button
const int BUTTON_PIN = 17;

// Integer to track mode of operation
int mode = 0;

// Some variables used to display text at certain events
bool display_mode_text = false;
int display_mode_text_counter = 0;
bool display_reset_text = false;
int display_reset_text_counter = 0;
const int display_text_ticks = 200;   // For how many steps the text messages will appear

// Define filter input and output bus
inp_Bus input_bus;
out_Bus output_bus;

// Define IMU sensor object
IMU IMU_sensor;

// TFT Screen
TFT_eSPI tft = TFT_eSPI();

// Artificial horizon display
ArtificialHorizon horizon(tft);

// Setup a new OneButton on pin 17.  
OneButton button(BUTTON_PIN);

// Function to reset allignment
void reset_filter() {
  // Set input flags for the estimation
  input_bus.reset_allign_bool = true;
  input_bus.reset_bool = true;
  // Set flag to display "reset" text
  display_reset_text = true;
}

// Function to set allignment
void beginallignment() {
  input_bus.allign_bool = true;
}

// Function to stop allignment
void stopallignment() {
  input_bus.allign_bool = false;
}

// Function to cycle between the different modes when the button is pressed
void cycle_mode() {

  // Go to next mode
  mode++;

  // Set flag to show current mode on display
  display_mode_text = true;
  display_mode_text_counter = 0;

  // Reset integrators
  input_bus.reset_bool = true;

  switch (mode)
  {
  //case 0:
  //  // Kalman filter
  //  input_bus.mode_idx  = 0;
  //  break;
  case 1:
    // Dead reckoning
    input_bus.mode_idx  = 1;
    break;

  case 2:
    // Complementary filter
    input_bus.mode_idx  = 2;
    break;

  case 3:
    // Inclinometer
    input_bus.mode_idx  = 3;
    break;

  default:
    // Kalman filter
    // Set mode back to zero for a cycling behavior
    mode = 0;
    input_bus.mode_idx  = 0;
    break;
  }

}

void task100Hz(void *pvParameters) {
  const TickType_t xFrequency = pdMS_TO_TICKS(10);
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (true) {

    // Read sensor
    IMU_sensor.read();

    // Assign the measurements to input bus
    input_bus.rot_K_meas_IB_B_radDs[0] = IMU_sensor.sensor_meas.rot_K_meas_IB_B_radDs[0];
    input_bus.rot_K_meas_IB_B_radDs[1] = IMU_sensor.sensor_meas.rot_K_meas_IB_B_radDs[1];
    input_bus.rot_K_meas_IB_B_radDs[2] = IMU_sensor.sensor_meas.rot_K_meas_IB_B_radDs[2];
    input_bus.sfor_meas_B_mDs2[0] = IMU_sensor.sensor_meas.sfor_meas_B_mDs2[0];
    input_bus.sfor_meas_B_mDs2[1] = IMU_sensor.sensor_meas.sfor_meas_B_mDs2[1];
    input_bus.sfor_meas_B_mDs2[2] = IMU_sensor.sensor_meas.sfor_meas_B_mDs2[2];

    // Step Filter
    attitude_filter_wrapper_U.inp = input_bus;
    attitude_filter_wrapper_step();

    // Set all reset flags to false
    input_bus.reset_bool = false;
    input_bus.reset_allign_bool = false;

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void setup(void)
{

  //Serial.begin(115200);

  // Initialize IMU
  IMU_sensor.init();

  // Initialize filter
  attitude_filter_wrapper_initialize();

  // Set button pin
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Attach functions to button press
  button.attachDoubleClick(reset_filter);
  button.attachClick(cycle_mode);
  button.attachDuringLongPress(beginallignment);
  button.attachLongPressStop(stopallignment);

  // Initialize display
  tft.init();
  tft.setRotation(1); // Change accordingly depending on the orientation of the screen
  tft.fillScreen(TFT_BLACK);

  // Initialize attitude indicator
  horizon.begin();

  // Add task to run estimation filter at the specified rate
  xTaskCreatePinnedToCore(
    task100Hz,
    "100Hz Task",
    8000,
    NULL,
    2,      // Higher priority
    NULL,
    0       // Run on core 1
  );
}

void loop(void)
{

  // Get output from estimation filter
  output_bus = attitude_filter_wrapper_Y.out;

  // Set initial state for filter reset
  for (int i=0; i < 13; i++){
    input_bus.delta_x0[i] = output_bus.x_est[i];
  }

  // Update horizon sprite
  horizon.update(
    output_bus.Eul_rad[0] * RAD_TO_DEG,
    output_bus.Eul_rad[1] * RAD_TO_DEG, 
    output_bus.Eul_rad[2] * RAD_TO_DEG
  );

  //====== Print text on screen if there is any "event" active =============================
  // Alignment
  if (input_bus.allign_bool){
    horizon.add_text("ALIGNING", 0, 135, TFT_WHITE, TFT_BLACK, CL_DATUM);
  }

  // Reset
  if (display_reset_text){
    horizon.add_text("RESET", 240, 135, TFT_WHITE, TFT_RED, CR_DATUM);
    
    // Increment counter
    display_reset_text_counter++;

    // Reset counter and stop display if it reaches limit
    if (display_reset_text_counter > display_text_ticks){
      display_reset_text_counter = 0;
      display_reset_text = false;
    }
  }

  // Type of filter
  if (display_mode_text){
    

    char filter_mode[20];

    //switch (input_bus.mode_idx)
    switch (output_bus.mode)
    {
    case 0:
      strcpy(filter_mode, "Kalman Filter");
      break;

    case 1:
      strcpy(filter_mode, "Dead Reckoning");
      break;

    case 2:
      strcpy(filter_mode, "Comp. Filter");
      break;

    case 3:
      strcpy(filter_mode, "Inclinometer");
      break;
    
    default:
      strcpy(filter_mode, "Unexpected");
      break;
    }

    // Add text indicating the estimation mode
    horizon.add_text(filter_mode, 240, 155, TFT_YELLOW, TFT_BLACK, CR_DATUM);
    
    // Increment counter
    display_mode_text_counter++;

    // Reset counter and stop display if it reaches limit
    if (display_mode_text_counter > display_text_ticks){
      display_mode_text_counter = 0;
      display_mode_text = false;
    }
  }
  //========================================================================================

  // Update display
  horizon.draw();

  // Listen to button
  button.tick();

}