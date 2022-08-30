#include <BLEDevice.h> // inclui outras bibliotecas, cria um novo cliente BLE, cria um servidor BLE, obtém o local do aparelho de BLE, pega a sonda do objeto, inicializa o ambiente de BLE, habilita multiconexão.
#include <BLEServer.h> // estrutura o servidor do BLE com conexão e de desconexão de clientes.
#include <BLEUtils.h> // Adiciona parâmetros e configurações utilitárias para o servidor BLE.
#include <BLE2902.h> // Notifica o servidor quanto a conexão do cliente.
#include <Wire.h> //--*--*-- Eduardo --*--*-- 
#include "SSD1306Wire.h" //--*--*-- Eduardo --*--*-- 

//Define a pinagem e o tipo do display
SSD1306Wire  display(0x3c, 21, 22); //--*--*-- Eduardo --*--*-- 

// Ponteiros, variaveis e condicoes booleanas
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;
const int LED = 2;

// Geracao do servidor BLE
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_RX "4ac8a682-9736-4e5d-932b-e9b31405049c"


// Classe para mostrar o BLE e indentificar conexão com o servidor
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

// Classe para receber os valores mandados pelo dispositivo
class CharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *characteristic) {
      //retorna ponteiro para o registrador contendo o valor atual da caracteristica
      std::string rxValue = characteristic->getValue(); 
      //verifica se existe dados (tamanho maior que zero)
      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Valor recebido: ");

        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
        }

        Serial.println();

        // A partir do comando 'L1' executa os seguintes comandos
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
          Serial.print("Ligando o LED!");
          digitalWrite(LED, HIGH);
        }
        // Caso 'L0' os seguintes comandos
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
          Serial.print("Desligando o LED!"); 
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

// Corrige bug do display de caracteres estranhos
  Wire.setClock(10000); //--*--*-- Eduardo --*--*-- 
  
  // Cria o dispositivo BLE
  BLEDevice::init("Irrigador-BLE");

  // Cria o servidor BLE
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Cria o tipo de serviço BLE
  BLEService *pService = pServer->createService(SERVICE_UUID);

  //Cria as caracteriscas do BLE
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,  
                      BLECharacteristic::PROPERTY_NOTIFY
                    ); 

  // Descrição do BLE
  pCharacteristic->addDescriptor(new BLE2902());

 BLECharacteristic *characteristic= pService->createCharacteristic(
                      CHARACTERISTIC_UUID_RX,  
                      BLECharacteristic::PROPERTY_WRITE
                    ); 

  characteristic->setCallbacks(new CharacteristicCallbacks());
  
  // Começa o serviço
  pService->start();

  // Começa a 'anunciar' o dispositivo BLE (advertising)
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();
}

// loop para verificar a conexao dos dispositivos e caso desconecte os dispositivo novo se torna o dispositivo novo, corrigindo o bug em que só podia conectar um dispositivo
void loop() {
    // Notifica a mudança de valor 
    if (deviceConnected) {
        pCharacteristic->setValue((uint8_t*)&value, 4);
        pCharacteristic->notify();
        value++;
        delay(10); // Pode ser mais rapido mas corre risco de travar devido a quantidade de pacotes sendo enviados
        //Serial.println("Conectado");
    }
    // Desconecta o disposistivo pareado anteriormente
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // da tempo para os pacotes não acumularem
        pServer->startAdvertising(); // Recomeça a 'anunciar' o dispositivo BLE (advertising)
        //Serial.println("Desconectado");
        oldDeviceConnected = deviceConnected;
    }
    // Conecta novamente com o novo dispositivo
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
}
