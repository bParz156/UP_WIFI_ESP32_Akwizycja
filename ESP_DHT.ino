/*
Program napisany przez grupę A Piątek Parzysty 11-14
Skład grupy:
Barbara Parzonka, Katarzyna Matuszek

Problemem tego programu jest połączenie systemu ESP8266 do Wifi pasmo 2,4 GHz i 
umożliwienie komunikacji tego urządzenia z użytkownikiem podłączonym do tej samej sieci.
W tym celu ESP zostaje webowym serwerem działającym na protokole transportu UDP.
Adres strony, na którą powinien wejść chętny użytkownik zostanie wyświetlona na Serial Monitor.
Strona ta pokazuje w formie wykresów dane pomiarowe uzyskane w procesie akwizycji danych.
Nazwa sieci i haslo dostepu, są opisane w odpowiednim miejscu w poniższym pliku. 
Jeśli korzystający z tego programu ma życzenie nazwać sieć w inny sposób lub zmienić hasło, 
serdecznie do tego zapraszamy. Aby to zrobić należy zmienić odpowiednie nazwy w cudzysłowiach
*/
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include<iostream>
//#include "FreeRTOS.h"
//#include "freertos/task.h"

using namespace std;

#define DHTPIN 2
#define DHTTYPE    DHT11

const char* ssid = "UWU"; //nazwa sieci, do ktorej ma zostac podlaczony WSP
const char* password = "KOCHAMUP"; //haslo do sieci

ESP8266WebServer server(80); //serwer na porcie 80, a wiec działający na protokole UDP

unsigned long wifiTimeOut=10000; //czas, po którym w przypadku nienawiązania połączenia, program ma się poddać i zwrócić informację o trudności z połączeniem
unsigned long timeInterval=10000; // czas opoznienia (miedzy kolejnymi odczytami z czujnika)

DHT dht(DHTPIN, DHTTYPE); //sensor używany do pomiarów
float t_actual = 0.0;
float h_actual=0.0;

/*
Funkcja ma za zadanie polaczenie do wybranej sieci wifi jako rownoczesnie AccesSPoint i stacja
@return true jeśli udało się nawiązać połączenie zanim upłynął limit czasu, w przeciwnym wypadku zwraca false
Poprzez narzędzie Serial Monitor można monitorować statsu nawiązywania połączenia.

*/
bool initWiFi() {
 WiFi.mode(WIFI_AP_STA);
 Serial.println("Zaczynam probe polaczenia do WiFi");


 WiFi.begin(ssid, password);
 Serial.print("Polaczenie do ");
 Serial.print(ssid);
 Serial.println("...");

  unsigned long startTime=millis();

 while (WiFi.status() != WL_CONNECTED && (millis()-startTime)<wifiTimeOut) {
   Serial.print('.');
   delay(100);
 }

  if(WiFi.status()==WL_CONNECTED)
  {
    Serial.println("\n Udalo sie nawiazac polaczenie! \n");// Twoje IP to: "+WiFi.localIP()+"\n");
  Serial.print("Twoje IP to: ");
  Serial.println(WiFi.localIP());
  WiFi.setAutoReconnect(true);
  return true;
  }

  Serial.println("\n Nie udalo sie polaczyc do Wifi \n");
  return false;
 
}
/*
Wszystkie trzy procedury są handlerami serweru http
Dokładnijeszy opis poniższych procedur został zamieszczony przy ciałach poszcególnych procedur.
*/
void handleRoot();             
void handleNotFound();
void handlePomiar();
void pomiar();

/*
Procedura wykonuje się tylko raz.
Rozpoczyna działanie Serial Monitora.
Wywołuje funckję initWifi() celem nawiązania połączenia WiFi
Zmienna lokalna numer_proby wskazuje na ilosc nieudanych prób nawiązania połączenia. 
Jeśli urządzenie bezskutecznie próbowało podłączyć się do WiFi trzy razy, użytkownik jest informowany o trudności z połączeniem i proszony o sprawdzenie dostępności sieci, do której ma zostać podłączone urządzenie.
W razie gdy połączenie zostanie nawiązane do trzeciej próby włącznie, zwracana jest informacja o nawiązaniu połączenia i podawane jest IP.
Wówczas uruchamiany jest serwer HTTP
*/
bool polacz_Internet()
{
   int numer_proby=0;
while(!initWiFi() && numer_proby<3)
{
  numer_proby++;
}

if(WiFi.status()==WL_CONNECTED)
{
server.on("/", handleRoot);               //  gdy użytkownik wejdzie na stronę serweru (IP/), przywoływana jest metoda handleRoot()
server.on("/pomiar", HTTP_GET, handlePomiar); //  gdy użytkownik wejdzie na stronę serweru w zakladke imie (IP/imie), przywoływana jest metoda handleImie()
server.onNotFound(handleNotFound);  

server.begin();
Serial.println("Usluga serwera HTTP zaincjowana, aby moc zobaczyc dane, podlacz sie do sieci i wejdz na strone: ");
 Serial.println(WiFi.localIP());
 return true;
}
return false;
}



