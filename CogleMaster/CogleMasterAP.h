/** CogleMasterAP.h
アクセスポイントモードで起動する場合に利用

*/
#ifndef CogleMasterAP_h
#define CogleMasterAP_h

#include <Arduino.h>
#include <IPAddress.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include "CogleMasterEEPROM.h"

class CogleMasterAP {

  private:
    MDNSResponder mdns;
    WiFiServer apiServer;
    CogleMasterEEPROM cogleEeprom;
    
    //接続クライアント待ち
    WiFiClient clientAvailable(void);
    
    //リクエストURLを抽出する
    String getRequestURL(const String request);
    
    //httpヘッダー生成
    String createHttpHeader(const int code,const String message,const String contentType);
    
    //セパレータ分割
    String getStringPartByNr(String data, char separator, int index);
    
    //http POSTリクエストのパラメータ抽出
    void parsePostRequest(String body,String keys[],String params[],int len);
    
    //Requestが届いた時の処理
    String onReqSampleApi(WiFiClient client,const String path);
    //設定値RESET APIの処理
    String onReqConfigResetApi(WiFiClient client,const String path);
    
    //設定値保存 APIの処理
    String onReqConfigSaveApi(WiFiClient client,const String path);
    
  public:
    CogleMasterAP();
    ~CogleMasterAP();
    
    //初期化
    void begin(IPAddress localIp,IPAddress subnet,const char* dnsDomain,const char* ssid,const char* password);
    //終了処理
    void end(void);
    //リクエスト待ち等のメイン処理
    bool receiveRequest( void );  
};

#endif /* CogleMasterAP_h */
