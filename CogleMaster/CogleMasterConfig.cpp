/** CogleMasterConfig.cpp
 ユーザー設定やIFTTT設定等の情報をサーバから読み込むクラス
 
 */
#include "CogleMasterConfig.h"

//設定保存サーバの設置先
//TODO:本来は外部のドメイン持ちのサーバ
#define COFIG_SERVER_API_HOST "192.168.1.178"
//#define COFIG_SERVER_API_HOST "nodejs.moe.hm"
#define COFIG_SERVER_API_PORT 3030

//IFTTT設定 情報
#define COFIG_SERVER_API_DEVICE_IFTTT "/device_ifttt"

CogleMasterConfig::CogleMasterConfig(){
  
}

CogleMasterConfig:: ~CogleMasterConfig(){
  end();
}

//初期化
void CogleMasterConfig::begin(const char* deviceKey){
  this->deviceKey = deviceKey;
  
  if (!client.connect(COFIG_SERVER_API_HOST, COFIG_SERVER_API_PORT)) {
    //サーバの接続に失敗
    Serial.println( String("Connect failure ") + String(COFIG_SERVER_API_HOST) + " "+ String(COFIG_SERVER_API_PORT) );
    return;
  }
  
}

//終了処理
void CogleMasterConfig::end(void){
  client.stop();
}

//設定を設定サーバーから読み出す
void CogleMasterConfig::loadDeviceIFTTT(void){

  String url = String(COFIG_SERVER_API_DEVICE_IFTTT)
               +"?deviceKey="
               +String(deviceKey)
               +"&_sort=id"
               +"&_order=DESC"
               +"&_end=3";
               
  //Serial.print("url:");Serial.println(url);
  
  char body[1024];
  httpGet(url, body, 1024);

  //Serial.println("body:");Serial.println(body);
  Serial.println("body size:");Serial.println( String(body).length() );
    
  //TODO:JSONでパースして内容を取得
  StaticJsonBuffer<1024> jsonBuffer;
  
  JsonArray &array = jsonBuffer.parseArray(body);
  Serial.print("json size:");Serial.println(array.size());
  
  for(int i=0;i<array.size();i++){
    //Slaveは3個まで
    if(i > SLAVE_COUNT) break;
    JsonObject &iftttObj = array[i];

    deviceIfttt[i].deviceKey     = iftttObj["deviceKey"];
    deviceIfttt[i].deviceSlaveId = iftttObj["deviceSlaveId"];
    deviceIfttt[i].secretKey     = iftttObj["secretKey"];
    deviceIfttt[i].event         = iftttObj["event"];
    deviceIfttt[i].actionType    = iftttObj["actionType"];

    //Serial.print("deviceSlaveId:"); Serial.println( deviceIfttt[i].deviceSlaveId );
  }
}

//GETリクエストをしてbody部分を受信
void CogleMasterConfig::httpGet(String url,char* body,int body_size){
  client.print(String("GET ")
                + url
                + " HTTP/1.1\r\n" 
                + "Host: " + COFIG_SERVER_API_HOST + "\r\n"
                + "Connection: close\r\n\r\n");
  delay(100);
  
  while(client.available()){
    String line = client.readStringUntil('\r');
    line.trim();
    //Serial.println( "line:"+line );
    if(line.length() == 0){
        //空行の後がbody
        String resp = "";
        while(client.available()){
          resp += client.readStringUntil('\r');
        }
        resp.trim();
        resp.toCharArray(body,body_size);
    }
  }      
}


//アドレスに一致した IFTTT 設定を読み出す
CogleDeviceIFTTT CogleMasterConfig::getDeviceIFTTT( String slaveAddrHex ){
  CogleDeviceIFTTT result;
  
  //Serial.print("slaveAddrHex:"); Serial.println( slaveAddrHex );
  for(int i=0;i<SLAVE_COUNT;i++){
      //Serial.print("deviceSlaveId:"); Serial.println( deviceIfttt[i].deviceSlaveId );
      if(deviceIfttt[i].deviceSlaveId == slaveAddrHex){
        result = deviceIfttt[i];
        break;
      }
  }
  
  return result;
}


