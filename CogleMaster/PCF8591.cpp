/**
 PCF8591.cpp
 PCF8591 I2Cインターフェース8ビットA/D & D/Aコンバータチップを利用する為のArduino用クラス
 
*/

#include "PCF8591.h"

//コンストラクタ
PCF8591::PCF8591(){
  
}

//デストラクタ
PCF8591::~PCF8591(){
  end();
}

//i2c 初期処理
void PCF8591::begin(int sda, int scl){
  Wire.begin(sda,scl);
}

//終了処理
void PCF8591::end(void){

}

//バイトデータ読み込み
byte PCF8591::read(const int i2cAddr,const int adc){
  byte result = 0;

  Wire.beginTransmission(i2cAddr);
  Wire.write(adc);
  Wire.endTransmission();
  delay(100);
  
  Wire.requestFrom(i2cAddr, 2);
  delay(100);
  while(Wire.available())
  {
    result = Wire.read();
    //Serial.print("read:");Serial.println(result,HEX);
  }
  //Serial.print("result:");Serial.println(result);

  return result;
}
