/*
  Edu's IoT Services - ESP8266-01 code, it opens and closes gates
  Created by Eduardo Fernandez (EduFdezSoy), April 10, 2020.
  Released into the public domain.
  version: 1.0
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ServerCalls.h>

const char *ssid = "xxxxxxxxxxx"; // type your ssid
const char *password = "xxxxxxxxxxx";  // type your password

const char *HOSTNAME = "ESP-xxxxxxx";

const int RELAYS_LENGHT = 2;
const int Relays[RELAYS_LENGHT] = {1, 3};
bool relaysStatus[RELAYS_LENGHT] = {false, false};

WiFiServer server(80);

///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// UTILS FUNCTIONS ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void powerRelays(int relay, bool status)
{
  if (relay < RELAYS_LENGHT)
  {
    if (status != relaysStatus[relay])
    {
      relaysStatus[relay] = status;

      if (status)
      {
        digitalWrite(Relays[relay], HIGH);
      }
      else
      {
        digitalWrite(Relays[relay], LOW);
      }
    }
  }
}

void registerDevice(String ip)
{
  bool done = false;
  do
  {
    HTTPClient http;

    http.begin("http://pi/iot/register"); // deprecated tus muertos
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "device=";
    postData += HOSTNAME;
    postData += "&ip=" + ip;
    postData += "&relayCount=";
    postData += RELAYS_LENGHT;

    int res = http.POST(postData);
    http.end();

    if (res == 200)
    {
      done = true;
    }
    else
    {
      delay(500);
    }
  } while (!done);
}

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// PATH MANAGEMENT FUNCTIONS //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/**
 * From "GET /LED?set=0 HTTP/1.1"
 * Returns "/LED?set=0"
 */
String getFullPath(String call)
{
  call.toLowerCase();

  return call.substring(call.indexOf(' ') + 1, call.lastIndexOf(' '));
}

/**
 * From "GET /LED?set=0 HTTP/1.1"
 * Returns "LED"
 */
String getPath(String call)
{
  call = getFullPath(call);
  int iArgs = call.indexOf('?');

  if (iArgs == -1)
  {
    return call.substring(1);
  }

  return call.substring(1, iArgs);
}

/**
 * From "GET /LED?set=0 HTTP/1.1"
 * Returns 0
 */
int getIntArg(String call, String arg)
{
  call = getFullPath(call);

  int iArgs = call.indexOf(arg);
  if (iArgs == -1)
  {
    return -1;
  }

  return call.substring(iArgs + arg.length() + 1).toInt();
}

///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// ROUTE FUNCTIONS ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void routeBlank(WiFiClient client)
{
  ServerCalls call = ServerCalls(client);

  call.sendBlankHTML();
  call.sendHTML();
}

void routeBaseURL(WiFiClient client)
{
  ServerCalls call = ServerCalls(client);

  call.sendStatusHTML(relaysStatus, RELAYS_LENGHT);
  call.sendHTML();
}

void routeStatus(WiFiClient client)
{
  ServerCalls call = ServerCalls(client);

  call.sendStatus(relaysStatus, RELAYS_LENGHT);
  call.send();
}

void routeOpen(WiFiClient client, int relay)
{
  ServerCalls call = ServerCalls(client);

  if (relay != -1)
  {
    powerRelays(relay, true);
  }

  call.sendBlank();
  call.send();
}

void routeClose(WiFiClient client, int relay)
{
  ServerCalls call = ServerCalls(client);

  if (relay != -1)
  {
    powerRelays(relay, false);
  }

  call.sendBlank();
  call.send();
}

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// ROUTE MANAGER ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void routeManager(WiFiClient client, String request)
{
  String path = getPath(request);
  if (path == "status")
  {
    routeStatus(client);
  }
  else if (path == "open")
  {
    routeOpen(client, getIntArg(request, "relay"));
  }
  else if (path == "close")
  {
    routeClose(client, getIntArg(request, "relay"));
  }
  else if (path == "")
  {
    routeBaseURL(client);
  }
  else
  {
    routeBlank(client);
  }
}

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// SETUP ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

// FALTA QUE HAGA DE CLIENTE EN EL SETUP PARA QUE SE REGISTRE EN EL TRUE SERVER
void setup()
{
  delay(10);
  //********** CHANGE PIN FUNCTION  TO GPIO **********
  // GPIO 1 (TX) swap the pin to a GPIO.
  pinMode(1, FUNCTION_3);
  // GPIO 3 (RX) swap the pin to a GPIO.
  pinMode(3, FUNCTION_3);
  //**************************************************

  // Serial.begin(115200);
  delay(10);

  // setup relays
  for (int i = 0; i < RELAYS_LENGHT; i++)
  {
    pinMode(Relays[i], OUTPUT);
    powerRelays(i, true);
  }

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.hostname(HOSTNAME);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // lets register this device in our server
  registerDevice(WiFi.localIP().toString());

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  Serial.println("");
}

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// LOOP /////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void loop()
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available())
  {
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // call the routes manager
  routeManager(client, request);

  // another delay, just in case, and the conection ends
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}