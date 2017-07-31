#include <SPI.h>
#include <Ethernet.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

/* Les constantes suivantes ne doivent en aucun cas être modifié si tel est le cas la modification devras être apporté aussi au 
 *  niveau logiciel java
 */
#define LOG_ARDUINO "rtyuiop"   // A ne surtout pas modifié
#define PASS_ARDUINO "ertyuiop^" // A ne surtout pas modifié
#define ID_ARDUINO "Arduino" // A ne surtout pas modifié

// Leds Pins
#define PINLED 8
#define NBLINE 25
#define NBLEDLINE 30

//Matrix de Leds
#define MAX_CARACTERE 6
#define TAILLE_DEP -6
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(NBLEDLINE, NBLINE, PINLED, 
NEO_MATRIX_BOTTOM     + NEO_MATRIX_LEFT  + NEO_MATRIX_SEQUENCE, 
NEO_RGB + NEO_KHZ800);
const uint16_t colors[] = { matrix.Color(0, 255, 0), matrix.Color(0, 0, 255), matrix.Color(255, 255, 0), matrix.Color(255, 0, 0) };
char colorIndex = '0';
int x = 0, y = 4, passColor    = 0;
String msg = "", msg2 = "";
byte SERVERIP[] = { 192, 168, 137, 1 };
byte mac[] = {
  0xDE, 0xAD, 0xEA, 0xEB, 0x00, 0x02
}; 
byte ip[] = { 192, 168, 137, 177 };
IPAddress myDns(192,168,1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

EthernetClient client;

void setup() {
  Ethernet.begin(mac,ip);
  Serial.begin(9600);
  delay(6000);
  Serial.println("Ethernet OK");
  setupLed();
  connectClient(3);
}

void loop() {
  if (client) {
    while (client.available()) {
      char c = client.read();
      if( c != '\n'){
          msg += c;
      }else{
        Serial.println("Message reçu : " + msg);
        colorIndex = msg[0];
        msg2 = msg.substring(1);
        msg = "";
      }
    }
  }
  if(MAX_CARACTERE > (int) msg2.length()){
    staticPrintOnLed(msg2);
  }else{
    dynamicPrintOnLed(msg2);
  }
}

void connectClient(int retries){
    while ( retries > 0 ){
        Serial.print("Attempting connection to server... ");
        if(client.connect(SERVERIP, 5030)){
            Serial.println("SUCCESS");
            sendMessage(LOG_ARDUINO);
            sendMessage(PASS_ARDUINO);
            sendMessage(ID_ARDUINO);
            break;
        }else if(retries > 0){
            Serial.println("FAILED");
            retries--;
        }
    }
    if(retries <= 0){
        Serial.println("FAILED");
        Serial.println("Connection to server failed.");
        delay(5000);
        connectClient(3);
    }
}

void sendMessage(String toSend){
    if(client){
        client.println(toSend);
        client.flush();
        Serial.println("Send message: " + toSend);
    }else{
        Serial.println("Could not send message; Not connected.");
    }
}

/* Partie Matrix de Leds */
void setupLed(){
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(40);
  matrix.setTextColor(colors[0]);
}

void staticPrintOnLed(String sms){
  x = 0;
  msgColorIndex(sms);
  matrix.show();
  delay(50);
}

void dynamicPrintOnLed(String sms){
  msgColorIndex(sms);
  if(--x < (TAILLE_DEP * (int) sms.length())) {
     x = matrix.width();   
  }
  matrix.show();
  delay(50);
}

void msgColorIndex(String sms){
  matrix.fillScreen(0);
  matrix.setCursor(x, y);
  switch(colorIndex){
    case '0':
      int j, k;
      j = 0;
      for(k = 0;  k < (int) sms.length(); k++){
        matrix.setTextColor(colors[j]); j++;
        if(j > 4) j = 0;
        matrix.print(sms[k]);
      }
    break;
    case '1':
      matrix.setTextColor(colors[0]);
      matrix.print(sms);
    break;
    case '2':
      matrix.setTextColor(colors[1]);
      matrix.print(sms);
    break;
    case '3':
      matrix.setTextColor(colors[2]);
      matrix.print(sms);
    break;
    case '4':
      matrix.setTextColor(colors[3]);
      matrix.print(sms);
    break;
  }
}

