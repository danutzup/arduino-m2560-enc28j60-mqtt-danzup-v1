/*
enc28j60 mqtt  danzup arduino atmega2560 , BMP180 , tft lcd ili ILI9486 , AM2320
version 1.0
*/
#include <SFE_BMP180.h>
#include <Wire.h>
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
LCDWIKI_KBV mylcd(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset
//#include "DHT.h"
#include <UIPEthernet.h>
#include <UIPClient.h>
#include <PubSubClient.h>
#include "Adafruit_Sensor.h"
#include "Adafruit_AM2320.h"
//define some colour values
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
//#define DHTTYPE DHT22     // DHT 22  (AM2302), AM2321
SFE_BMP180 pressure;
#define ALTITUDE 65.0 // Altitude of Dumbrava in meters
Adafruit_AM2320 am2320 = Adafruit_AM2320();
//IP of my mosquito server ot receive mqqt messages
IPAddress server(192, 168, 1, 111);
uint8_t mac[6] = {0xA1,0xB1,0xC2,0xD3,0x84,0x95};
IPAddress ip(192, 168, 1, 112);      //this is ip address static
EthernetClient ethClient2;
PubSubClient client(ethClient2);
String out_temp;
String tempset;
float set_temp_r = 20;
long now = millis();
long lastMeasure = 0;
int lcd_show = 0;
long lastTime = 0;
int switchPIN1 = 29;           // pin switch +
int switchStatus1;            // status switch
int switchPIN2 = 27;           // pin switch -
int switchStatus2;            // status switch
float set_temp;

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message  is : ");
  String messageTemp;
    for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
    Serial.println();
    Serial.print("messageTemp:");
    Serial.println(messageTemp);

 if (strcmp(topic, "termostat/temperature") == 0)
 {
    tempset =  messageTemp;
    //mylcd.Print_String("         C", 0, 410);
    Serial.println("messageTemp");
    Serial.println(messageTemp);
    //mylcd.Print_String("Temperatura setata", 0, 370);
    //mylcd.Print_String(tempset, 0, 410);
    Serial.println("I am inside termostat/temperature topic");
    Serial.println(tempset);
    Serial.println("====================================");
 }
  if(strcmp(topic, "termostat/set_temperature") == 0)
  {
    //tempset =  messageTemp;
    Serial.println("====================================");
    Serial.println("I am inside termostat/set_temperature topic");
    Serial.println(messageTemp);
    Serial.println("====================================");
  }
   if (strcmp(topic, "afara/temperature") == 0)
   {
    out_temp =  messageTemp;
    Serial.println("====================================");
    Serial.println("I am inside afara/temperature topic");
    Serial.println(messageTemp);
    Serial.println("====================================");
  }

   //Serial.println();
}
//
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ethClient2")) {
      Serial.println("mqtt  connected");  
      client.subscribe("termostat/temperature");
      client.subscribe("termostat/set_temperature");
      client.subscribe("afara/temperature");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() 
{
  pinMode(switchPIN1, INPUT);
  pinMode(switchPIN2, INPUT);
  pinMode(switchPIN1, INPUT_PULLUP);
  pinMode(switchPIN2, INPUT_PULLUP); 
  Serial.begin(9600);
  Ethernet.begin(mac,ip);
  mylcd.Init_LCD();
  mylcd.Fill_Screen(BLACK);
  mylcd.Set_Text_Mode(0);
  mylcd.Fill_Screen(0x0000);
  mylcd.Set_Text_colour(RED);
  mylcd.Set_Text_Back_colour(BLACK);
  mylcd.Set_Text_Size(3);
  mylcd.Print_String("- Starting UP -", 0, 10); 
  client.setServer(server, 1883);
  client.setCallback(callback);
  client.subscribe("termostat/temperature");
  client.subscribe("termostat/set_temperature");
  client.subscribe("afara/temperature");
  am2320.begin();
    // Initialize the sensor (it is important to get calibration values stored on the device).
  if (pressure.begin()){
    Serial.println("BMP180 init success");
  }
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    Serial.println("BMP180 init fail\n\n");
      }
    Serial.print("IP=");Serial.println(Ethernet.localIP());
    Serial.println("SETUP finished");     
  }
  
