#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

String mensajeAct;
byte sensor1 = 4; //PIN Digital 2
byte sensor2 = 0; //PIN Digital 3
byte sensor3 = 2; //PIN Digital 4

bool bandSensor1 = false;
bool bandSensor2 = false;
bool bandSensor3 = false;


//====================================
//           WIFICONFIG 
//====================================
const char* ssid = "INFINITUM3781";
const char* password =  "FLdzbe092r";
 

//====================================
//             MQTT CONFIG 
//====================================

//const char *mqtt_server = "192.168.1.88";
const char *mqtt_server = "35.239.161.254";
const int mqtt_port = 1883;
const char *root_topic_subscribe = "/casa/pir/input";
const char *root_topic_publish = "/cada/pir/output";
const char *mqtt_user = "";
const char *mqtt_pass = "";


//====================================
//               GLOBALES   
//====================================
WiFiClient espClient;
PubSubClient client(espClient);


//====================================
//           F U N C I O N E S 
//====================================
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();

 
void setup() {

  pinMode(sensor1,INPUT);
  pinMode(sensor2,INPUT);
  pinMode(sensor3,INPUT);
  
  //Serial.begin(9600);
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

}


void loop() {

    mensajeAct="";
    if(client.connected() == true && bandSensor1 == true){
      mensajeAct +=  Sensor1();
    }
    
    if(client.connected() == true && bandSensor2 == true){
      if(mensajeAct == ""){
        mensajeAct+=Sensor2();
      }else{
       mensajeAct+=","+Sensor2();
      }
    }
    
    if(client.connected() == true && bandSensor3 == true){
      if(mensajeAct == ""){
        mensajeAct+=Sensor3();
      }else{
       mensajeAct+=","+Sensor3();
      }
    }

  if(client.connected() == true && mensajeAct != ""){  
    client.publish(root_topic_publish, mensajeAct.c_str());
  }
  


  if (!client.connected()) {
    Serial.print("Reconectando ...");
    reconnect();
 
 }
  delay(1000);
  client.loop();
}



//====================================
//           CONEXION WIFI      
//====================================
void setup_wifi(){
  delay(10);
  // Nos conectamos a nuestra red Wifi
  Serial.println();
  Serial.print("Conectando a ssid: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a red WiFi!");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}


//====================================
//             CONEXION MQTT      
//====================================

void reconnect() {
  
  while (!client.connected()) {
    Serial.print("Intentando conexión Mqtt...");
    // Creamos un cliente ID
    String clientId = "ESP-PIR-UV_";
    clientId += String(random(0xffff), HEX) ;
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!");
      // Nos suscribimos
      
      if(client.subscribe(root_topic_subscribe)){
        Serial.println("Suscripcion ok");
      }else{
        Serial.println("fallo Suscripciión");
      }
    } else {
      Serial.print("falló 🙁 con error ===> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}


//====================================
//              CALLBACK        
//====================================

void callback(char* topic, byte* payload, unsigned int length){


  byte* p = (byte*)malloc(length);
   memcpy(p,payload,length);

  
  Serial.print("Se recibió mensaje:  ");
  Serial.print(topic);
  Serial.println("");

  String incoming = (char*)p;
  incoming.trim();

  String incomingVaidated = "";
  for (int i = 0; i <= 1; i++){
    incomingVaidated += (char)incoming[i];
 
  }
  Serial.println("Mensaje  ==>  " + incomingVaidated + "\n\n");



//======================================
// APAGADO/ENCENDIDO TODO LOS SENSORES
//======================================
  if(incomingVaidated == "1"){
    bandSensor1 = true;
    bandSensor2 = true;
    bandSensor3 = true;
  }
  
  if(incomingVaidated == "0"){
    bandSensor1 = false;
    bandSensor2 = false;
    bandSensor3 = false;
  }

//============================
// APAGADO/ENCENDIDO SENSOR1
//============================
  if(incomingVaidated == "11"){
    bandSensor1 = true;
  }
  
  if(incomingVaidated== "01"){
    bandSensor1 = false;
  }

//============================
// APAGADO/ENCENDIDO SENSOR2
//============================

if(incomingVaidated == "12"){
    bandSensor2 = true;
  }
  
  if(incomingVaidated == "02"){
    bandSensor2 = false;
  }
//============================
// APAGADO/ENCENDIDO SENSOR3
//============================

if(incomingVaidated == "13"){
    bandSensor3 = true;
  }
  
  if(incomingVaidated == "03"){
    bandSensor3 = false;
  }


 free(p);
}


//====================================
//        CONEXIÓN DE SENSORES      
//====================================

String Sensor1(){
  //=============================
  //        SENSOR PIR 1
  //=============================

  if(digitalRead(sensor1) == HIGH){
    Serial.println("Movimiento Dectectado en el sensor 1....");
    delay(1000);

     return "S1:true";
  }else{
    Serial.println("Sin movimiento en el sensor 1");
    delay(1000);
    return "S1:false";
  }
  
}

String Sensor2(){
  //=============================
  //        SENSOR PIR 2
  //=============================

  if(digitalRead(sensor2) == HIGH){
    Serial.println("Movimiento Dectectado en el sensor 2....");
    delay(1000);
    
    return "S2:true";
  }else{
    Serial.println("Sin movimiento en el sensor 2");
    delay(1000);
    return "S2:false";
  }
  
}

String Sensor3(){
  //=============================
  //        SENSOR PIR 3
  //=============================

  if(digitalRead(sensor3) == HIGH){
    Serial.println("Movimiento Dectectado en el sensor 3....");
    delay(1000);

    return "S3:true" ;
  }else{
    Serial.println("Sin movimiento en el sensor 3");
    delay(1000);
    return "S3:false";
  }
  
}
