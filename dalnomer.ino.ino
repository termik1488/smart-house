
#include <ESP8266WiFi.h>
const char* ssid = "TP-LINK-2";
const char* password = "12340000";
WiFiServer server(80);
unsigned long lastMillis = 0;
//Определяем и задаем к каким контактам (D7, D6)модуля микроконтроллера ESP-8266 будут подключены контакты (Echo, Trigger) ультразвукового датчика HC-SR04
#define echoPin D7 // Echo — входной сигнал эха от модуля ультразвукового датчика
// HC-SR04
#define trigPin D6 // Trigger — выходной сигнал запуска внутреннего микроконтроллера
// датчика HC-SR04 поступающий от модуля ESP-8266

long duration, distance; /* Задаём переменную duration — значение длины принимаемого импульса со входа Echo — эхо от модуля ультразвукового датчика HC-SR04; и переменную distance — вычисленное значение расстояния от датчика до предмета от которого отразился ультразвуковой импульс. long — переменная длиной 32 бита со знаком (4 байта), может принимать значения от -2147483648 до 2147483647 */

void setup() {
	Serial.begin(115200);
	
  pinMode(trigPin, OUTPUT);/* задаем режим OUTPUT- «Выход» контакту D6 модуля ESP-8266 с именем в нашей программе trigPin */
pinMode(echoPin, INPUT);/* задаем режим INPUT- «Вход» контакту D7 модуля ESP-8266 с именем в нашей программе echoPin */
// Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}


void loop() {

 
digitalWrite(trigPin, LOW);/*С помощью функции digitalWrite() подаем на цифровой вход/выход (pin) (контакт D6 модуля ESP-8266 переменная в программе trigPin) значение логического нуля LOW — 0 Вольт (земля).*/

delayMicroseconds(2); /*задаем длительность логического нуля на trigPin 2 микросекунды*/

digitalWrite(trigPin, HIGH);/*Подаем на цифровой вход/выход (pin) (контакт D6 модуля ESP-8266 переменная в программе trigPin) значение логической единицы HIGH — 5 Вольт (3.3 Вольт для 3.3 Вольт-вых  плат).*/

delayMicroseconds(10);/*задаем длительность логической единицы на trigPin 10 микросекунд */

digitalWrite(trigPin, LOW);/*Подаем на цифровой вход/выход(pin)(контакт D6 модуля ESP-8266 переменная в программе trigPin)значение логического нуля LOW — 0 Вольт (земля).*/

duration = pulseIn(echoPin, HIGH);/*Присваиваем переменной duration значение HIGH длительности принимаемого импульса с контакта D7 модуля ESP-8266 (переменная в программе echoPin) то есть принимаем усиленный и преобразованный датчиком HC-SR04 импульс пропорциональный дистанции до обнаруженного объекта ультразвуковым датчиком */

distance = duration/58.2;/*  Вычисляем по эмпирической формуле дистанцию до обнаруженного объекта (в см), опираясь на скорость звука в воздухе  изменяя значение коэффициента 58.2 можем скорректировать  линейную погрешность ультразвукового датчика*/

Serial.println(distance);/* Передаем в «Монитор порта» дистанцию до обнаруженного объекта (в см) через последовательный порт как ASCII текст с следующим за ним символом переноса строки (ASCII символ 13 или ‘\r’) и символом новой строки (ASCII 10 или ‘\n’) */

delay(1000);
 // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  int val;
  if (req.indexOf("/gpio/0") != -1)
    val = 0;
  else if (req.indexOf("/gpio/1") != -1)
    val = 1;
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  // Set GPIO2 according to the request
  digitalWrite(2, val);
  
  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\ndistance= ";
 s += duration/58.2;
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");


}