void loop() 
{
switchStatus1 = digitalRead(switchPIN1);   // read status of switch
if (switchStatus1 == LOW)  // if status of button has changed
    {
      delay(40);     // debounce time of 50 ms
      switchStatus1 = digitalRead(switchPIN1);   // read the status of the switchPIN again
      if (switchStatus1 == LOW) // if the status of the button stay pressed
      {
       // switch is pressed for sure
       //set_temp = tempset.toFloat();
       set_temp_r = set_temp_r + 0.5;
       static char set_temp_rp[8];
       dtostrf(set_temp_r, 6, 2, set_temp_rp);  
       client.publish("termostat/set_temperature", set_temp_rp);
       Serial.println("termostat/set_temperature +++++");  
       //Serial.println("set_temp");
       //Serial.println(set_temp);
       Serial.println("set_temp_r");  
       Serial.println(set_temp_r);
       delay(10); 
         //client.subscribe("termostat/temperature");
      }
      delay(10);
    }
    
switchStatus2 = digitalRead(switchPIN2);   // read status of switch
if (switchStatus2 == LOW)  // if status of button has changed
    {
      delay(40);     // debounce time of 50 ms
      switchStatus2 = digitalRead(switchPIN2);   // read the status of the switchPIN again
      if (switchStatus2 == LOW) // if the status of the button stay pressed
      { 
       // switch is pressed for sure
       //set_temp = tempset.toFloat();
       set_temp_r = set_temp_r - 0.5;    
       static char set_temp_rp[8];
       dtostrf(set_temp_r, 6, 2, set_temp_rp);   
       client.publish("termostat/set_temperature", set_temp_rp);
       Serial.println("termostat/set_temperature ----");  
       //Serial.println("set_temp");
       //Serial.println(set_temp);
       Serial.println("set_temp_r");  
       Serial.println(set_temp_r); 
       delay(10);
          //client.subscribe("termostat/temperature"); 
      }
      delay(10);
    }
        
  client.loop();
  
  if (!client.connected()) 
    reconnect();
  
  if(!client.loop())
    client.connect("ethClient2");

/* //-----------lm 50  senzor 1------------------
 //getting the voltage reading from the temperature sensor
 int reading1 = analogRead(sensor1Pin);  
 // converting that reading to voltage, for 3.3v arduino use 3.3 in my case I am using 5V ref voltage from main power
 float voltage1 = reading1 * 5.0;
 voltage1 /= 1024.0; 
 // print out the voltage
 //Serial.print(voltage); Serial.println(" volts");
 // now print out the temperature
 float temperature1A = (voltage1 - 0.5) * 100;  
 temperature1A = temperature1A - 3;
 //converting from 10 mv per degree whit 500 mV offset
 //to degrees ((voltage - 500mV) times 100)
 static char sensor1Temp[6];
 dtostrf(temperature1A, 4, 2, sensor1Temp);
 */

/* 
if(millis() - lastTime >= 60000) {
  lastTime = millis();
  lcd_show = !lcd_show;
  mylcd.Init_LCD();
  mylcd.Fill_Screen(BLACK);
}
*/

//if (lcd_show == 0){ 
 // now = millis();
 // if (now - lastMeasure > 50000) {
 //   lastMeasure = now;
  char status;
  double T,P,p0,a,b;
  //Serial.println();
  ///Serial.print("provided altitude: ");
  //Serial.print(ALTITUDE,0);
  //Serial.print(" meters, ");
  //Serial.print(ALTITUDE*3.28084,0);
  //Serial.println(" feet");
   status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);
    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Print out the measurement:
      //Serial.print("temperature: ");
      //Serial.print(T,2);
      //Serial.print(" deg C, ");
      //Serial.print((9.0/5.0)*T+32.0,2);
      //Serial.println(" deg F");
      //mylcd.Set_Text_colour(BLUE);
      //mylcd.Set_Text_Size(4);
      //static char tTmp[7];
      //dtostrf(T, 6, 2, tTmp);
      //mylcd.Print_String(tTmp, 0, 90);  
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.
      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);
        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.
        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          // Print out the measurement:
          //Serial.print("absolute pressure: ");
          //Serial.print(P,2);
          //Serial.print(" mb, ");
          //Serial.print(P*0.0295333727,2);
          //Serial.println(" inHg");
          // The pressure sensor returns abolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sealevel function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb
          p0 = pressure.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          //Serial.print("relative (sea-level) pressure: ");
          //Serial.print(p0,2);
          //Serial.print(" mb, ");
          //Serial.print(p0*0.0295333727,2);
          //Serial.println(" inHg");
          // On the other hand, if you want to determine your altitude from the pressure reading,
          // use the altitude function along with a baseline pressure (sea-level or other).
          // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
          // Result: a = altitude in m.
          a = pressure.altitude(P,p0);
          //Serial.print("computed altitude: ");
          //Serial.print(a,0);
          //Serial.print(" meters, ");
          //Serial.print(a*3.28084,0);
          //Serial.println(" feet");
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
    //Serial.print("Temp: "); Serial.println(am2320.readTemperature());
    //Serial.print("Hum: "); Serial.println(am2320.readHumidity());
    float  t2320 = am2320.readTemperature();
    static char t2320Temp[8];
    dtostrf(t2320, 6, 2, t2320Temp);  
    float  humi = am2320.readHumidity(); 
    static char humiT[8];
    dtostrf(humi, 6, 2, humiT); 
    static char pTemp[8];
    dtostrf(P, 6, 2, pTemp);
    //Serial.println("Publishes Temperature Presure and Humidity values"); 
    // Publishes Temperature and Humidity values
    //client.publish("test/temperature", tTemp);
    //client.publish("test/humidity", hTemp);
    //Serial.println("temperature:");
    //Serial.println(T);
    //Serial.println("presure:");
    //Serial.println(P);

    mylcd.Set_Text_Size(3);
    mylcd.Set_Text_colour(RED);
    mylcd.Print_String("-Termostat etaj-", 0, 10); 
    mylcd.Set_Text_colour(BLUE);
    mylcd.Print_String("Temperatura afara:", 0, 50); 
    mylcd.Print_String(out_temp, 0, 90);
    mylcd.Print_String("C ", 180, 90);
    mylcd.Print_String("Presiune : ", 0, 130);
    mylcd.Print_String(pTemp, 0, 170);  
    mylcd.Print_String("mb ", 180, 170);
    mylcd.Print_String("Temperatura camera", 0, 210); 
    mylcd.Print_String(t2320Temp, 0, 250);
    mylcd.Print_String("C ", 180, 250);
    mylcd.Print_String("Umiditate : ", 0, 290);
    mylcd.Print_String(humiT, 0, 330);  
    mylcd.Print_String("% ", 180, 330);
    //mylcd.Print_String("....     C", 0, 410);
    mylcd.Print_String("Temperatura setata", 0, 370);
    mylcd.Print_String(tempset, 0, 410);
    mylcd.Print_String("T set :", 0, 450);
    static char set_temp_rp[8];
    dtostrf(set_temp_r, 6, 2, set_temp_rp); 
    mylcd.Print_String(set_temp_rp, 180, 450);

   // Serial.println("AM2320  umiditate:");
   // Serial.println(humiT);
   // Serial.println("AM2320  umiditate 2:");
   // Serial.println(humi);
  //}
  
//}

//if (lcd_show == 1){

 //mylcd.Init_LCD();
 //mylcd.Fill_Screen(BLACK);
 //mylcd.Set_Text_Mode(0);
 //mylcd.Fill_Screen(0x0000);
 //mylcd.Set_Text_colour(RED);
 //mylcd.Set_Text_Back_colour(BLACK);
 //mylcd.Set_Text_Size(3);
 
//}



}


