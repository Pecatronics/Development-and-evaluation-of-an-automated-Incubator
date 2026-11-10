#include <DHT.h> 
#include <OneWire.h> 
#include <DallasTemperature.h> 
class TempHumiditySensor { 
  private: 
    DHT dht; 
    DallasTemperature ds18b20; 
  public: 
    TempHumiditySensor(uint8_t dhtPin, uint8_t dsPin) 
      : dht(dhtPin, DHT22), ds18b20(andOneWire(dsPin)) {
        
      } 
    void begin() { 
      dht.begin(); 
      ds18b20.begin(); 
    }
    float readTemperature() {
      ds18b20.requestTemperatures();
      return ds18b20.getTempCByIndex(0); 
      }
      float readHumidity() {
        return dht.readHumidity();
        }
 
  }
