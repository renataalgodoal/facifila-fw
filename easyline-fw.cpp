#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>



// ############# VARIAVEIS ############### //

const char* SSID = "MARIA_WIFI"; // rede wifi
const char* PASSWORD = "sissi03021961"; // senha da rede wifi
String BASE_URL = "http://192.168.0.101:3000/";


String appointmentId ="";
int  serIn;             
char serInString[100];                         
int  serInIndx  = 0;    
int  serOutIndx = 0; 


// ############### OBJETOS ################# //

WiFiClient client;
HTTPClient http;
LiquidCrystal_I2C lcd(0x27,16,2);



// ############# FUNÇÕES ############### //

void lcdConnected() {
  lcd.setBacklight(HIGH);
  lcd.setCursor(0,0);
  lcd.print("CONECTADO   ");
  lcd.setCursor(0,1);
  lcd.print("A REDE        ");
  delay(1000);

}

void lcdError() {
  lcd.setBacklight(HIGH);
  lcd.setCursor(0,0);
  lcd.print("ERRO");
  lcd.setCursor(0,1);
  lcd.print("");
  delay(1000);
}

void lcdSuccess() {
  lcd.setBacklight(HIGH);
  lcd.setCursor(0,0);
  lcd.print("STATUS");
  lcd.setCursor(0,1);
  lcd.print("UPDATED     ");
  delay(1000);
}

void lcdWaiting() {
  lcd.setBacklight(HIGH);
  lcd.setCursor(0,0);
  lcd.print("PRONTO");
  lcd.setCursor(0,1);
  lcd.print("PARA LEITURA");
  delay(1000);
}



void readSerialString () {
    int sb;   
    if(Serial.available()) { 
       while (Serial.available()){ 
          sb = Serial.read();   
          if(sb <= 32) continue;          
          serInString[serInIndx] = sb;
          serInIndx++;
       }
       //serInString[serInIndx] = '\0';
        serInIndx = 0;
       appointmentId = serInString;
       Serial.println(appointmentId);
    }  
}

void initWiFi() {
  delay(10);
  Serial.println("Conectando-se em: " + String(SSID));

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  //Serial.println();
  Serial.print("Conectado na Rede " + String(SSID) + " | IP => ");
  Serial.println(WiFi.localIP());
  lcdConnected()
}




String makeRequest(String path)
{
  String URLrequest = BASE_URL + path;
  http.begin(URLrequest);
  Serial.println(URLrequest);
  //Serial.println(BASE_URL + path);
  http.addHeader("content-type", "application/json");

  String body = "{\"status\":\"confirmed\"}";

  int httpCode = http.PUT(body);
  //Serial.println(httpCode);

  if (httpCode < 0) {
    Serial.println("request error - " + httpCode);
    return "";

  }

  if (httpCode != HTTP_CODE_OK) {
    return "";
  }

  String response =  http.getString();
  http.end();

  return response;
}

void httpRequest(String path){
  String payload = makeRequest(path);
 
  if (payload != "") {
    return;
  }


}

// ############## SETUP ################# //

void setup() {
   Serial.begin(9600);
   lcd.init();
   initWiFi();

}


// ############### LOOP ################# //

void loop() {
  
  readSerialString();
  Serial.println("appointmentId = " + appointmentId);

   if (appointmentId != "") {
  //String appointmentId = "366c0810-2457-49bd-a5af-e2387ea045ca";


  Serial.println(appointmentId);
  
  String path = "appointments/" + appointmentId;

  Serial.println(path);
  httpRequest(path);
  appointmentId = "";

  lcdSuccess();
  
  }
  
  memset(serInString, 0, sizeof(serInString));

  lcdWaiting();

  delay(2000);
}