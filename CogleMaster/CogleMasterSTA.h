/** CogleMasterSTA.h
ステーションモードで起動する場合に利用

*/
#ifndef CogleMasterSTA_h
#define CogleMasterSTA_h

#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "CogleMasterEEPROM.h"
#include "CogleMasterConfig.h"
#include "PCF8591.h"

class CogleMasterSTA {

  private:
    //EEPROMに保存んした設定情報を読み出す
    CogleMasterEEPROM cogleEeprom;

    //CogleSlaveを扱う為のクラス
    PCF8591 cogleSlave;
    
    //サーバに保存した設定情報を読み出す
    CogleMasterConfig cogleConfig;
  public:
    CogleMasterSTA();
    ~CogleMasterSTA();
    
    //初期化
    void begin(const int sdaPin,const int sclPin,const char* ssid,const char* password);
    //終了処理
    void end(void);
    
    //センサー値の送信
    void send(const int cogleSlaveAddr);
};

#endif /* CogleMasterSTA_h */
