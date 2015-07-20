/** CogleMasterAP.cpp
アクセスポイントモードで起動する場合に利用

*/
#include "CogleMasterAP.h"

//各APIのURLを記載
#define API_URL_CONFIG_RESET "/api/v1/config_reset"
#define API_URL_CONFIG_SAVE "/api/v1/config_save"

CogleMasterAP::CogleMasterAP():apiServer(80){
  
}

CogleMasterAP:: ~CogleMasterAP(){
  end();
}

void CogleMasterAP::end(void){
  cogleEeprom.end();
}
    
//初期化
void CogleMasterAP::begin(IPAddress localIp, IPAddress subnet,const char* dnsDomain,const char* ssid,const char* password){
  //設定記録のためEEPROMを利用
  cogleEeprom.begin(512);
  
  // Wi-Fi をアクセスポイント設定で初期化
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password); 
  WiFi.config(localIp, localIp, subnet);  
  
  //DNSサーバを起動
  if (!mdns.begin(dnsDomain, WiFi.localIP())) {
    Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
 
  //設定等を登録するAPI Web Serverを起動
  apiServer.begin();
}

//接続クライアント待ち
WiFiClient CogleMasterAP::clientAvailable(void){
  //DNS情報の更新
  mdns.update();
  
  WiFiClient client = apiServer.available();
  
  return client;
}

//リクエスト待ち&処理
bool CogleMasterAP::receiveRequest( void ){
  WiFiClient client = clientAvailable();
  if(!client) return false;
  
  //リクエストURLを取得
  String reqURL = getRequestURL( client.readStringUntil('\r') );
  //client.flush(); 
  if (reqURL == 0) {
    //何らかの
    Serial.print("Invalid request: "); Serial.println(reqURL);
    //エラーをクライアントへ通知
    String s = createHttpHeader(500,"Internal Server Error","text/html");
    client.flush();
    client.print(s);
    return true;
  }
  Serial.print("RequestURL: "); Serial.println(reqURL);
  
  //レスポンスを返す
  String resp = "";
  
  //TODO:各URLにあったAPIを実装する
  if (resp.length() == 0) resp = onReqSampleApi(client,reqURL);
  if (resp.length() == 0) resp = onReqConfigResetApi(client,reqURL);
  if (resp.length() == 0) resp = onReqConfigSaveApi(client,reqURL);
  
  if (resp == 0){
   //一致するAPIが無かった場合 404 を返す
   resp =  createHttpHeader(404,"Not Found","application/json");
   resp += "{\"status\":404,\"message\":\"Not Found\"}";
  }
  client.flush();
  client.print(resp);
  
  return true;
}

//リクエストURLを抽出する
String CogleMasterAP::getRequestURL(const String request){
  String req;
  int addr_start = request.indexOf(' ');
  int addr_end = request.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1) {
    //何らかの
    Serial.print("Invalid request: ");
    Serial.println(req);
    return "";
  }
  req = request.substring(addr_start + 1, addr_end);
  return req;
}

//Httpヘッダーを生成して返す
//TODO: あとで別ファイル化が必要
String CogleMasterAP::createHttpHeader(const int code,const String message,const String contentType){
  String header;
  
  header =  "HTTP/1.1 "+String(code)+" "+message+"\r\n";
  header += "Content-Type: "+contentType+"\r\n";
  header += "\r\n";
   
  return header;
}

String CogleMasterAP::getStringPartByNr(String data, char separator, int index) {
    int stringData = 0;        //variable to count data part nr 
    String dataPart = "";      //variable to hole the return text

    for(int i = 0; i<data.length(); i++) {    //Walk through the text one letter at a time

        if(data[i]==separator) {
            //Count the number of times separator character appears in the text
            stringData++;

        }else if(stringData==index) {
            //get the text when separator is the rignt one
            dataPart.concat(data[i]);

        }else if(stringData>index) {
            //return text and stop if the next separator appears - to save CPU-time
            return dataPart;
            break;

        }

    }
    //return text if this is the last part
    return dataPart;
}

//http POSTリクエストのパラメータ抽出
//TODO: パラメータ順が固定になっているので改善ずる必要がある
void CogleMasterAP::parsePostRequest(String body,String keys[],String params[],int len){
  
    for(int i=0;i<len;i++){
      params[i] = getStringPartByNr(body,'&',i);
      params[i].replace(keys[i]+"=","");
      params[i].replace("+"," ");
      params[i].trim();
    }
    
}

//サンプル APIの処理
String CogleMasterAP::onReqSampleApi(WiFiClient client,const String path){
  if(path != "/api/v1/sample"){
    return "";
  }
  
  //設定を書き込んだ事を EEPROMに記録
  //EEPROM.write(COGLEMASTERAP_EEPROM_ADDR_SAMPLE, (byte)1);
  //EEPROM.commit();
  
  String resp;
  
  //ヘッダー部分 生成
  resp = createHttpHeader(200,"OK","application/json");
    
    //body部分
  resp += "{\"status\":200,\"message\":\"sample\"}";

  return resp;
}

//設定値の保存 APIの処理
String CogleMasterAP::onReqConfigSaveApi(WiFiClient client,const String path){
  if(path != API_URL_CONFIG_SAVE){
    return "";
  }
  
  //TODO: POSTデータを受け取って値を取り出す
  char ssid[COGLEMASTER_EEPROM_ADDR_CONFIG_SSID_SIZE];
  char password[COGLEMASTER_EEPROM_ADDR_CONFIG_PASSWORD_SIZE];
  char deviceKey[COGLEMASTER_EEPROM_ADDR_CONFIG_DEVICE_KEY_SIZE];

  //POSTデータ body部分を読み出す
  while(client.available()){
    String line = client.readStringUntil('\r');
    line.trim();
    //Serial.print("line [");Serial.print(line);Serial.println("]");
    
    //空行の後がbody要素
    if(line == ""){
      String body = client.readStringUntil('\r');
      Serial.print("body:"); Serial.println(body);
      
      //POSTパラメータを取り出す
      String keys[3] = {"ssid","password","deviceKey"};
      String params[3];
      parsePostRequest(body,keys,params,3);
 
      params[0].toCharArray(ssid,COGLEMASTER_EEPROM_ADDR_CONFIG_SSID_SIZE);
      params[1].toCharArray(password, COGLEMASTER_EEPROM_ADDR_CONFIG_PASSWORD_SIZE);
      params[2].toCharArray(deviceKey, COGLEMASTER_EEPROM_ADDR_CONFIG_DEVICE_KEY_SIZE);
      
      Serial.print("ssid:"); Serial.println(ssid);
      Serial.print("password:"); Serial.println(password);
      Serial.print("deviceKey:"); Serial.println(deviceKey);
    }
  }

  
  //設定を書き込む
  cogleEeprom.config_save(ssid,password,deviceKey);
  
  String resp;
  
  //ヘッダー部分 生成
  resp = createHttpHeader(200,"OK","application/json");
    
    //body部分
  resp += "{\"status\":200,\"message\":\"success\"}";

  return resp;
}

//設定値RESET APIの処理
String CogleMasterAP::onReqConfigResetApi(WiFiClient client,const String path){
  if(path != API_URL_CONFIG_RESET){
    return "";
  }
  
  String resp;
  
  //設定を書き込んだ事を EEPROMを初期化
  cogleEeprom.config_clear();
  
  //ヘッダー部分 生成
  resp = createHttpHeader(200,"OK","application/json");
    
    //body部分
  resp += "{\"status\":200,\"message\":\"success\"}";

  return resp;
}

