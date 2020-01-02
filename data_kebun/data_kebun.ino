#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

#define SSID "connected"
#define PASS "43_17_43"

const char *ssid = SSID;
const char *password = PASS;

ESP8266WebServer server(80); //PORT

struct dataUtama
{
  int air;
  int humidity;
  int PH;
  int PPM; //part per million (chemical)
  int water;
};

String genJSON(dataUtama data)
{
    char buff[64];
    sprintf(buff,   "data_air_temp=%d&"
                    "data_humidity=%d&"
                    "data_ph=%d&"
                    "data_ppm=%d&"
                    "data_water_temp=%d&"
                    ,data.air
                    ,data.humidity
                    ,data.PH
                    ,data.PPM
                    ,data.water);
    return buff;
}

int genRandom(int batasBawah, int batasAtas)
{
    int temp = rand() * time(0) % (batasAtas - batasBawah) + batasBawah;
    return temp;
}

void setup(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266"))
  {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void)
{
  server.handleClient();
  MDNS.update();

  HTTPClient http;
  if (http.begin("http://smart-hyco.herokuapp.com/sensor"))
  {
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    dataUtama data;

    data.air      = genRandom(10, 20);
    data.humidity = genRandom(100, 200);
    data.PH       = genRandom(-7, 7);
    data.PPM      = genRandom(17, 28);
    data.water    = genRandom(21, 32);

    String JSONQ = genJSON(data);

    int httpCode = http.POST(JSONQ);
    Serial.printf("[HTTP] code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
    {
      String payload = http.getString();
      Serial.println(payload);
    }
    else
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  else
    Serial.printf("[HTTP} Unable to connect\n");
  delay(10);
}
