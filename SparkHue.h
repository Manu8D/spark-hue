#include "application.h"
#include <stdlib.h>

boolean manageBridgeConnection();
void sendCommandToHueBridge(char requestBody[], char endpoint[]);
void setBrightness(int brightness);
void increaseDecreaseBrightness();

TCPClient client;
extern char* itoa(int a, char* buffer, unsigned char radix);