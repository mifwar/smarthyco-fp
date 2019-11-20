#include <dht.h>
#define dht_apin A0 // Analog Pin sensor is connected to 
dht DHT; //to access dht library
#define sensor A0
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0x74, 0x2F, 0x68, 0x1A, 0x3B, 0xDE };

IPAddress server(127, 0, 0, 1); //local

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(127, 0, 0, 1); //local
EthernetServer HTTPserver(80);
EthernetClient HTTPclient;

unsigned long beginMicros, endMicros;

int temp, humidity;
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // try to congifure using IP address instead of DHCP:
  Ethernet.begin(mac, ip);
  HTTPserver.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.print("connecting to ");
  Serial.print(server);
  Serial.println("...");
  beginMicros = micros();
}

void loop() {
  DHT.read11(dht_apin);
  Serial.print("Current humidity = ");
  Serial.print(DHT.humidity);
  Serial.print("%  ");
  Serial.print("temperature = ");
  Serial.print(DHT.temperature); 
  Serial.println("C  ");                                    
               
  temp = DHT.temperature;

  if(HTTPclient.connect(server, 3000)) {
    //    Serial.println("Sending to Server: ");
    HTTPclient.println("POST /data HTTP/1.1");
    Serial.print("POST /data HTTP/1.1");
    HTTPclient.println("Host: 192.168.1.177");
    HTTPclient.println("Content-Type: application/x-www-form-urlencoded");
    HTTPclient.println("Connection: close");
    HTTPclient.println("User-Agent: Arduino/1.0");
    HTTPclient.print("Content-Length: ");

    data = "&temp=" + String(temp) + "&=humidity" + String(humitidty);
    HTTPclient.println(data.length());
    HTTPclient.println();
    HTTPclient.print(data);
    HTTPclient.println();
  }
  else
    Serial.println("connection failed");

  // if the server's disconnected, stop the client:
  if (!HTTPclient.connected()) {
    endMicros = micros();
       Serial.println();
       Serial.println("disconnecting.");
    HTTPclient.stop();
  }
}

