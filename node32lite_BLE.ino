/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/
#include <Arduino.h>
#include <sstream>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

/*  Duration of BLE scan
    ==============                  ==============                   ==============
    =   WINDOW   =  ===INTERVAL===  =   WINDOW   =  ===INTERVAL===   =   WINDOW   =
    ==============                  ==============                   ==============
    ===============================================================================
    =                                  SCAN TIME                                  =
    ===============================================================================
*/
#define SCAN_TIME       1  // seconds
#define INTERVAL_TIME   200   // (mSecs)
#define WINDOW_TIME     100   // less or equal setInterval value

BLEScan* pBLEScan;

String deviceName;
String deviceAddress;
int16_t deviceRSSI;
uint16_t countDevice;


boolean check;
int count_stable = 0; 

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      /* unComment when you want to see devices found */
      Serial.printf("Found device: %s \n", advertisedDevice.toString().c_str());
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("BLEDevice init...");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(INTERVAL_TIME); // Set the interval to scan (mSecs)
  pBLEScan->setWindow(WINDOW_TIME);  // less or equal setInterval value
}

void loop() {
  BLEScanResults foundDevices = pBLEScan->start(SCAN_TIME);

  int count = foundDevices.getCount();
  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices.getDevice(i);

    if (d.getName() == "Amazfit Bip Watch") {
      check = true;
      count_stable =0;
       
      char deviceBuffer[100];
      deviceName = d.getName().c_str();
      deviceAddress = d.getAddress().toString().c_str();
      deviceRSSI = d.getRSSI();

      sprintf(deviceBuffer, "Name: %s| Address: %s| RSSI: %d\n", deviceName.c_str(), deviceAddress.c_str(), deviceRSSI);
      Serial.print(deviceBuffer);

      if (deviceAddress == "cd:bc:16:89:51:9f" && deviceRSSI > -80)
      {
        digitalWrite(LED_BUILTIN, LOW); // Turn on LED
        Serial.println("ON");
      }
      else
      {
        digitalWrite(LED_BUILTIN, HIGH); // Turn off LED
        Serial.println("OFF");
      }
      //---------------------------------------------------------Check if not found Mi Band-------------------------------------
    }else if(i == count-1 && check == false){
      count_stable +=1;
      if(count_stable ==20){ // set limit to reset counter
        count_stable =0;
      }
      Serial.println(count_stable);
      if(count_stable == 4){ //set quantity of scan cycle for accept missing Mi Band
      digitalWrite(LED_BUILTIN, HIGH); 
        Serial.println("Not Found");
      }
    }
      check = false;
      //---------------------------------------------------------------------------------------------------------------------------
  }
  pBLEScan->clearResults();
}
