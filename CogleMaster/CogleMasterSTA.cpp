/** CogleMasterSTA.cpp
ステーションモードで起動する場合に利用

*/
#include "CogleMasterSTA.h"

CogleMasterSTA::CogleMasterSTA(){
  
}

CogleMasterSTA:: ~CogleMasterSTA(){
  end();
}

//初期化
void CogleMasterSTA::begin(const int sdaPin,const int sclPin,const char* ssid,const char* password){
  //ESP-01用 i2cピン設定
  Wire.pins(sdaPin,sclPin);
  Wire.setClock(10000);
  
  //CogleSlaveの利用開始
  cogleSlave.begin();
  delay(10);
  
  // Wi-Fi 設定を開始
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  Serial.print("localIP:"); Serial.println(WiFi.localIP());
  //TODO:リフレクタ サーバへ登録する
}

//終了処理
void CogleMasterSTA::end(void){

}

//センサー値の送信
void CogleMasterSTA::send(const int cogleSlaveAddr){
  //CogleSlaveの値の読み出し
  byte a0 = cogleSlave.read(cogleSlaveAddr,PCF8591_ADC0);
  byte a1 = cogleSlave.read(cogleSlaveAddr,PCF8591_ADC1);
  byte a2 = cogleSlave.read(cogleSlaveAddr,PCF8591_ADC2);
  byte a3 = cogleSlave.read(cogleSlaveAddr,PCF8591_ADC3);

  Serial.print( "a0:" ); Serial.println( a0 );
  
  //TODO: サーバにデバイス識別キーとcogleSlaveAddr と一緒に送信
  
}
