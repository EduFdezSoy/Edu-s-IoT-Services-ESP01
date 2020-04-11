/*
  ServerCalls.h - Library to send crap to your clients.
  Created by Eduardo Fernandez (EduFdezSoy), April 10, 2020.
  Released into the public domain.
*/
// those lines define the class if it is not defined, to prevent to include it twice
#ifndef ServerCalls_h
#define ServerCalls_h

#include "Arduino.h"

class ServerCalls
{
public:
  ServerCalls(WiFiClient client);
  void sendStatus(bool statuses[], int lenght);
  void sendStatusHTML(bool statuses[], int lenght);
  void sendBlank();
  void sendBlankHTML();
  void send();
  void sendHTML();

private:
  WiFiClient _client;
  void responseHeader();
  void responseHTMLHeader();
  String printBool(bool a);
};

#endif