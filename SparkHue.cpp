#include "SparkHue.h"

#define PORT 80
#define TIME_BETWEEN_REQUESTS 100 //max. 10 request/second for bridge
#define REQUEST_LEN 800

byte bridgeIP[] = { 10, 0, 1, 2 }; // Enter the IP address of your hue bridge
char bridgeIP_str[] = "10.0.1.2"; //same as bridgeIP as char[]
char bridgeUser[] = "newdeveloper"; // a registered user on your hue bridge

void setup() {
    Serial.begin(9600);
}

void loop() {
  if(!Serial.available()){
    return;
  }

  if (manageBridgeConnection()) {
    increaseDecreaseBrightness();
  }
}

unsigned long lastRequest = 0;

boolean manageBridgeConnection() {
  unsigned long timeSince = millis()-lastRequest;

  if(!(timeSince >= TIME_BETWEEN_REQUESTS)) {
    return false;
  }

  if(client.connected()) {
    return true;
  }
  else {
    if(client.connect(bridgeIP, PORT)) {
      return true;
    }
    else {
      client.stop();
      return false;
    }
  }
}

void sendCommandToHueBridge(char requestBody[], char endpoint[]) {
  if (manageBridgeConnection()) {
    char request[REQUEST_LEN];
    char smallBuffer[10];

    sprintf(request, "PUT ");
    strcat(request, endpoint);
    strcat(request, " HTTP/1.1\r\nHost: ");
    strcat(request, bridgeIP_str);
    strcat(request, "\r\nContent-Length: ");
    itoa(strlen(requestBody), smallBuffer, 10);
    strcat(request, smallBuffer);
    strcat(request, "\r\nContent-Type: application/json\r\n");
    strcat(request, "Connection: keep-alive\r\n");
    strcat(request, "\r\n");
    strcat(request, requestBody);
    strcat(request, "\r\n");

    int bufferLength = strlen(request);

    if(bufferLength == 0){
        Serial.println("No request");
        return;
    }

    client.write((const uint8_t *)request, REQUEST_LEN - bufferLength);

    while(client.available()) {
      client.flush();
      //or read the data with client.read();
    }

    lastRequest = millis();
  }
  else {
    Serial.println("Not connected");
  }
}

void setBrightness(int brightness) {
  //Convert int to char[]
  char smallBuffer[10];
  itoa(brightness, smallBuffer, 10);

  char requestBody[20];
  sprintf(requestBody, "%s", "{\"bri\":");
  strcat(requestBody, smallBuffer);
  strcat(requestBody, "}");

  char endpoint[200];
  sprintf(endpoint, "/api/");
  strcat(endpoint, bridgeUser);
  strcat(endpoint, "/lights/1/state");

  sendCommandToHueBridge(requestBody, endpoint);
}

// Example
boolean increaseBrightness = false;
int brightness = 255;

void increaseDecreaseBrightness() {
  if(increaseBrightness) {
      brightness += 10;
        if(brightness > 255) {
          brightness = 255;
          increaseBrightness = false;
        }
    }
    else {
      brightness -= 10;
      if(brightness < 0) {
        brightness = 0;
        increaseBrightness = true;
      }
    }
  setBrightness(brightness);
}
