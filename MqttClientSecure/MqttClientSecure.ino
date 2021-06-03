#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* WIFI_SSID = "HUAWEI-5G";
const char* WIFI_PASS = "COVA65755522";
const char* AWS_ENDPOINT = "a20pnhdb4e5whl-ats.iot.us-east-2.amazonaws.com";
const int AWS_ENDPOINT_PORT = 8883;

const char* MQTT_CLIENT_ID = "YOUR_CLIENT_ID";
const char* SUBCRIBE_TOPIC = "ucb/aws_iot_in"; // subscribe
const char* PUBLISH_TOPIC = "ucb/aws_iot_out"; // publish

int cm = 0;

const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUEZI3GNwf/+uDLs0E4YKhHCTzIIAwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIxMDUxNzIwMzUy
MVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKU8EOAtwcF0tVlqdtWx
ID7/PEm81edUHra0SFmX7PuftvW2sSDVdLsr/+sq87lq2sVGIDK3cP1nq8WIGqfy
X08YvOp+RV6LuX8Go6qsCXChw+wBF0jqqyRKqlR09genOWnC/3AwS9yLMKeSwqNz
k0zBYIi38KjYvJ4+yoQ2TeCYdOBy4Ie6HzAS0t207c74ogS4csLywRkBx00xK/Yo
DUUh3q+7zer8PzoG9WQl2CqGKKJqtWiN5RcDZM443KIWfOhQRDGYlM4hFcyB1Ov6
pAgtlixGq5/eMR8usTbFg01xe1tcB5VIsShak+AeLZo0xpcDgwinIYcL3O6nKXG2
IocCAwEAAaNgMF4wHwYDVR0jBBgwFoAU6C5lXF2uFFtFirrZWjdRDmig/nYwHQYD
VR0OBBYEFDLRPxA0hiD1o50syksmKT+6ZBtbMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAXYoMYtz92NZscHFEkRZjMmS0d
ug4y96yOahPG4VFgOGHJ0bTZbtFePxm04IwQ2PzVDW55WvZel2a7HeurEZDqIbQg
0btv5Q9WQXeB0s9b8Rihutlobi5oXP+tGz887M/jO9fj4ZFVrY4sqEuwBP2p5J6M
sx7u16gaaTLCEQiWYmqP0Bp9n9mhUDeluS3NxpeeXTYrjS1NRZPCdZmhUwsezfg5
byLUdbKO1E+kbbGXlGOnpRYuTggxKnlcHQvF87unsRZLbm3mHI185z3v3dFGzSs7
seJXwTLay3biCqTqtS6S1gHykdHOUpEl/lvZzHRlai4Ad+3BVY24y/21qlCf
-----END CERTIFICATE-----
)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEApTwQ4C3BwXS1WWp21bEgPv88SbzV51QetrRIWZfs+5+29bax
INV0uyv/6yrzuWraxUYgMrdw/WerxYgap/JfTxi86n5FXou5fwajqqwJcKHD7AEX
SOqrJEqqVHT2B6c5acL/cDBL3Iswp5LCo3OTTMFgiLfwqNi8nj7KhDZN4Jh04HLg
h7ofMBLS3bTtzviiBLhywvLBGQHHTTEr9igNRSHer7vN6vw/Ogb1ZCXYKoYoomq1
aI3lFwNkzjjcohZ86FBEMZiUziEVzIHU6/qkCC2WLEarn94xHy6xNsWDTXF7W1wH
lUixKFqT4B4tmjTGlwODCKchhwvc7qcpcbYihwIDAQABAoIBAHWcY4auCxPgNpUG
qr8/7Gvq0MbLmpWl0pbJjBX5hb002c3b1AhC52uyzd5eHE0GOeHhIFn5KHu2pxVa
415Xrm9mdFGUv1ukqMw5tZVuDuoUupfpzuEf5+FVJBgpiFW6OauGGLuKJPU+PcuJ
MvGU+mvP2R53jt0gKGGa4Btra9NIjEiGmyvB5HW195TKG7UKPNxW9tGORD+e0Pv/
5WfS+CCtWLj6p9Vg0B3Ld9IFdFKpiZSngapbsjTJVP6KRGO4SwgKy6TBrOsFQd8Y
EBdoJH3g9MytKj77R00Rv4h4YaZ2YM5P2vVC7SLxOKgNbGl+8rMHTzCgVB2mdvhi
XfrHh0ECgYEA0r7U6VrZUyovUrZG5V9B0TQpw+BOAUHNQvVXJaCJDqjQ+5zuVB1u
ND14BTpL6Q29PBVNg1ndF/PiAXCmrqsyRi/EOU2P++Ca8JbFMI0DgF4pTspLy5tY
/QdVmL7rnDDEWJUXeIcW29cEXwnUTmGsQ4VvQHlQeiA6MxWwEmh+oEMCgYEAyLdm
MCYW1zhzsZ4tzLuL3itI0NHYN1AkUDbrW9wNBDXQ0D+Fg4yeTQZUQKbnHTXSS8mQ
GYFWKO+zRNp62dAZYzFsuCKywd0i2mszUS1aeJV75ouLrIpOpCHlaDpuU/+cuFPD
shsoYYM4SdwYJiFr86ty+BmuK8f2eRPJynDBIm0CgYAYYZeEP8xwwKqk6Q2er7yP
lXdXf/XGU3SJ0JR6d+4jUgiLc4ZnGNiNFqdyA2KQ3dQMxXIoRhc8FC0LeZrvDkNM
t23+KykDPOLv/qFlkorbKQmiYekO7zQGYfjXKQ9dDI9KqEUE3qwYVvHchOiZANfL
H/7U+LQEzQGus02hc5FivQKBgD78fo7hhQ1o47V7Y/f5arz/bExLbcR928doVJdx
CIynrq2LIYTUneCh+dKIDcK23xV1XUkDs1wJTIJw5Vhw22arXeiyytHXaVaNiX8R
8nAgyM7iP6SXinIrSOhaXcDyV+XoM2My7osbOJDFBrcAifXfeaatuGB40bnRIePr
WlBJAoGBAMdCMVl1VJfWMbK2+Qyji0dZ/DWv6N4FPMTRmn+/lk3OHKxR+y0anh3g
gNr0l75gsSI7mzfap41yZz91ScJq+yp5qPiaE/qMQTl0clBE7aD77DSUD8bXR78u
eZqYgn8yZwfIWHulG7zKOUMrg4rzGomP0rCe7j77Dt1veWxJaKI+
-----END RSA PRIVATE KEY-----
)KEY";

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

