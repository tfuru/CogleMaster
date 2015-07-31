/** CogleMasterIFTTT.cpp
IFTTT関連の処理
*/

#include "CogleMasterIFTTT.h"

//IFTTTのサーバの情報
#define IFTTT_SERVER_HOST "maker.ifttt.com"
#define IFTTT_SERVER_PORT 80

CogleMasterIFTTT::CogleMasterIFTTT(){

}

CogleMasterIFTTT::~CogleMasterIFTTT(){
  end();
}
    
//初期化
void CogleMasterIFTTT::begin(void){
  sprintf(oldGeneralActionValues,"#%02X%02X%02X",0,0,0);
  sprintf(oldHueChangeColorValues,"#%02X%02X%02X",0,0,0);  
}
    
//終了処理
void CogleMasterIFTTT::end(void){

}

//maker channel へパラメータをPOSTする
void CogleMasterIFTTT::postAction(CogleDeviceIFTTT* deviceIfttt,const String values[]){
  Serial.println( "postAction" );
  
  if (!client.connect(IFTTT_SERVER_HOST, IFTTT_SERVER_PORT)) {
    //サーバの接続に失敗
    Serial.println( String("Connect failure ") + String(IFTTT_SERVER_HOST) + " "+ String(IFTTT_SERVER_PORT) );
    return;
  }
  delay(100);
  
  String url = "/trigger/"+ deviceIfttt->event + "/with/key/" + deviceIfttt->secretKey;
  //Serial.print( "url:" );Serial.println( url );

  //送信する為のJSONを作成
  char tmp[100];
  sprintf(tmp,"{\"value1\":\"%s\",\"value2\":\"%s\",\"value3\":\"%s\"}",values[0].c_str(),values[1].c_str(),values[2].c_str());
  String json = String(tmp);
  
  //POST 
  //Content-Length 必須
  String request = String("POST ") + url + " HTTP/1.1\r\n" 
                + "Host: " + IFTTT_SERVER_HOST + "\r\n"
                + "Content-Type: application/json\r\n"
                + "Content-Length: "+json.length()+"\r\n"
                + "\r\n"
                + String(json);
  
  //Serial.println( "request:" );Serial.println( request );
       
  client.print(request);
                  
  delay(300);
  
  while(client.available()){
    String line = client.readStringUntil('\r');
    line.trim();
    if(line.length() == 0){
        //空行の後がbody
        String resp = "";
        while(client.available()){
          resp += client.readStringUntil('\r');
        }
        resp.trim();
        //Serial.println( resp );
    }
  }
  client.stop();
}

//アクションの実行
void CogleMasterIFTTT::action(CogleDeviceIFTTT* deviceIfttt,const byte aioArry[]){
  
  //actionTypeが一致するactionを実行する
  bool flg = false;
  if(flg == false) flg = actionHueChangeColor(deviceIfttt,aioArry);
  if(flg == false) flg = actionHueDimLights(deviceIfttt,aioArry);
  if(flg == false) flg = actionHueToggleLightsOnOff(deviceIfttt,aioArry);
  if(flg == false) flg = actionIRkitAction(deviceIfttt,aioArry);
  if(flg == false) flg = actionGeneralAction(deviceIfttt,aioArry);

  if(flg == true){
    //次のaction実行まで間をあける
    delay(3000);
  }
}

//Hueの色変更
bool CogleMasterIFTTT::actionHueChangeColor(CogleDeviceIFTTT* deviceIfttt,const byte aioArry[]){
  String actionType = deviceIfttt->actionType;
  if(actionType.equals(ACTION_TYPE_HUE_CHANGE_COLOR) == false){
    //一致しないので falseを返す
    return false;  
  }
      
  byte r = (aioArry[0] < 20)?0:aioArry[0];
  byte g = (aioArry[1] < 20)?0:aioArry[1];
  byte b = (aioArry[2] < 20)?0:aioArry[2];

  byte dim = (aioArry[3] < 20)?0:aioArry[3];
   
  char v1[8];
  sprintf(v1,"#%02X%02X%02X",r,g,b);
  if(strcmp(v1,oldHueChangeColorValues) == 0){
      //前回値と同じだったので何もしない
      return false;  
  }
  sprintf(oldHueChangeColorValues,"%s",v1);
  
  String values[3];
  values[0] = String(v1);
  values[1] = "";
  values[2] = "";
  //Serial.println( "value1:"+values[0] );

  //アクション実行
  postAction(deviceIfttt,values);

  //調光 アクション実行
  CogleDeviceIFTTT deviceIfttt2;
  deviceIfttt2.deviceKey     = deviceIfttt->deviceKey;
  deviceIfttt2.deviceSlaveId = deviceIfttt->deviceSlaveId;
  deviceIfttt2.secretKey     = deviceIfttt->secretKey;
  deviceIfttt2.event         = "cogle_hue_dim_lights_trigger";
  deviceIfttt2.actionType    = "hue_dim_lights";

  byte aio2Arry[1];
  aio2Arry[0] = dim;
  actionHueDimLights(&deviceIfttt2,aio2Arry);
  
  return true;      
}

