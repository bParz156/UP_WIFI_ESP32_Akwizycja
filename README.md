### Urządzenia Peryferyjne - Akwizycja danych WiFi, układ ESP32 typu SoC
## Opis
Celem ćwiczenia było napisanie aplikacji dla ESP32, która ma umożliwiać odczyt danych z czujnika i jego reprezentację dla użytkownika
## Technologie
Arduino, ESP8266
## Uruchomienie
1.Skonfigurować router zgodnie z danymi wskazanymi w opisie programu:
Pasmo 2.4 GHz, nazwa sieci ssid="UWU", hasło="KOCHAMUP". Działanie sieci wymaga: routera, ESP8266 i urządzenia klienta, który chce uzyskać dane od serwera będącego ESP. <br>
2. Zgranie programu (plik _nazwa_pliku_) na urządzenie ESP8266
## Działanie programu
Działanie programu jest szcegółowo opisane w komentarzach w pliku _nazwa_pliku_ 
Urządzenie ESP8266 próbuje nawiązać połączenie WiFi z siecią, której kluczowe dane przedstawiono powyżej. W przypadku trzech nieudanych prób, użytkownik informowany jest o problemie i proszony o sprawdzenie dostępności sieci. 
Jeśli połączenie zostanie nawiązane, podawane jest IP urządzenia, a nastęnie uruchamiany jest serwer umożliwiający podgląd odczytywanych przez czujnik danych
## Autorzy
Autorkami programu są Barbara Parzonka i Katarzyna Matuszek
