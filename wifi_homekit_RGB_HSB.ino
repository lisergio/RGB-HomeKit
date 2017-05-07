#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *ssid =	"SSID";		// SSID de nuestra red wifi
const char *pass =	"PASSWORD";	// Contrasea de nuestra red wifi
const char* host = "rgb1";
IPAddress server(192, 168, 0, 56); // IP de la raspberry Pi
const int SalidaR = 0;
const int SalidaG = 2;
const int SalidaB = 16;
const int pulsador = 14 ;
const int WiFiLed = 13;
boolean estado_pulsador;
float  R=0;
float  G=0;
float  B=0;
int rgb_colors[3];
int hue;
int saturation;
int brightness;
int contador=0;

#define BUFFER_SIZE 100

const byte dim_curve[] = {
    0,   1,   1,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,
    3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   4,   4,   4,   4,
    4,   4,   4,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,   6,   6,
    6,   6,   6,   6,   6,   7,   7,   7,   7,   7,   7,   7,   8,   8,   8,   8,
    8,   8,   9,   9,   9,   9,   9,   9,   10,  10,  10,  10,  10,  11,  11,  11,
    11,  11,  12,  12,  12,  12,  12,  13,  13,  13,  13,  14,  14,  14,  14,  15,
    15,  15,  16,  16,  16,  16,  17,  17,  17,  18,  18,  18,  19,  19,  19,  20,
    20,  20,  21,  21,  22,  22,  22,  23,  23,  24,  24,  25,  25,  25,  26,  26,
    27,  27,  28,  28,  29,  29,  30,  30,  31,  32,  32,  33,  33,  34,  35,  35,
    36,  36,  37,  38,  38,  39,  40,  40,  41,  42,  43,  43,  44,  45,  46,  47,
    48,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,
    63,  64,  65,  66,  68,  69,  70,  71,  73,  74,  75,  76,  78,  79,  81,  82,
    83,  85,  86,  88,  90,  91,  93,  94,  96,  98,  99,  101, 103, 105, 107, 109,
    110, 112, 114, 116, 118, 121, 123, 125, 127, 129, 132, 134, 136, 139, 141, 144,
    146, 149, 151, 154, 157, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 190,
    193, 196, 200, 203, 207, 211, 214, 218, 222, 226, 230, 234, 238, 242, 248, 255,
};

void callback(const MQTT::Publish& pub) {

  String myMessage = String(pub.payload_string());
  Serial.print(pub.topic());
  Serial.print(" => ");
  String myTopic = String(pub.topic());

if(myTopic == host)
  {
    Serial.println(pub.payload_string());
if(pub.payload_string() == "on")
    { 
    getRGB(hue,saturation,brightness,rgb_colors);  
    }
else
    {
    analogWrite(SalidaR,0);
    analogWrite(SalidaG,0);
    analogWrite(SalidaB,0);
    }
  }
else if(myTopic == host+(String)"/brightness")
 { 
    Serial.println(pub.payload_string());
    brightness=(myMessage.toFloat());
    brightness = map (brightness,0,100,0,254);
    getRGB(hue,saturation,brightness,rgb_colors);
  }
else if(myTopic == host+(String)"/saturation")
 { 
    Serial.println(pub.payload_string());
    saturation = (myMessage.toFloat());
    saturation = map (saturation,0,100,0,254);
    getRGB(hue,saturation,brightness,rgb_colors);

  }
else if(myTopic == host+(String)"/hue")
 { 
    Serial.println(pub.payload_string());
    hue=(myMessage.toFloat());
    getRGB(hue,saturation,brightness,rgb_colors);

  }    
}

WiFiClient wclient;
PubSubClient client(wclient, server);

void setup() 
{
pinMode(SalidaR, OUTPUT);
pinMode(SalidaG, OUTPUT);
pinMode(SalidaB, OUTPUT);
pinMode(WiFiLed, OUTPUT);
pinMode(pulsador, INPUT);
Serial.begin(115200);
WiFi.mode(WIFI_STA);
delay(10);
digitalWrite(SalidaR, LOW);
digitalWrite(SalidaG, LOW);
digitalWrite(SalidaB, LOW);
client.set_callback(callback);
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
    digitalWrite(WiFiLed, HIGH);
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      if (client.connect("ESP8266: rgb1")) {
        client.publish("outTopic",(String)"hello world, I'm "+host);
        client.subscribe(host+(String)"/#");
      }
    }

    if (client.connected())
      client.loop();
  }
  
estado_pulsador = digitalRead(pulsador);
if (estado_pulsador==LOW)
{
  contador++;
  if (contador>=5)
  {
    contador=0;
    delay(500);
  }
  if (contador==0)
  {
    analogWrite(SalidaR, 0);           
    analogWrite(SalidaG, 0);           
    analogWrite(SalidaB, 0);
    delay(500);    
  }
  if (contador==1)
  {
    analogWrite(SalidaR, 1024);           
    analogWrite(SalidaG, 0);           
    analogWrite(SalidaB, 0);
    delay(500);    
  }
  if (contador==2)
  {
    analogWrite(SalidaR, 0);           
    analogWrite(SalidaG, 1024);           
    analogWrite(SalidaB, 0);
    delay(500);    
  }
  if (contador==3)
  {
    analogWrite(SalidaR, 0);           
    analogWrite(SalidaG, 0);           
    analogWrite(SalidaB, 1024);
    delay(500);    
  }
  if (contador==4)
  {
    analogWrite(SalidaR, 1024);           
    analogWrite(SalidaG, 1024);           
    analogWrite(SalidaB, 1024);
    delay(500);    
  }
}
}


void getRGB(int hue, int sat, int val, int colors[3]) { 

  val = dim_curve[val];
  sat = 255-dim_curve[255-sat];
 
  int r;
  int g;
  int b;
  int base;
 
  if (sat == 0) { 
    colors[0]=val;
    colors[1]=val;
    colors[2]=val;  
  } else  { 
 
    base = ((255 - sat) * val)>>8;
 
    switch(hue/60) {
    case 0:
        r = val;
        g = (((val-base)*hue)/60)+base;
        b = base;
    break;
 
    case 1:
        r = (((val-base)*(60-(hue%60)))/60)+base;
        g = val;
        b = base;
    break;
 
    case 2:
        r = base;
        g = val;
        b = (((val-base)*(hue%60))/60)+base;
    break;
 
    case 3:
        r = base;
        g = (((val-base)*(60-(hue%60)))/60)+base;
        b = val;
    break;
 
    case 4:
        r = (((val-base)*(hue%60))/60)+base;
        g = base;
        b = val;
    break;
 
    case 5:
        r = val;
        g = base;
        b = (((val-base)*(60-(hue%60)))/60)+base;
    break;
    }
 
    colors[0]=r;
    colors[1]=g;
    colors[2]=b; 
  } 
rgb_colors[0] = map (rgb_colors[0],0,254,0,1024);
rgb_colors[1] = map (rgb_colors[1],0,254,0,1024);
rgb_colors[2] = map (rgb_colors[2],0,254,0,1024);

analogWrite(SalidaR, rgb_colors[0]);           
analogWrite(SalidaG, rgb_colors[1]);           
analogWrite(SalidaB, rgb_colors[2]);            
 
    
}

