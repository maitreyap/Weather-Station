/**************************************************
* Weather Station
*
* Author: Maitreya Panse
* 
*
* This is a simple program that samples Temperature,
* Humidity, and Soil Moisture Data and 
* sends it to ThingSpeak over Wifi. 
*
* 24/09/2019 - Version 1.0 - Otter
*   Initial release
**************************************************/

/***************************************************/
/*Include Section*/

#include <ESP8266WiFi.h>
#include <DHT.h>
#include <DHT_U.h>
/***************************************************/

/***************************************************/
/*Defines Section */

// DHT22 (Temp Sensor) Initializations
#define DHTPIN D7     // DATA Pin connection
#define DHTTYPE DHT22   // DHT 22  Temperature Sensor 
/***************************************************/

/***************************************************/
/*Global Variables Section */
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
//Temperature Sensor Variables
float TEMP;
float HUM;
//Soil Moisture Variables
const int analogPin = A0;
int moistureSense = 0;
int moistureSenseOut = 0;
//Wifi Variables
String APIKey = "YOURAPIKEY";
const char* ssid = "SSID";
const char* pwd = "PWD";
const char* server = "184.106.153.149";
WiFiClient client;
/***************************************************/

void setup() {
  Serial.begin(9600);
  
  pinMode(D2,OUTPUT);
  dht.begin();
  Serial.println("Waking up....");
  delay(5000);
  
  if(WiFi.status() != WL_CONNECTED)
  {
     WiFi.mode(WIFI_STA);
     Serial.println("Wifi Not Connected");
     ConnectWifi();
  } 
//  WiFi.mode(WIFI_STA);
//  WiFi.disconnect();
//  delay(500); 
//  FindWiFiNetworks();
//  ConnectWifi();
  

 

}

void loop() {

  
  digitalWrite(D2, HIGH);
  delay(2000);
  Serial.println(" ");
  Serial.println("New Cycle");
  GetTemperatureData();
  GetMoistureSensorData();
  SendData(); 
  digitalWrite(D2, LOW);
  delay(500);
  //ESP.deepSleep(900e6); 
  ESP.deepSleep(30e6);
   

  
}

/**************************************************
* FindWiFiNetworks  : void FindWiFiNetworks()
* Created by    : Maitreya Panse
* Date created    : 05/08/2019
* Description   : List all visible WiFi Networks
**************************************************/
void FindWiFiNetworks()
{
  int count = WiFi.scanNetworks();
  Serial.println("Scanning in Progress:");
  while(WiFi.scanComplete() == -1)
  {
    Serial.print(".");
    delay(1000);
  
  }

  Serial.println();
  Serial.print("Number of discovered Networks: ");
  Serial.println(count);

  for (int i = 0; i < count; i++)
  {
    Serial.print(i+1);
    Serial.print("\t");
    
    Serial.print("Name: ");
    Serial.print(WiFi.SSID(i));
    Serial.print("\t");

    Serial.print("Strength: ");
    Serial.println(WiFi.RSSI(i));
  }
  
}

/**************************************************
* ConnectWiFi  : void ConnectWifi()
* Created by    : Maitreya Panse
* Date created    : 05/08/2019
* Description   : Connect to specified Wifi Network
**************************************************/
void ConnectWifi()
{
  WiFi.begin(ssid, pwd);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("CNCT, IP address: ");
  Serial.println(WiFi.localIP());
}

/**************************************************
* GetTemperatureData  : void GetTemperatureData()
* Created by    : Maitreya Panse
* Date created    : 05/08/2019
* Description   : Function gets temperature and humidity data from DHT22 sensor
**************************************************/
void GetTemperatureData()
{

   HUM = dht.readHumidity();
   Serial.print("Humidity:");
   Serial.print(HUM);
   Serial.println(" % ");
   delay(3000);
   TEMP = dht.readTemperature();
   Serial.print("Temperature:");
   Serial.print(TEMP);
   Serial.println(" Degrees C");
}

/**************************************************
* GetMoistureSensorDara  : void GetMoistureSensorData()
* Created by    : Maitreya Panse
* Date created    : 05/08/2019
* Description   : Samples analog input to get Moisture Sensor Data
**************************************************/
void GetMoistureSensorData()
{
   moistureSense = analogRead(analogPin);
   moistureSenseOut = map(moistureSense,510,860,100,0);
   Serial.print("Sensor : ");
   Serial.print(moistureSense);
   

   Serial.print("\t CompensatedSensor: ");
   Serial.println(moistureSenseOut);
  
}

/**************************************************
* SendData  : void SendData()
* Created by    : Maitreya Panse
* Date created    : 05/08/2019
* Description   : Sends Data to ThingSpeak using WiFi
**************************************************/
void SendData()
{
  if(client.connect(server,80))
{
  String postStr = APIKey;
  postStr +="&field1=";
  postStr += String(TEMP);
  postStr +="&field2=";
  postStr += String(HUM);
  postStr +="&field3=";
  postStr += String(moistureSense);
  postStr += "\r\n\r\n";
  client.print("POST /update HTTP/1.1\n");
  client.print("Host: api.thingspeak.com\n");
  client.print("Connection: close\n");
  client.print("X-THINGSPEAKAPIKEY: "+APIKey+"\n");
  client.print("Content-Type: application/x-www-form-urlencoded\n");
  client.print("Content-Length: ");
  client.print(postStr.length());
  client.print("\n\n");
  client.print(postStr);
}

 client.stop();
 Serial.println("Waiting...");
}
