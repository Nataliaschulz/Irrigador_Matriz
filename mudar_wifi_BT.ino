 
  #include <WiFi.h>
  #include "BluetoothSerial.h"
  
  const char* ssid     = "WiFi Name";
  const char* password = "WiFi Password";
  
  #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
  #endif
  BluetoothSerial SerialBT;
  
  void turn_ON_WIFI() {
    Serial.println("WIFI ON");
    WiFi.begin(ssid, password);
    delay(2000);
  }

  void turn_OFF_WIFI() {
    Serial.println("WIFI OFF");
    WiFi.mode( WIFI_MODE_NULL );
    delay(2000);
  }

  void turn_ON_BLE() {
    Serial.println("BLE ON");
    SerialBT.begin("ESP32test");
    delay(2000);
  }
  
  void turn_OFF_BLE() {
    Serial.println("BLE OFF");
    btStop();
    delay(2000);
  }
  
  void setup() {
    Serial.begin(115200);
  }
  
  void loop() {

    turn_ON_BLE();
    delay(30000);

    turn_OFF_BLE();
    delay(30000);

    turn_ON_WIFI();
    delay(30000);

    turn_OFF_WIFI();
    delay(30000);

  }
  