//Hueの調光
bool CogleMasterIFTTT::actionHueDimLights(CogleDeviceIFTTT* deviceIfttt,const byte aioArry[]){
  String actionType = deviceIfttt->actionType;
  if(actionType.equals(ACTION_TYPE_HUE_DIM_LIGHTS)== false){
    //一致しないので falseを返す
    return false;  
  }

  int v = map(aioArry[0], 0, 255, 0, 100);
  if(v == oldHueDimLightsValues){
      //前回値と同じだったので何もしない
      return false;  
  }
  oldHueDimLightsValues = v;
  
  String values[3];
  values[0] = String(v);
  values[1] = "";
  values[2] = "";

  //アクション実行
  postAction(deviceIfttt,values);
  
  return true;      
}

//HueのON,OFF
bool CogleMasterIFTTT::actionHueToggleLightsOnOff(CogleDeviceIFTTT* deviceIfttt,const byte aioArry[]){
  String actionType = deviceIfttt->actionType;
  if(actionType.equals(ACTION_TYPE_HUE_TOGGLE_LIGHTS_ON_OFF)== false){
    //一致しないので falseを返す
    return false;  
  }

/*
  int v = map(aioArry[0], 0, 256, 0, 100);
  if(v == oldHueToggleLightsOnOffValues){
      //前回値と同じだったので何もしない
      return true;  
  }
  oldHueToggleLightsOnOffValues = v;
*/
  
  String values[3];
  values[0] = "";
  values[1] = "";
  values[2] = "";

  //アクション実行
  postAction(deviceIfttt,values);
  
  return true;      
}

//IRkitアクション
bool CogleMasterIFTTT::actionIRkitAction(CogleDeviceIFTTT* deviceIfttt,const byte aioArry[]){
  String actionType = deviceIfttt->actionType;
  if(actionType.equals(ACTION_TYPE_IRKIT_ACTION)== false){
    //一致しないので falseを返す
    return false;  
  }

  String values[3];
  values[0] = "";
  values[1] = "";
  values[2] = "";

  //アクション実行
  if(aioArry[0] > 30){
    deviceIfttt->event = "cogle_irkit_0";
    postAction(deviceIfttt,values);
  }

  //アクション実行
  if(aioArry[1] > 30){
    deviceIfttt->event = "cogle_irkit_1";
    postAction(deviceIfttt,values);
  }
  
  //アクション実行
  if(aioArry[2] > 30){
    deviceIfttt->event = "cogle_irkit_2";
    postAction(deviceIfttt,values);
  }

  //アクション実行
  if(aioArry[3] > 30){
    deviceIfttt->event = "cogle_irkit_3";
    postAction(deviceIfttt,values);
  }
  
  return true;      
}


//汎用アクション
bool CogleMasterIFTTT::actionGeneralAction(CogleDeviceIFTTT* deviceIfttt,const byte aioArry[]){
  String actionType = deviceIfttt->actionType;
  if(actionType.equals(ACTION_TYPE_GENERAL_ACTION)== false){
    //一致しないので falseを返す
    return false;  
  }

  //Serial.println( "old1:"+String(oldGeneralActionValues) );

  char tmp[8];
  sprintf(tmp,"#%02X%02X%02X",aioArry[0],aioArry[1],aioArry[2]);
  //Serial.println( "tmp:"+String(tmp) );
  if(strcmp(tmp,oldGeneralActionValues) == 0){
      //前回値と同じだったので何もしない
      return false;  
  }
  sprintf(oldGeneralActionValues,"%s",tmp);

  //Serial.println( "old2:"+String(oldGeneralActionValues) );
  
  String values[3];
  values[0] = String(aioArry[0],DEC);
  values[1] = String(aioArry[1],DEC);
  values[2] = String(aioArry[2],DEC);
  
  //アクション実行
  postAction(deviceIfttt, values);
  
  return true;      
}
  

