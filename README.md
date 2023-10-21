### Urządzenia Peryferyjne - Transmisja WiFi, układ ESP32 typu SoC
## Opis
Celem ćwiczenia było napisanie aplikacji dla ESP32, która ma umożliwiać:
* aktywację i wyłączenie urządzenia
* uruchomienie oprogramowania realizującego transmisję WiFi
* realizację połączenia z wybranym urządzeniem WiFi
* przesłanie ciągu znaków (danych) za pomocą WiFi
## Technologie
Arduino, ESP8266
## Uruchomienie
1.Skonfigurować router zgodnie z danymi wskazanymi w opisie programu:
Pasmo 2.4 GHz, nazwa sieci ssid="UWU", hasło="KOCHAMUP". Działanie sieci wymaga: routera, ESP8266 i urządzenia klienta, który chce uzyskać dane od serwera będącego ESP.
2. Zgranie programu (plik _ESP_WIFI_connection.ino_) na urządzenie ESP8266
## Działanie programu
Działanie programu jest szcegółowo opisane w komentarzach w pliku _ESP_WIFI_connection.ino_ 
Urządzenie ESP8266 próbuje nawiązać połączenie WiFi z siecią, której kluczowe dane przedstawiono powyżej. W przypadku trzech nieudanych prób, użytkownik informowany jest o problemie i proszony o sprawdzenie dostępności sieci. 
Jeśli połączenie zostanie nawiązane, podawane jest IP urządzenia, a nastęnie uruchamiany jest serwer umożliwiający przesyłaniu ciągu znaków (danych) za pomocą WiFi
## Autorzy
Autorkami programu są Barbara Parzonka i Katarzyna Matuszek
