#include <NewPing.h>
unsigned int us_value1,us_value2;
#define MAX_DISTANCE 400
#define DEBUG true
#define wifi_pin 13
boolean wificonnected=false;

String Wifi_SSID="TP-LINK_9B957E";//replace with your Wifi SSID name
String Wifi_PWD="279B957E";       //replace with your Wifi password
//String Wifi_SSID="Ambientwalk";//replace with your Wifi SSID name
//String Wifi_PWD="somatheremin"; 
//String apikey="0P31Q41VMNV3KVJO";  // YOUR THINGSPEAK.COM ACCOUNT API KEY
//for device 1 (sewed)
//NewPing sonar1(4, 2, MAX_DISTANCE);//left
//NewPing sonar2(9, 7, MAX_DISTANCE);//right
//for device 2 (cover-removable)
NewPing sonar1(3, 2, MAX_DISTANCE);//left
NewPing sonar2(8, 9, MAX_DISTANCE);//right

void setup()
{
  pinMode(13, OUTPUT); 
  pinMode(15, OUTPUT);
  digitalWrite (13, HIGH);   
  Serial.begin(9600); 
  Serial1.begin(9600);

  sendData("AT+RST\r\n",2000,DEBUG); // reset module
  sendData("AT+CWMODE=1\r\n",500,DEBUG); // configure as client
  sendData("AT+CWLAP\r\n",500,DEBUG);
  Serial.println(Serial1.read());
  //sendData("AT+CWJAP=\"" + Wifi_SSID + "\",\"" + Wifi_PWD + "\"\r\n",5000,DEBUG); // connect to router
  //delay(3000);
  for(int i=1; i<5; i++){
 sendData("AT+CWJAP=\"" + Wifi_SSID + "\",\"" + Wifi_PWD + "\"\r\n",5000,DEBUG);
 if(Serial1.find("OK")){
    wificonnected=true;
    //digitalWrite (15, HIGH); 
    Serial.println("Wifi connected");
    break;
 }
 else{
  // digitalWrite (15, LOW); 
 delay(50);
 }
 }
 // if(Serial1.find("OK")){
    //wificonnected=true;
  //break;
 // }//end if
 // else{delay(50);}
  //}//end for
  //if(!wificonnected){Serial.println("Couldn't connect to wifi.");}
  sendData("AT+CIFSR\r\n",500,DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n",500,DEBUG); // configure for multiple connections
  
  Serial.println("Server ready");
  Serial.println();
}

void loop()
{ us_value1 = sonar1.ping()/US_ROUNDTRIP_CM;
  Serial.print("Distance1: ");
  Serial.print(us_value1);
  Serial.print("cm; ");
  us_value2 = sonar2.ping()/US_ROUNDTRIP_CM;
   Serial.print("Distance2: ");
  Serial.print(us_value2);
  Serial.println("cm ");
  
  String ultrasonic1=String(us_value1);// turn integer to string
  String ultrasonic2=String(us_value2);
  
  // TCP CONNECTION
  /*
  String connectServer = "AT+CIPSTART=4,\"UDP\",\"";
  connectServer += "184.106.153.149"; // api.thingspeak.com
  connectServer += "\",80";
*/
//UDP connection to OSC receiver

  String connectServer = "AT+CIPSTART=4,\"UDP\",\"";
  //connectServer += "172.20.10.2"; // server on processing (uni mac), will change ip to phone's ip
  connectServer += "192.168.0.100";//android phone,TP-Link modem
  //connectServer += "172.20.10.6"; //home mac, Ambientwalk
 // connectServer += "172.20.10.4"; //android phone, Ambientwalk
  connectServer += "\",12000,1112,0";

    
  //Serial.print("TCP Connection = ");
  Serial.print("UDP Connection = ");
  Serial.println(connectServer);
  Serial1.println(connectServer);
  delay(1000);  // wait 1 second to establish connection between thingSpeak and ESP8266
   
  if(Serial1.find("Error"))
  {
    Serial.println("AT+CIPSTART error");
    return;
  }
  
  // UPDATE THINGSPEAK CHANNEL WITH DATA STRING
  /*
  String dataToPush = "GET /update?api_key=";
  dataToPush += apikey;
  dataToPush +="&field1=";
  dataToPush += ultrasonic1;
  dataToPush +="&field2=";
  dataToPush += ultrasonic2;
  dataToPush += "\r\n\r\n";
*/
//sample osc
String dataToPush = "L"+ultrasonic1+"R"+ultrasonic2+"\r\n";
//String dataToPush="L220R20\r\n";
  Serial.print("Data updated = ");
  Serial.print(dataToPush);

  // SEND DATA LENGTH
  String sendData = "AT+CIPSEND=4,";
  sendData += String(dataToPush.length());

  Serial.print("Data length = ");
  Serial.println(sendData);
  Serial1.println(sendData);
  delay(200);  // wait 2 second to establish connection between thingSpeak and ESP8266
//Serial.println(Serial1.readString());
  if(Serial1.find("OK"))
  {
    Serial1.print(dataToPush);
    Serial.println("Data sent");
    digitalWrite (15, HIGH); 
    delay(50);
    digitalWrite (15, LOW); 
  }
  else
  { digitalWrite (15, LOW); 
    Serial1.println("AT+CIPCLOSE");
    Serial.println("AT+CIPCLOSE");  // alerting closing connection
  }
  
 // delay(1000);  // 1 sec delay between updates
}

String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    
    Serial1.print(command); // send the read character to the esp8266
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(Serial1.available())
      {
        char c = Serial1.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(response);
    }
    
    return response;
}
