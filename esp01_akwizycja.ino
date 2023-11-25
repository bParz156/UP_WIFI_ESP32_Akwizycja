#include <ESP8266WiFi.h>
#include <DHT.h>
#include<iostream>


//tylko do losowego
#include <cstdlib>
#include <ctime>


using namespace std;

#define DHTPIN 2
#define DHTTYPE    DHT11

const char* ssid = "UWU"; //nazwa sieci, do ktorej ma zostac podlaczony WSP
const char* password = "KOCHAMUP"; //haslo do sieci

WiFiServer server(80); //serwer na porcie 80, a wiec działający na protokole UDP

unsigned long wifiTimeOut=10000; //czas, po którym w przypadku nienawiązania połączenia, program ma się poddać i zwrócić informację o trudności z połączeniem
unsigned long timeInterval=10000; // czas opoznienia (miedzy kolejnymi odczytami z czujnika)
char *dt;

DHT dht(DHTPIN, DHTTYPE); //sensor używany do pomiarów
float t_actual = 0.0;
float h_actual=0.0;

String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
String html_1 = R"=====(
<!DOCTYPE html>
<html>
 <head>
 <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
 <meta charset='utf-8'>
 <style>
   body {font-size:100%;} 
   #main {display: table; margin: auto;  padding: 0 10px 0 10px; } 
   h2 {text-align:center; } 
   p { text-align:center; }
 </style>
 <script>
  function refresh(refreshPeriod) 
  {
      setTimeout("location.reload(true);", refreshPeriod);
  } 
  window.onload = refresh(5000);
 </script>
   <title>Serwer pogodowy ESP8266</title>
 </head>
 
 <body>
   <div id='main'>
     <h2>Serwer Pogodowy ESP8266</h2>
     <p>Strona aktualizuje sie co 5 sekund</p>
     <div id='dane'> 
       <p>Temperatura = %temperatura% C</p>
       <p>Wilgotność= %wilgotnosc% %</p>
       <p>Czas pomiaru= %czas% s od początku pobierania danych<p>
     </div>
   </div> 
 </body>
</html>
)====="; 


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
  if(WL_CONNECTED)
  {
    pomiar();
    obsuga_klienta();
  }
  delay(timeInterval);

}


void obsuga_klienta()
{
  WiFiClient client = server.available();
  if(client.connected())
  {
    String tmp=html_1;
    tmp.replace("%temperatura%", String(t_actual));
    tmp.replace("%wilgotnosc%", String(h_actual));
    unsigned long cz=millis()/1000;
    tmp.replace("%czas%", String(cz));
    client.flush();
    client.print(header);
    client.print(tmp);
  }
}




/*
Funkcja zwraca zapamiętane temperture i wilgoc w formie tekstowej
*/
String dane_pomiarowe()
{
  return"Temperatura: "+String(t_actual)+" C \n" +"Wilgotnosc: "+String(h_actual)+" %";
}


/*
Funckja odpowiedzialna za realizacje pomiaru
*/
void pomiar()
{
    getTemperature();
    getHumidity();
    Serial.println(dane_pomiarowe());

}


void getTemperature()
{
  
  float t=dht.readTemperature();
  t_actual=t;
  if (isnan(t)) {    
    Serial.println("Nie udalo sie odczytac temperatury!");
  }
  
  //t_actual=wylosuj();
  
}

void getHumidity()
{
  
  float h=dht.readHumidity();
  h_actual=h;
  if (isnan(h)) {    
    Serial.println("Nie udalo sie odczytac wilgoci!");
  }
  
  //h_actual=wylosuj();

}


float wylosuj()
{
  srand((unsigned int)time(NULL));
  return float(rand())/float((RAND_MAX))*20;
}