void setup() {
 // put your setup code here, to run once:
 Serial.begin(115200);

 if(initWiFi())
 {
  dht.begin();
  if(WiFi.status()==WL_CONNECTED)
{
server.on("/", handleRoot);               //  gdy użytkownik wejdzie na stronę serweru (IP/), przywoływana jest metoda handleRoot()
server.on("/pomiar", HTTP_GET, handlePomiar); //  gdy użytkownik wejdzie na stronę serweru w zakladke imie (IP/imie), przywoływana jest metoda handleImie()
server.onNotFound(handleNotFound);  

server.begin();
Serial.println("Usluga serwera HTTP zaincjowana, aby moc zobaczyc dane, podlacz sie do sieci i wejdz na strone: ");
 Serial.println(WiFi.localIP());
}
 }

else {
Serial.println("Pomimo wielu prob nie udalo sie podlaczyc do wskazanego WiFi. Przykro nam. Sprawdz czy siec, do ktorej chcesz sie podlaczyc jest na pewno wlaczona i sprobuj ponownie");
}

 
}



/*
Program działający w pętli przez cały czas uruchomienia urządzenia ESP (po zakończeniu procedury setup())
Jego celem jest obsługa przez serwer HTTP klientów
*/
void loop() {
 
 // xTaskCreate(pomiar, "pomiar", 256, NULL, 1, NULL);
 // xTaskCreate(toHtml, "html", 256, NULL, 1, NULL);
 pomiar();
 server.handleClient();
 delay(10000);
  

}


void toHtml()//void *arg)
{
  server.handleClient();
}

/*
Strona "startowa", umożliwia użytkownikowi podanie imienia
*/

void handleRoot() {
  server.send(200, "text/plain", "Temperatura"+String(t_actual)+"C Wilgotnosc: "+String(h_actual)+" %");
}
/*
"<head>\n" +
"  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n" +
"  <link rel=\"stylesheet\" href=\"https://use.fontawesome.com/releases/v5.7.2/css/all.css\" integrity=\"sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr\" crossorigin=\"anonymous\">\n" +
"  <style>\n" +
"    html {\n" +
"     font-family: Arial;\n" +
"     display: inline-block;\n" +
"     margin: 0px auto;\n" +
"     text-align: center;\n" +
"    }\n" +
"    h2 { font-size: 3.0rem; }\n" +
"    p { font-size: 3.0rem; }\n" +
"    .units { font-size: 1.2rem; }\n" +
"    .dht-labels{\n" +
"      font-size: 1.5rem;\n" +
"      vertical-align:middle;\n" +
"      padding-bottom: 14px;\n" +
"    }\n" +
"  </style>\n" +
"</head>\n" +
"<body>\n" +
"  <h2>Odczyt temperatury z czujnika DHT przy uzyciu ESP8266</h2>\n" +
"  <p>\n" +
"    <i class=\"fas fa-thermometer-half\" style=\"color:#059e8a;\"></i> \n" +
"    <span class=\"dht-labels\">Temperature</span> \n" +
"    <span id=\"temperature\">%TEMPERATURE%</span>\n" +
"    <sup class=\"units\">&deg;C</sup>\n" +
"  </p>\n" +
"  <p>\n" +
"    <i class=\"fas fa-tint\" style=\"color:#00add6;\"></i> \n" +
"    <span class=\"dht-labels\">Humidity</span>\n" +
"    <span id=\"humidity\">%HUMIDITY%</span>\n" +
"    <sup class=\"units\">%</sup>\n" +
"  </p>\n" +
"</body>\n" +
"<script>\n" +
"setInterval(function ( ) {\n" +
"  var xhttp = new XMLHttpRequest();\n" +
"  xhttp.onreadystatechange = function() {\n" +
"    if (this.readyState == 4 && this.status == 200) {\n" +
"      document.getElementById(\"temperature\").innerHTML = this.responseText;\n" +
"    }\n" +
"  };\n" +
"  xhttp.open(\"GET\", \"/pomiar\", true);\n" +
"  xhttp.send();\n" +
"}, 10000 ) ;\n" +
"\n" +
"</script>\n" +
"</html>");
}
*/
void handlePomiar() {
  //String s=pomiar();
  String s="Temperatura"+String(t_actual)+" C \n Wilgotnosc "+String(h_actual);
  
  server.send(200, "text/plain", s.c_str());
}


/*
Odpowiedz serwera w innym przypadku
*/
void handleNotFound(){
  server.send(404, "text/plain", "404: Not found");
}


/*
Funkcja zwraca temperture w formie tekstowej (stopnie celsjusza)
*/

void pomiar()//void *arg)
{
  getTemperature();
  getHumidity();
  String info="Temperatura"+String(t_actual)+" C" +"Wilgotnosc "+String(h_actual)+" %";
  Serial.println(info);
  //vTaskDelay(timeInterval);

}


void getTemperature()
{
  float t=dht.readTemperature();
  t_actual=t;
  if (isnan(t)) {    
    Serial.println("Failed to read from sensor!");
  //  return "";
  }
  else {
//    return String(t);
  }
}

void getHumidity()
{
  float h=dht.readHumidity();
  h_actual=h;
  if (isnan(h)) {    
    Serial.println("Failed to read from sensor!");
  //  return "";
  }
  else {
 //   return String(h);
  }
}
