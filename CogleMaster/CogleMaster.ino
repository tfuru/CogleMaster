#include <Wire.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include "PCF8591.h"
#include "CogleMasterAP.h"
#include "CogleMasterSTA.h"

//設定情報を保存する
CogleMasterEEPROM cogleEeprom;

//ステーションモード時のメイン処理を実装したクラス
CogleMasterSTA staMode;

//アクセスポイントモード時のメイン処理を実装したクラス
CogleMasterAP apMode;

//GPIO0,SDA
const int pinSdaGpio0 = 0;
//GPIO2,SCL
const int pinSclGpio2 = 2;

//接続先 i2c アドレス
const int cogleSlave0I2CAddr = (0x90 >> 1);
//const int cogleSlave1I2CAddr = (0x90 >> 1);
//const int cogleSlave2I2CAddr = (0x90 >> 1);

//STAモード時の Wi-Fi 設定情報
char staSsid[COGLEMASTER_EEPROM_ADDR_CONFIG_SSID_SIZE];
char staPassword[COGLEMASTER_EEPROM_ADDR_CONFIG_PASSWORD_SIZE];
char deviceKey[COGLEMASTER_EEPROM_ADDR_CONFIG_DEVICE_KEY_SIZE];
    
//APモードの時の設定
const IPAddress apIpAddr = IPAddress(192, 168, 0, 1);
const IPAddress apIpSubNetAddr = IPAddress(255, 255, 255, 0);
const char* apModeSsid = "CogleMasterAP";
const char* apModePassword = "1234567890";
const char* apDnsDomain = "cogle_ap";

bool isAPMode = false;
bool isSTAModeInit = false;

void setup() {
  Serial.begin(115200);
  
  //設定をEEPROMに記録しているので読み出し用
  cogleEeprom.begin(512);
    
  //モード決定までWi-FiはOFF(WIFI_OFF パラメータが無いバージョンなので WIFI_STA)を設定とく
  WiFi.mode(WIFI_STA);
  
  //起動モード検知のためGPIO2をINPUTに設定する
  pinMode(pinSclGpio2, INPUT);
}

void loop() {
  //
  if(isAPMode == false){
    int modePin = digitalRead(pinSclGpio2);
    if(modePin == LOW){
      //APモードを起動する
      apMode.begin(apIpAddr,
                 apIpSubNetAddr,
                 apDnsDomain,
                 apModeSsid,
                 apModePassword);
      
      isAPMode = true;
      Serial.println("isAPMode:true");
    }
  }
  
  if(isAPMode == true){
    //APモードの時のメイン処理実行
    bool flg = apMode.receiveRequest();
  }
  else{
    //通常モード
    //初期設定が終わっていない場合は なにもしない

    cogleEeprom.config_load(staSsid,staPassword,deviceKey);
    if(staSsid != 0){
      //初期設定済み
      if(isSTAModeInit == false){
        Serial.print("staSsid:");     Serial.println(staSsid);
        Serial.print("staPassword:"); Serial.println(staPassword);
        Serial.print("deviceKey:");   Serial.println(deviceKey);
      
        //通常起動モードの初期設定を実行
        staMode.begin(pinSdaGpio0,pinSclGpio2,staSsid,staPassword);
        isSTAModeInit = true;
      }
      else{
        //CogleSlaveの値の読み出し&送信
        staMode.send(cogleSlave0I2CAddr);
      }
      
    }
    
    delay(500);    
  }
}

/*
//logServer IPとポート
//const char* host = "192.168.0.3";
//const int httpPort = 3000;

//Logをサーバに送信
void sendLog(int aioPort,int value){
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    return;
  }
  
  // We now create a URI for the request
  String url = "/device/";
          url += aioPort;
          url += "?value=";
          url += value;
    
  // This will send the request to the server
  client.print(String("GET ")
                + url 
                + " HTTP/1.1\r\n" 
                + "Host: " + host + "\r\n"
                + "Connection: close\r\n\r\n");
  delay(10);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
  }
  
}
*/
