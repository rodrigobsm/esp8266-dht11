#include<stdlib.h>
#include <SoftwareSerial.h>
#include "DHT.h"

#define SSID "YourWifiSSID"
#define PASS "YourWifiPasswd"
#define IP "192.168.1.111" // thingspeak.com
String GET = "GET /update.php?key=C2UPN80ITNARQTO8&field1="; 
SoftwareSerial monitor(2, 3); // RX, TX

#define DHTPIN 8     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11 
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  monitor.begin(9600);
  Serial.begin(9600);
  sendDebug("AT");
  delay(5000);
  if(monitor.find("OK")){
    Serial.println("RECEIVED: OK");
    connectWiFi();
  }

  // inicia dht11
  dht.begin();
}

void loop(){

  // le umidade e temperatura
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float tempC = dht.readTemperature();
  
  char buffer[10];
  String tempCStr = dtostrf(tempC, 0, 0, buffer);
  
  char buffer2[10];
  String humidade = dtostrf(h, 0, 0, buffer2);
  
  updateTemp(tempCStr, humidade);

  // publica temperatura a cada 6 segundos
  delay(6000);
}

void updateTemp(String tenmpF, String umidade) {

// AT+CIPSTART="TCP","184.106.153.149",80
  
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80\r\n";
  sendDebug(cmd);
  delay(2000);
  if(monitor.find("Error")){
    Serial.print("RECEIVED: Error");
    return;
  }
  cmd = GET;
  cmd += tenmpF;
  cmd += "&field2=";
  cmd += umidade;
  cmd += "\r\n";

  String cipsendcmd = "AT+CIPSEND=";
  cipsendcmd += (cmd.length());
  cipsendcmd += "\r\n";
  monitor.print(cipsendcmd); 
  if(monitor.find(">")){
    Serial.print(">");
    Serial.print(cmd);
    monitor.print(cmd);
  }else{
    sendDebug("AT+CIPCLOSE");
  }

  if(monitor.find("OK")){
    Serial.println("RECEIVED: OK");
    sendDebug("AT+CIPCLOSE");
  }else{
    Serial.println("RECEIVED: Error");
  }
}
void sendDebug(String cmd){
  Serial.print("SEND: ");
  Serial.println(cmd);
  monitor.println(cmd);
} 
 
boolean connectWiFi(){
  monitor.println("AT+CWMODE=");
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  sendDebug(cmd);
  delay(5000);
  if(monitor.find("OK")){
    Serial.println("RECEIVED: WIFI CONNECTION OK");
    return true;
  }else{
    Serial.println("RECEIVED: WIFI CONNECTION ERROR");
    return false;
  }
}
