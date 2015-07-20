/** CogleMasterConfig.h
ユーザー設定やIFTTT設定等の情報をサーバから読み込むクラス

*/
#ifndef CogleMasterConfig_h
#define CogleMasterConfig_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

//Slaveモジュールの数
#define SLAVE_COUNT 3

//IFTTT設定
class CogleDeviceIFTTT {
  public:
    String deviceKey;
    String deviceSlaveId;
    String secretKey;
    String event;
    String actionType;
};

class CogleMasterConfig {

  private:
    const char* deviceKey;
    WiFiClient client;
    
    CogleDeviceIFTTT deviceIfttt[SLAVE_COUNT];
    
    //GETリクエストをしてbody部分を受信
    void httpGet(String url,char* body);
  public:
    CogleMasterConfig();
    ~CogleMasterConfig();
    
    //初期化
    void begin(const char* deviceKey);
    
    //終了処理
    void end(void);

    //設定を設定サーバーから読み出す
    void loadDeviceIFTTT(void);

    //アドレスに一致した IFTTT 設定を読み出す
    CogleDeviceIFTTT getDeviceIFTTT( String slaveAddrHex );
};

#endif /* CogleMasterConfig_h */
