/** CogleMasterEEPROM.h
EEPROMに保存する値のアドレス等をまとめる

*/
#ifndef CogleMasterEEPROM_h
#define CogleMasterEEPROM_h

#include <Arduino.h>
#include <EEPROM.h>

//各パラメータのサイズ
#define COGLEMASTER_EEPROM_ADDR_CONFIG_SSID_SIZE 30
#define COGLEMASTER_EEPROM_ADDR_CONFIG_PASSWORD_SIZE 30
#define COGLEMASTER_EEPROM_ADDR_CONFIG_DEVICE_KEY_SIZE 16

class CogleMasterEEPROM {
  private:
    int rom_size;
    
  public:
    CogleMasterEEPROM();
    ~CogleMasterEEPROM();
    
    //初期化
    void begin(const int rom_size);
    
    //終了処理
    void end(void);
    
    //設定情報を保存する
    void config_save(const char* ssid,const char* password,const char* deviceKey);
    
    //設定情報を読み出す
    void config_load( char* ssid, char* password, char* deviceKey);
  
    //設定情報を消去
    void config_clear(void);  
};


#endif /* CogleMasterEEPROM_h */
