#include <Wire.h> 
#include <MPU6050.h>
class GyroscopeHandler { 
  private: 
    MPU6050 mpu; 
    const int horizontalThreshold = 5; // Threshold for horizontal plane (in degrees) 
  public: 
    void begin() { 
      Wire.begin(); 
      mpu.initialize(); 
      if (!mpu.testConnection()) { 
        Serial.println("MPU6050 connection failed"); 
      } 
      else { 
        Serial.println("MPU6050 connection successful"); 
      }
       } 
    bool isHorizontal() { 
      int16_t ax, ay, az; 
      mpu.getAcceleration(andax, anday, andaz); 
      float angleX = atan2(ax, sqrt(ay * ay + az * az)) * 180.0 / PI; 
      return (abs(angleX) < horizontalThreshold); 
    } 
}