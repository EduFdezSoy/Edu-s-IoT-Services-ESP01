/*
  ServerCalls.cpp - Library to send crap to your clients.
  Created by Eduardo Fernandez (EduFdezSoy), April 10, 2020.
  Released into the public domain.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "ServerCalls.h"

ServerCalls::ServerCalls(WiFiClient client)
{
    _client = client;
}

void ServerCalls::sendStatus(bool statuses[], int lenght)
{
    responseHeader();

    _client.print("{");
    _client.print("\"output\":");
    _client.print("[");

    for (int i = 0; i < lenght; i++)
    {
        _client.print("{");
        _client.print("\"value\": ");
        _client.print(printBool(statuses[i]));
        _client.print("}");

        if (i != lenght - 1)
        {
            _client.print(",");
        }
    }

    _client.print("]");
    _client.println("}");
}

void ServerCalls::sendStatusHTML(bool statuses[], int lenght)
{
    responseHTMLHeader();

    for (int i = 0; i < lenght; i++)
    {
        _client.println("<p>");
        _client.print("pin ");
        _client.print(i);
        _client.print(" set to ");
        _client.println(printBool(statuses[i]));
        _client.print("<a href=\"");
        // if its open, close it!
        if (statuses[i])
        {
            _client.print("/close");
        }
        else
        {
            _client.print("/open");
        }
        _client.print("?relay=");
        _client.print(i);
        _client.print("\" target=\"hidden\">");
        // if its open, close it!
        if (statuses[i])
        {
            _client.print("Close");
        }
        else
        {
            _client.print("Open");
        }
        _client.println("</a>");

        _client.println("</p>");
    }

    _client.println("<br>");
}
void ServerCalls::sendBlank()
{
    responseHeader();
    _client.println("{}");
}

void ServerCalls::sendBlankHTML()
{
    responseHTMLHeader();
    _client.println("<p>");
    _client.println("There's nothing in this page, stop messing around with my chips and drink some water!");
    _client.println("</p>");
}

void ServerCalls::send()
{
    delay(1);
}

void ServerCalls::sendHTML()
{
    _client.println("<br>");
    _client.println("<footer>Copyright &copy; 2020 Eduardo Fernandez.</footer>");
    _client.println("</body>");
    _client.println("</html>");
    send();
}

// private
void ServerCalls::responseHeader()
{
    _client.println("HTTP/1.1 200 OK");
    _client.println("Content-Type: application/json");
    _client.println("Server: Edu's IoT Services Embebed Server");
    _client.println(""); //  do not forget this one
}

// private
void ServerCalls::responseHTMLHeader()
{
    _client.println("HTTP/1.1 200 OK");
    _client.println("Content-Type: text/html");
    _client.println("Server: Edu's IoT Services Embebed Server");
    _client.println(""); //  do not forget this one

    _client.println("<!DOCTYPE html>");
    _client.println("<html>");
    _client.println("<head>");
    _client.println("<title>Edu's IoT Services</title>");
    _client.print("<link rel=\"icon\" type=\"image/png\" href=\"");
    _client.print("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAC4jAAAuIwF4pT92AAAAB3RJTUUH5AQLCiQSbEbHzQAAABl0RVh0Q29tbWVudABDcmVhdGVkIHdpdGggR0lNUFeBDhcAAAFzSURBVDjLrZKtiwJRFMV/yjad+sAgYtAgOFmQB2IR/wLBZLTMBK0aDCaTRoPJSYIYBIMios00IILigJOtgyIYZtMuO7pfrHvhwLuXx7nnnfN8gMsT5X876LqOYRgYhkEulwOgUqmQSCR+R5DJZDidTsznc47HIwDZbJZwOAyAEIJOp0Oz2fQQvHxs0uk0yWSS7XYLwGAw4HA4AFCr1RBCYNv2gwoXcIfDoVutVt23/h5SSnc8Hrur1coz939kKhQKzGYzSqXSwxYpJdfrlWAw+LmC30BK6SqK4pn5/i3Gv9Z7CpqmkUqlACiXyziO47nY7XYJBAIAmKZJq9XyKthsNkSjUfb7PY7joCgK7XYbTdMAWC6XRCIRLMvCNM3HJywWCy6XC7vdDoBer8ftdqNYLJLP5+n3+5zPZyzLYjqd/uxBKBSi0Whg2zaqqv5sYr1eJxaLoes6QgjW6zWj0Yh4PM5kMvnWyC9zV1X1Ifd7PP0PXgFLrKzhD+Ym/gAAAABJRU5ErkJggg==");
    _client.println("\">");
    _client.println("<meta http-equiv=\"refresh\" content=\"5\">");
    _client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
    _client.println("</head>");
    _client.println("<body>");
    _client.println("<h1>Edu's IoT Services</h1>");
    _client.println("<iframe name=\"hidden\" hidden></iframe>");
}

// private
String ServerCalls::printBool(bool a)
{
    if (a)
    {
        return "true";
    }
    else
    {
        return "false";
    }
}