//funcion para encender el led amarillo, controlado por ALEXA
void ledOn() {
  digitalWrite(27, HIGH);
}

//funcion para apagar el led amarillo, controlado por ALEXA
void ledOff() {
  digitalWrite(27, LOW);
}

//Funcion que recibe el mensaje JSON que se publica en ucb/aws_iot_in, solo acepta un valor de en el documento JSON
StaticJsonDocument<JSON_OBJECT_SIZE(1)> inputDoc;

// PubSubClient callback function
void callback(const char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += String((char) payload[i]);
  }
  if (String(topic) == SUBCRIBE_TOPIC) {
    Serial.println("Message from topic " + String(SUBCRIBE_TOPIC) + ":" + message);

    //Se deserializa el mensaje en JSON
    DeserializationError err = deserializeJson(inputDoc, payload);
    if (!err) {
      String action = String(inputDoc["action"].as<char*>());

      //El mensaje "action":"OFF" y "action":"ON" son controlados por ALEXA
      if (action == "ON") ledOn();
      else if (action == "OFF") ledOff();   
      else if (action == "LED_RED") //Los mensahes "action":"LED_RED" y "action":"LED_GREEN" son controlados por las reglas en el IoT CORE de AWS
      {
         digitalWrite(14, HIGH); //Encender led rojo
         digitalWrite(25, LOW); //Apagar led verde
      }
      else if (action == "LED_GREEN") 
      {
         digitalWrite(25, HIGH);//Encender led verde
         digitalWrite(14, LOW);//Encender led rojo
      }
      else Serial.println("Unsupported action received: " + action);
    }
  }
}


