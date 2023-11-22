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
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE    DHT11

const char* ssid = "UWU"; //nazwa sieci, do ktorej ma zostac podlaczony WSP
const char* password = "KOCHAMUP"; //haslo do sieci

ESP8266WebServer server(80); //serwer na porcie 80, a wiec działający na protokole UDP

unsigned long wifiTimeOut=10000; //czas, po którym w przypadku nienawiązania połączenia, program ma się poddać i zwrócić informację o trudności z połączeniem
unsigned long timeInterval=1000; // czas opoznienia (miedzy kolejnymi odczytami z czujnika)
unsigned long previousMillis = 0;

DHT dht(DHTPIN, DHTTYPE); //sensor używany do pomiarów
float t = 0.0;

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
void handleImie();

/*
Procedura wykonuje się tylko raz.
Rozpoczyna działanie Serial Monitora.
Wywołuje funckję initWifi() celem nawiązania połączenia WiFi
Zmienna lokalna numer_proby wskazuje na ilosc nieudanych prób nawiązania połączenia. 
Jeśli urządzenie bezskutecznie próbowało podłączyć się do WiFi trzy razy, użytkownik jest informowany o trudności z połączeniem i proszony o sprawdzenie dostępności sieci, do której ma zostać podłączone urządzenie.
W razie gdy połączenie zostanie nawiązane do trzeciej próby włącznie, zwracana jest informacja o nawiązaniu połączenia i podawane jest IP.
Wówczas uruchamiany jest serwer HTTP
*/
void setup() {
 // put your setup code here, to run once:
 Serial.begin(115200);
 delay(1000); //Opoznienie 1sekundowe ma na celu mozliwosc zobaczenia wszytskiego co przesylane jest na serial monitor
 int numer_proby=0;
while(!initWiFi() && numer_proby<3)
{
  numer_proby++;
}

if(WiFi.status()==WL_CONNECTED)
{
server.on("/", handleRoot);               //  gdy użytkownik wejdzie na stronę serweru (IP/), przywoływana jest metoda handleRoot()
server.on("/temperature", HTTP_GET, handleTemp); //  gdy użytkownik wejdzie na stronę serweru w zakladke imie (IP/imie), przywoływana jest metoda handleImie()
server.onNotFound(handleNotFound);  

server.begin();
Serial.println("Usluga serwera HTTP zaincjowana, aby moc zobaczyc dane, podlacz sie do sieci i wejdz na strone: ");
 Serial.println(WiFi.localIP());

dht.begin();
 
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
 server.handleClient();
}

/*
Strona "startowa", umożliwia użytkownikowi podanie imienia
*/

void handleRoot() {
  server.send(200, "index.html");
}

void handleTemp() {
  String s=getTemperature();
  
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
String getTemperature()
{
  float t=dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("Failed to read from sensor!");
    return "";
  }
  else {
    return String(t);
  }
}


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>Odczyt temperatury z czujnika DHT przy uzyciu ESP8266</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">%</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(t);
  }
  return String();
}



String id_html="<!DOCTYPE HTML><html>\n" +
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
"      padding-bottom: 15px;\n" +
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
"  xhttp.open(\"GET\", \"/temperature\", true);\n" +
"  xhttp.send();\n" +
"}, 10000 ) ;\n" +
"\n" +
"</script>\n" +
"</html>";
