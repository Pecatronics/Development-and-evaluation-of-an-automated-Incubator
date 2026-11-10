// #include "LCDDisplay.h" 
// #include "ButtonHandler.h" 
// #include "EEPROMHandler.h" 
#include "TempHumiditySensor.h" 
// #include "RelayControl.h" 
// #include "Timer.h" 
// #include "WaterLevelSensor.h" 
#include "GyroscopeHandler.h" 
// Thresholds and constants 
const float thresholdTemp = 37.5; 
const float thresholdHumi = 53.0; 
const float hysteresis = 0.5; 
const float hysteresisHumi = 2.0; 
const long waterLevelThreshold = 10; // Example value, adjust as necessary 
const long tiltInterval = 8 * 60 * 60 * 1000; // 8 hours in milliseconds 
const long tiltDuration = 60 * 60 * 1000; // 1 hour in milliseconds 
class IncubatorControl { 
  private: 
    LCDDisplay lcd; 
    ButtonHandler startButton, stopButton, tempUpButton, tempDownButton, daysUpButton, 
daysDownButton, resetButton; 
    EEPROMHandler eeprom; 
    TempHumiditySensor tempHumSensor; 
    RelayControl heatingElementRelay, cirFanRelay, bulbRelay, exhaustRelay, airInletRelay, 
humidifierRelay, solenoidValveRelay, tiltingMechanismRelay; 
    Timer timer; 
    WaterLevelSensor waterLevelSensor; 
    GyroscopeHandler gyroscope; 
 
    byte temperature; 
    byte days; 
    unsigned long incubationDays = 21; // Example value, adjust as necessary 
    unsigned long lastTiltTime = 0; 
    unsigned long tiltStartTime = 0; 
    bool tilting = false; 
  public: 
    IncubatorControl()
      : lcd(), startButton(8), stopButton(9), tempUpButton(10), tempDownButton(11), 
daysUpButton(12), daysDownButton(13), resetButton(14), 
        tempHumSensor(2, 3), heatingElementRelay(21), cirFanRelay(22), bulbRelay(23), 
exhaustRelay(24), airInletRelay(25), humidifierRelay(26), solenoidValveRelay(27), 
tiltingMechanismRelay(28), 
        timer(3600000), waterLevelSensor(12, 11) {} 
 
    void begin() { 
      lcd.begin(); 
      startButton.begin(); 
      stopButton.begin(); 
      tempUpButton.begin(); 
      tempDownButton.begin(); 
      daysUpButton.begin(); 
      daysDownButton.begin(); 
      resetButton.begin(); 
      tempHumSensor.begin(); 
      gyroscope.begin(); 
      heatingElementRelay.begin(); 
      cirFanRelay.begin(); 
      bulbRelay.begin(); 
      exhaustRelay.begin(); 
      airInletRelay.begin();
      humidifierRelay.begin(); 
      solenoidValveRelay.begin(); 
      tiltingMechanismRelay.begin(); 
      waterLevelSensor.begin(); 
      temperature = eeprom.read(0); 
      days = eeprom.read(1); 
      displayStatus(); 
    } 
    void update() { 
      if (startButton.isPressed()) { 
        timer = 0; 
        temperature = thresholdTemp; 
        days = 0; 
        eeprom.write(0, temperature); 
        eeprom.write(1, days); 
        displayStatus(); 
      } 
 
      if (stopButton.isPressed()) { 
        timer = 0; 
        heatingElementRelay.off(); 
      }
        if (tempUpButton.isPressed()) { 
        temperature++; 
        eeprom.write(0, temperature); 
        displayStatus(); 
      } 
      if (tempDownButton.isPressed()) { 
        temperature--; 
        eeprom.write(0, temperature); 
        displayStatus(); 
      } 
      if (daysUpButton.isPressed()) { 
        days++; 
        eeprom.write(1, days); 
        displayStatus(); 
      } 
      if (daysDownButton.isPressed()) { 
        days--; 
        eeprom.write(1, days); 
        displayStatus(); 
      } 
      if (resetButton.isPressed()) { 
        temperature = thresholdTemp; 
        days = 0;
          eeprom.write(0, temperature); 
        eeprom.write(1, days); 
        displayStatus(); 
      } 
      float temp = tempHumSensor.readTemperature(); 
      float humi = tempHumSensor.readHumidity(); 
      controlRelays(temp, humi); 
      if (timer.isReady()) { 
        timer++; 
        displayStatus(); 
      } 
      long distance = waterLevelSensor.readDistance(); 
      if (distance > waterLevelThreshold) { // If this doesn't work, try < 
        solenoidValveRelay.on(); 
      } else { 
        solenoidValveRelay.off(); 
      } 
      manageTilting(); 
    } 
 
    void displayStatus() { 
      lcd.clear(); 
      lcd.display("Temp: " + String(temperature) + "C", 0, 0);
      lcd.display("Days: " + String(days) + "d", 0, 1); 
      lcd.display("Time: " + String(timer), 8, 1); 
    } 
    void controlRelays(float temp, float humi) { 
      if (temp < thresholdTemp - hysteresis) { 
        heatingElementRelay.on(); 
        cirFanRelay.on(); 
      } else if (temp > thresholdTemp + hysteresis) { 
        heatingElementRelay.off(); 
        cirFanRelay.off(); 
      } 
      if (humi < thresholdHumi - hysteresisHumi) { 
        humidifierRelay.on(); 
        cirFanRelay.on(); 
      } else if (humi > thresholdHumi + hysteresisHumi) { 
        humidifierRelay.off(); 
      } 
    } 
    void manageTilting() { 
      unsigned long currentTime = millis(); 
      if (!tilting andand (currentTime - lastTiltTime >= tiltInterval)) { 
        tilting = true; 
        tiltStartTime = currentTime;
        tiltingMechanismRelay.on(); 
      } 
      if (tilting andand (currentTime - tiltStartTime >= tiltDuration)) { 
        if (gyroscope.isHorizontal()) { 
          tiltingMechanismRelay.off(); 
          tilting = false; 
          lastTiltTime = currentTime; 
        } 
      } 
    } 
}; 
IncubatorControl incubator; 
void setup() { 
  incubator.begin(); 
} 
void loop() { 
  incubator.update(); 
}