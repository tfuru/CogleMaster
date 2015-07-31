/** CogleMasterIFTTT.h
IFTTT関連の処理
*/
#ifndef CogleMasterIFTTT_h
#define CogleMasterIFTTT_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>
#include "CogleMasterConfig.h"

//IFTTT アクションの種類 
#define ACTION_TYPE_HUE_CHANGE_COLOR          "hue_change_color"
#define ACTION_TYPE_HUE_DIM_LIGHTS            "hue_dim_lights"
#define ACTION_TYPE_HUE_TOGGLE_LIGHTS_ON_OFF  "hue_toggle_lights_on_off"
#define ACTION_TYPE_IRKIT_ACTION              "irkit_action"
#define ACTION_TYPE_GENERAL_ACTION            "general_action"

class CogleMasterIFTTT {

  private:
    WiFiClient client;

    //処理済みの汎用アクション
    char oldHueChangeColorValues[8];
    char oldGeneralActionValues[8];
    int oldHueDimLightsValues;
    int oldHueToggleLightsOnOffValues;
    
    //Hueの色変更
    bool actionHueChangeColor(CogleDeviceIFTTT* deviceIfttt,const byte aioArry[]);
    //Hueの調光
    bool actionHueDimLights(CogleDeviceIFTTT* deviceIfttt,const byte aioArry[]);
    //HueのON,OFF
    bool actionHueToggleLightsOnOff(CogleDeviceIFTTT* deviceIfttt,const byte aioArry[]);
    //IRkitのアクション
    bool actionIRkitAction(CogleDeviceIFTTT* deviceIfttt,const byte aioArry[]);
    //汎用アクション
    bool actionGeneralAction(CogleDeviceIFTTT* deviceIfttt,const byte aioArry[]);

    //maker channel へパラメータをPOSTする
    void postAction(CogleDeviceIFTTT* deviceIfttt,const String value[]);
        
  public:
    CogleMasterIFTTT();
    ~CogleMasterIFTTT();
    
    //初期化
    void begin(void);
    
    //終了処理
    void end(void);

    //アクションの実行
    void action(CogleDeviceIFTTT* deviceIfttt,const byte aioArry[]);
};

#endif /* CogleMasterIFTTT_h */
