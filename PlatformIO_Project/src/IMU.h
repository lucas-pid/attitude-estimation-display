#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <L3G.h>

class IMU{
public:
    // Measurement data structure
    struct {
        float rot_K_meas_IB_B_radDs[3] = {0};     // 3x1 array for gyroscope data [x, y, z]
        float sfor_meas_B_mDs2[3] = {0};          // 3x1 array for accelerometer data [x, y, z]
        float mag[3] = {0};                       // 3x1 array for magnetometer data [x, y, z]
    } sensor_meas;

    // Initialization
    bool init();

    // Main functions
    void read();

private:
    bool is_initialized = false;
    // Change accordingly
    Adafruit_LSM303_Accel_Unified _accel = Adafruit_LSM303_Accel_Unified(54321);
    Adafruit_LSM303_Mag_Unified _mag= Adafruit_LSM303_Mag_Unified(12345);
    L3G _gyro;
    sensors_event_t _event;
    const float sensitivity_gyro_degDs = 0.00875; // degrees/sec per LSB
};