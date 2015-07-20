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
  Wire.setClock(10000);
  
  //CogleSlaveの利用開始
  cogleSlave.begin(sdaPin,sclPin);
  delay(10);
  
  // Wi-Fi 設定を開始
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.print("localIP:"); Serial.println(WiFi.localIP());
  //TODO:リフレクタ サーバへ登録する

  //設定をEEPROMに記録しているので読み出し用
  cogleEeprom.begin(512);
  
  //設定値を読み出し deviceKeyを取得
  char staSsid[COGLEMASTER_EEPROM_ADDR_CONFIG_SSID_SIZE];
  char staPassword[COGLEMASTER_EEPROM_ADDR_CONFIG_PASSWORD_SIZE];
  char deviceKey[COGLEMASTER_EEPROM_ADDR_CONFIG_DEVICE_KEY_SIZE];
  cogleEeprom.config_load(staSsid,staPassword,deviceKey);

  //サーバにアクセスして 保存情報を取得
  cogleConfig.begin(deviceKey);
  cogleConfig.loadDeviceIFTTT();
}

//終了処理
void CogleMasterSTA::end(void){

}

//センサー値の送信
void CogleMasterSTA::send(const int cogleSlaveAddr){
  //サーバに保存さていてロード済みの IFTTT設定を読み出す
  String slaveAddrHex = "0x"+String((cogleSlaveAddr<<1),HEX);
  CogleDeviceIFTTT ifttt = cogleConfig.getDeviceIFTTT( slaveAddrHex );
  if(ifttt.deviceKey.length() == 0){
    //IFTTT設定がない場合は 以下の処理はしない
    return;
  }
  
  //Serial.print( "ifttt event:" ); Serial.println( ifttt.event );
  
  //CogleSlaveの値の読み出し
  byte a0 = cogleSlave.read(cogleSlaveAddr,PCF8591_ADC0);
  byte a1 = cogleSlave.read(cogleSlaveAddr,PCF8591_ADC1);
  byte a2 = cogleSlave.read(cogleSlaveAddr,PCF8591_ADC2);
  byte a3 = cogleSlave.read(cogleSlaveAddr,PCF8591_ADC3);

  Serial.print( "cogleSlaveAddr:" ); Serial.println( slaveAddrHex );
  Serial.print( " a0:" ); Serial.println( a0 );
  Serial.print( " a1:" ); Serial.println( a1 );
  Serial.print( " a2:" ); Serial.println( a2 );
  Serial.print( " a3:" ); Serial.println( a3 );
  Serial.println( "" );
  
  //TODO: サーバにデバイス識別キーとcogleSlaveAddr と一緒に送信
}
