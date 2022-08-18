#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h> //--*--*-- Eduardo --*--*-- 
#include "SSD1306Wire.h" //--*--*-- Eduardo --*--*-- 

SSD1306Wire  display(0x3c, 21, 22); //--*--*-- Eduardo --*--*-- 

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;
const int LED = 2;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_RX "4ac8a682-9736-4e5d-932b-e9b31405049c"


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class CharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *characteristic) {
      //retorna ponteiro para o registrador contendo o valor atual da caracteristica
      std::string rxValue = characteristic->getValue(); 
      //verifica se existe dados (tamanho maior que zero)
      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");

        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
        }

        Serial.println();

        // Do stuff based on the command received
        if (rxValue.find("L1") != -1) { 
          display.clear(); //--*--*-- Eduardo --*--*--    
          display.setTextAlignment(TEXT_ALIGN_CENTER); //--*--*-- Eduardo --*--*-- 
          display.setFont(ArialMT_Plain_24); //--*--*-- Eduardo --*--*-- 
          display.drawString(64, 22, "Ligado"); //--*--*-- Eduardo --*--*-- 
          display.setTextAlignment(TEXT_ALIGN_CENTER); //INTERNET //--*--*-- Eduardo --*--*-- 
          display.setFont(ArialMT_Plain_16); //INTERNET //--*--*-- Eduardo --*--*-- 
          //display.drawString(64, 42, ntp.getFormattedTime()); //INTERNET //--*--*-- Eduardo --*--*-- 
          display.drawString(64, 50, "11:06"); //INTERNET //--*--*-- Eduardo --*--*-- 
          display.setTextAlignment(TEXT_ALIGN_LEFT); //--*--*-- Eduardo --*--*-- 
          display.setFont(ArialMT_Plain_16); //--*--*-- Eduardo --*--*-- 
          display.drawString(0, 0, "12/08/2022"); //--*--*-- Eduardo --*--*-- 
          display.display(); //--*--*-- Eduardo --*--*--  
          Serial.print("Turning LED ON!");
          digitalWrite(LED, HIGH);
        }
        else if (rxValue.find("L0") != -1) {
          display.clear(); //--*--*-- Eduardo --*--*--   
          display.setTextAlignment(TEXT_ALIGN_CENTER); //--*--*-- Eduardo --*--*-- 
          display.setFont(ArialMT_Plain_24); //--*--*-- Eduardo --*--*-- 
          display.drawString(64, 22, "Desligado"); //--*--*-- Eduardo --*--*-- 
          display.setTextAlignment(TEXT_ALIGN_CENTER); //INTERNET //--*--*-- Eduardo --*--*-- 
          display.setFont(ArialMT_Plain_16); //INTERNET //--*--*-- Eduardo --*--*-- 
          //display.drawString(64, 42, ntp.getFormattedTime()); //INTERNET //--*--*-- Eduardo --*--*-- 
          display.drawString(64, 50, "11:06"); //INTERNET //--*--*-- Eduardo --*--*-- 
          display.setTextAlignment(TEXT_ALIGN_LEFT); //--*--*-- Eduardo --*--*-- 
          display.setFont(ArialMT_Plain_16); //--*--*-- Eduardo --*--*-- 
          display.drawString(0, 0, "12/08/2022"); //--*--*-- Eduardo --*--*-- 
          display.display(); //--*--*-- Eduardo --*--*-- 
          Serial.print("Turning LED OFF!"); 
          digitalWrite(LED, LOW);
        }
        Serial.println();
        Serial.println("*********");
      }
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println(); //--*--*-- Eduardo --*--*-- 
  Serial.println(); //--*--*-- Eduardo --*--*-- 

// INICIALIZA O DISPLAY
  display.init(); //--*--*-- Eduardo --*--*-- 
  display.flipScreenVertically(); // INVERTE O DISPLAY VERTICALMENTE //--*--*-- Eduardo --*--*-- 

  pinMode(LED,OUTPUT);

  Wire.setClock(10000); //--*--*-- Eduardo --*--*-- 
  
  // Create the BLE Device
  BLEDevice::init("Irrigador-BLE");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,  
                      BLECharacteristic::PROPERTY_NOTIFY
                    ); 

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

 BLECharacteristic *characteristic= pService->createCharacteristic(
                      CHARACTERISTIC_UUID_RX,  
                      BLECharacteristic::PROPERTY_WRITE
                    ); 

  characteristic->setCallbacks(new CharacteristicCallbacks());
  
  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
    // notify changed value
    if (deviceConnected) {
        pCharacteristic->setValue((uint8_t*)&value, 4);
        pCharacteristic->notify();
        value++;
        delay(10); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
        //Serial.println("Conectado");
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        //Serial.println("Desconectado");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
