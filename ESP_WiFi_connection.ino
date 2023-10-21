/*
Program napisany przez grupę A Piątek Parzysty 11-14
Skład grupy:
Barbara Parzonka, Katarzyna Matuszek

Problemem tego programu jest połączenie systemu ESP8266 do Wifi i umożliwienie komunikacji tego urządzenia z użytkownikiem podłączonym do tej samej sieci.
W tym celu ESP zostaje webowym serwerem działającym na protokole transportu UDP. Adres strony, na którą powinien wejść chętny użytkownik zostanie wyświetlona na Serial Monitor.
Strona ta umomżliwia podanie sowjego imienia, na co użytkownik otrzyma w odpowiedzi personalizowane przywitanie.
Nazwa sieci i haslo dostepu, są opisane w odpowiednim miejscu w poniższym pliku. Jeśli korzystający z tego programu ma życzenie nazwać sieć w inny sposób lub zmienić hasło, serdecznie do tego zapraszamy. Aby to zrobić należy zmienić odpowiednie nazwy w cudzysłowiach
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char* ssid = "UWU"; //nazwa sieci, do ktorej ma zostac podlaczony WSP
const char* password = "KOCHAMUP"; //haslo do sieci

ESP8266WebServer server(80); //serwer na porcie 80, a wiec działający na protokole UDP

unsigned long wifiTimeOut=10000; //czas, po którym w przypadku nienawiązania połączenia, program ma się poddać i zwrócić informację o trudności z połączeniem


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
server.on("/imie", HTTP_POST, handleImie); //  gdy użytkownik wejdzie na stronę serweru w zakladke imie (IP/imie), przywoływana jest metoda handleImie()
server.onNotFound(handleNotFound);  


server.begin();
Serial.println("Usluga serwera HTTP zaincjowana, aby moc zobaczyc dane, podlacz sie do sieci i wejdz na strone: ");
 Serial.println(WiFi.localIP());
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
  server.send(200, "text/html", "Hello world!<br> W koncu moge przekazac ci istotne informacje<br>Jesli podasz mi swoje imie bede mogl sie z toba przywitac<br><form action=\"/imie\" method=\"POST\"><input type=\"text\" name=\"name\" placeholder=\"Tutaj wpisz swoje imie\"></br><input type=\"submit\" value=\"Przeslij\"></form>");   
}

/*
Strona zajmująca się odpowiedzią na podanie imienia (personalizowane przywitanie) lub w przypadku braku wcześniejszego podania imienia umożliwia podanie imienia
*/
void handleImie() {                         // If a POST request is made to URI /imie
  if( ! server.hasArg("name") || server.arg("name") == NULL ) { // If the POST request doesn't have name and password data
    server.send(400, "text/html", "Wybacz bezimienno osoba, przykro nam, ze nie poznalismy twojego imienia. Jesli zmnienisz zdanie, wciaz zapraszamy cie do podania nam swojego imienia<br><form action=\"/imie\" method=\"POST\"><input type=\"text\" name=\"name\" placeholder=\"Tutaj wpisz swoje imie\"></br><input type=\"submit\" value=\"Przeslij\"></form>""");         // The request is invalid, so send HTTP status 400
    return;
  }
  else { // If both the name and the password are correct
    server.send(200, "text/html", "<h1>Witaj, " + server.arg("name")+"! Milo mi moc wymieniac z Toba dane");
  } 
}
/*
Odpowiedz serwera w innym przypadku
*/
void handleNotFound(){
  server.send(404, "text/plain", "404: Not found");
}