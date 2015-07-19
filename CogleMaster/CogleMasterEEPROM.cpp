/** CogleMasterEEPROM.cpp
EEPROMに保存する値のアドレス等をまとめる

*/

#include "CogleMasterEEPROM.h"

//EEPROMの記録先アドレス
#define COGLEMASTER_EEPROM_ADDR_CONFIG 0
#define COGLEMASTER_EEPROM_ADDR_CONFIG_SSID 1
#define COGLEMASTER_EEPROM_ADDR_CONFIG_PASSWORD 31
#define COGLEMASTER_EEPROM_ADDR_CONFIG_DEVICE_KEY 81

CogleMasterEEPROM::CogleMasterEEPROM(){

}

CogleMasterEEPROM::~CogleMasterEEPROM(){
  end();
}
    
//初期化
void CogleMasterEEPROM::begin(const int rom_size){
  this->rom_size = rom_size;
  EEPROM.begin(rom_size);
}
    
//終了処理
void CogleMasterEEPROM::end(void){
  EEPROM.end();
}


//設定情報を保存する
void CogleMasterEEPROM::config_save(const char* ssid,const char* password,const char* deviceKey){
  //最大 512バイト 設定を書き込む
  //TODO: パラメータのアドレスが固定なのは非効率なので、なんか考える
  
  //SSID 値を保存
  for(int i=0;i<COGLEMASTER_EEPROM_ADDR_CONFIG_SSID_SIZE;i++){
    EEPROM.write(COGLEMASTER_EEPROM_ADDR_CONFIG_SSID+i, ssid[i]);  
  }
  
  //パスワードを保存
  for(int i=0;i<COGLEMASTER_EEPROM_ADDR_CONFIG_PASSWORD_SIZE;i++){
    EEPROM.write(COGLEMASTER_EEPROM_ADDR_CONFIG_PASSWORD+i, password[i]);  
  }
  
  //デバイスキーを保存
  for(int i=0;i<COGLEMASTER_EEPROM_ADDR_CONFIG_DEVICE_KEY_SIZE;i++){
    EEPROM.write(COGLEMASTER_EEPROM_ADDR_CONFIG_DEVICE_KEY+i, deviceKey[i]);  
  }
  
  //記録した事を保存
  EEPROM.write(COGLEMASTER_EEPROM_ADDR_CONFIG, (byte)1);
  
  EEPROM.commit();
}

//設定情報を読み出す
void CogleMasterEEPROM::config_load(char* ssid, char* password, char* deviceKey){
  {
    char tmp[COGLEMASTER_EEPROM_ADDR_CONFIG_SSID_SIZE];
    for(int i=0;i<COGLEMASTER_EEPROM_ADDR_CONFIG_SSID_SIZE;i++){
       tmp[i] = EEPROM.read(COGLEMASTER_EEPROM_ADDR_CONFIG_SSID+i);
    }
    String t = String(tmp);
    //Serial.print("t:"); Serial.println(t);
    t.toCharArray(ssid,COGLEMASTER_EEPROM_ADDR_CONFIG_SSID_SIZE);
  }
  
  {
    char tmp[COGLEMASTER_EEPROM_ADDR_CONFIG_PASSWORD_SIZE];
    for(int i=0;i<COGLEMASTER_EEPROM_ADDR_CONFIG_PASSWORD_SIZE;i++){
       tmp[i] = EEPROM.read(COGLEMASTER_EEPROM_ADDR_CONFIG_PASSWORD+i);
    }
    String t = String(tmp);
    //Serial.print("t:"); Serial.println(t);
    t.toCharArray(password,COGLEMASTER_EEPROM_ADDR_CONFIG_PASSWORD_SIZE);
  }
    
  {
    char tmp[COGLEMASTER_EEPROM_ADDR_CONFIG_DEVICE_KEY_SIZE];
    for(int i=0;i<COGLEMASTER_EEPROM_ADDR_CONFIG_DEVICE_KEY_SIZE;i++){
       tmp[i] = EEPROM.read(COGLEMASTER_EEPROM_ADDR_CONFIG_DEVICE_KEY+i);
    }
    String t = String(tmp);
    //Serial.print("t:"); Serial.println(t);
    t.toCharArray(deviceKey,COGLEMASTER_EEPROM_ADDR_CONFIG_DEVICE_KEY_SIZE);
  }
  
  /*  
  Serial.println("config_load");
  Serial.print("ssid:"); Serial.println(ssid);
  Serial.print("password:"); Serial.println(password);
  Serial.print("deviceKey:"); Serial.println(deviceKey);
  */
}


//設定情報を消去
void CogleMasterEEPROM::config_clear(void){
  for (int i = 0; i < rom_size; i++)
    EEPROM.write(i, 0);
  EEPROM.commit();    
}