//leer la distancia calculada por el sensor ulrasonico y devolver un valor en pulgadas
long readUltrasonicDistance(int triggerPin, int echoPin)
{
pinMode(triggerPin, OUTPUT); // Clear the trigger
digitalWrite(triggerPin, LOW);
delayMicroseconds(2);
// Sets the trigger pin to HIGH state for 10 microseconds
digitalWrite(triggerPin, HIGH);
delayMicroseconds(10);
digitalWrite(triggerPin, LOW);
pinMode(echoPin, INPUT);
// Reads the echo pin, and returns the sound wave travel time in microseconds
return pulseIn(echoPin, HIGH);
}


//Conectar al endpoint de IoTCORE de AWS
boolean mqttClientConnect() {
  Serial.println("Connecting to AWS " + String(AWS_ENDPOINT));
  if (mqttClient.connect(MQTT_CLIENT_ID)) {
    Serial.println("Connected to " + String(AWS_ENDPOINT));

    mqttClient.subscribe(SUBCRIBE_TOPIC);

    Serial.println("Subscribed to " + String(SUBCRIBE_TOPIC));
  } else {
    Serial.println("Can't connect to " + String(AWS_ENDPOINT));
  }
  return mqttClient.connected();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  pinMode(14, OUTPUT);
  pinMode(27, OUTPUT); 
  pinMode(25, OUTPUT);
  Serial.println("Connecting to " + String(WIFI_SSID));

  //Intenta conectarse al wifi con las cadenas establecidas al principio del codigo
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Can't connect to " + String(WIFI_SSID));
    while (1) delay (200);
  }
  Serial.println("Connected to " + String(WIFI_SSID));

  //Establece la comunicacion cifrada
  wifiClient.setCACert(AMAZON_ROOT_CA1);
  wifiClient.setCertificate(CERTIFICATE);
  wifiClient.setPrivateKey(PRIVATE_KEY);

  mqttClient.setServer(AWS_ENDPOINT, AWS_ENDPOINT_PORT);
  mqttClient.setCallback(callback);
}

unsigned long previousConnectMillis = 0;
unsigned long previousPublishMillis = 0;

unsigned char counter = 0;

//Publica datos en ucb/aws_iot_out en formato JSON los cuales son "id":"espdino32,"distancia":"(distancia_calculada)","value":("contador_de_envios")
StaticJsonDocument<JSON_OBJECT_SIZE(3)> outputDoc;
char outputBuffer[128];

void publishMessage(unsigned char value, String message) {
  
  outputDoc["id"] = "espdino32";
  outputDoc["distancia"] = message.c_str();
  outputDoc["value"] = value;
  
  serializeJson(outputDoc, outputBuffer);
  mqttClient.publish(PUBLISH_TOPIC, outputBuffer);
}


void loop() {
  unsigned long now = millis();
  if (!mqttClient.connected()) {
    // Connect to MQTT broker
    if (now - previousConnectMillis >= 4000) {
      previousConnectMillis = now;
      if (mqttClientConnect()) {
        previousConnectMillis = 0;
      } else delay(1000);
    }
  } else {
    // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
    mqttClient.loop();
    delay(20);

    if (now - previousPublishMillis >= 1000) {
      previousPublishMillis = now;
      // Publish message
      
      cm = 0.01723 * readUltrasonicDistance(17, 16); //convertir la distancia leida por el sensor a centimetros
      String message = String(cm);
      publishMessage(counter++, message);//enviar la distancia y el contador para su publicacion
    }
  }
}
