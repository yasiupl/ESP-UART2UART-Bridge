#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char *ssid = "AntaresSkyNet";
const char *pass = "nimbus2000";

// This is Access Point "Server" code

IPAddress ServerIP(192, 168, 4, 1);
IPAddress ClientIP(192, 168, 4, 2);

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;
unsigned int udpPort = 2000;

#define bufferSize 8192
char incomingBuffer[255];
char outgoingBuffer[bufferSize]; //Where we get the UDP data
uint8_t iofs = 0;

void setup()
{
    Serial.begin(115200);
    Serial.println("Welcome to SkyNet.");
    WiFi.softAP(ssid, pass); //Create Access point

    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    //Start UDP
    Serial.println("Starting UDP");
    udp.begin(udpPort);
    Serial.print("Local port: ");
    Serial.println(udp.localPort());
}

void loop()
{
    int packetSize = udp.parsePacket();
    if (packetSize)
    {
        // We've received a UDP packet, send it to serial
        int len = udp.read(incomingBuffer, 255);
        if (len > 0)
        {
            incomingBuffer[len] = 0;
        }

        Serial.print(incomingBuffer);
    }
    else
    {
        iofs = 0;
        //If serial data is recived send it to UDP
        if (Serial.available())
        {
            while (1)
            {
                if (Serial.available())
                {
                    outgoingBuffer[iofs] = char(Serial.read()); // read char from UART
                    if (iofs < bufferSize - 1)
                    {
                        iofs++;
                    }
                }
                else
                {
                    break;
                }
            }

            udp.beginPacket(ClientIP, udpPort); //Send Data to Slave unit
            udp.write(outgoingBuffer, iofs);
            udp.endPacket();
        }
    }
}
