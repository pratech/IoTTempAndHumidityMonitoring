#include <ESP8266WiFi.h>
#include <DHT.h>

String apiKey = "92TE724P1VSWQNGK"; // Enter your Write API key from ThingSpeak
const char *ssid = "Pinwheel"; // replace with your wifi ssid and wpa2 key
const char *pass = "YourWiFiPassword";
const char* server = "api.thingspeak.com";

#define DHTPIN 0 //pin where the dht11 is connected
#define DHTTYPE DHT11
#define RELAY_PIN 4

DHT dht(DHTPIN, DHTTYPE);

WiFiClient client;

void setup()
{
  pinMode(RELAY_PIN,OUTPUT);
  Serial.begin(9600);
  delay(10);
  dht.begin();

  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

}

void loop()
{

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  if (client.connect(server,80)) // “184.106.153.149” or api.thingspeak.com
  {
  
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(temperature);
    postStr +="&field2=";
    postStr += String(humidity);
    postStr += "\r\n\r\n";
  
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    
    Serial.print("Temperature:");
    Serial.print( temperature) ;
    Serial.print("degrees Celcius, Humidity: ");
    Serial.print(humidity);
    Serial.println("Sent to Thingspeak.");
    //Operating Relay Based on Temperature && Humidity
    if(temperature >= 500) //Check the sensor output
    {
      temperature=500; // 
      digitalWrite(RELAY_PIN, HIGH);
      digitalWrite(RELAY_PIN, HIGH); // switch on the relay
      Serial.println ("cooling on " );
  
    }
    else
    {
      temperature<=50; //
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("cooling off" );
    }
    client.stop();
  
    Serial.println("waiting..."); 
  
    //thingspeak needs minimum 15 sec delay between updates
    delay(15000);
  }
}
