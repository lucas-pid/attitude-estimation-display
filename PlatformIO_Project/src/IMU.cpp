#include "IMU.h"

// Inialization function
// Change accordingly depending on sensor
bool IMU::init() {
    _mag.enableAutoRange(true);
    _accel.begin();
    _gyro.init();
    _gyro.enableDefault();
    return true; // or false if initialization fails
}

// Read sensor
// Change accordingly depending on sensor
void IMU::read() {
    
    _accel.getEvent(&_event);
    _gyro.read();

    // Populate the data structure (example values)
    sensor_meas.rot_K_meas_IB_B_radDs[0] = _gyro.g.x * sensitivity_gyro_degDs * DEG_TO_RAD; // x-axis
    sensor_meas.rot_K_meas_IB_B_radDs[1] = _gyro.g.y * sensitivity_gyro_degDs * DEG_TO_RAD; // y-axis
    sensor_meas.rot_K_meas_IB_B_radDs[2] = _gyro.g.z * sensitivity_gyro_degDs * DEG_TO_RAD; // z-axis
    
    sensor_meas.sfor_meas_B_mDs2[0] = _event.acceleration.x;  // x-axis
    sensor_meas.sfor_meas_B_mDs2[1] = _event.acceleration.y;  // y-axis
    sensor_meas.sfor_meas_B_mDs2[2] = _event.acceleration.z;  // z-axis
    
    sensor_meas.mag[0] = 0.0f; // x-axis
    sensor_meas.mag[1] = 0.0f;  // y-axis
    sensor_meas.mag[2] = 0.0f;  // z-axis
    
}