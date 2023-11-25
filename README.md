### Urządzenia Peryferyjne - Akwizycja danych WiFi, układ ESP8266 typu SoC
## Opis
Celem ćwiczenia było napisanie aplikacji dla ESP8266 (i/lub ESP32), która ma umożliwiać odczyt danych z czujnika DHT11 i ich reprezentację dla użytkownika. Należało użyć systemu operacyjnego czasu rzeczywistego FreeRTOS 
## Technologie
Arduino, ESP8266, ESP32, DHT11, FreeRTOS
## Uruchomienie
1.Skonfigurować router zgodnie z danymi wskazanymi w opisie programu:
Pasmo 2.4 GHz, nazwa sieci ssid="UWU", hasło="KOCHAMUP". 
Minimalna wielkość sieci to 2 urządzenia (jednak jej wielkość zależy od tego, jak dużo użytkowników chce uzyskać dostęp do uzyskiwanych danych). 
Proponuje się ustawienie maski sieci na /29 (255.255.255.248).
Działanie sieci wymaga: routera, układu ESP8266 (lub ESP32), czujnika DHT11 i urządzenia klienta, który chce uzyskać dane od serwera będącego ESP. <br>
2. Zgranie programu (plik: _esp01_akwizycja.ino_) na urządzenie ESP8266 (lub w przypadku urządzenie ESP32 plik: _esp_32_akwizycja_FreeRTOS.ino_)
## Działanie programu
Działanie programu jest szcegółowo opisane w komentarzach w pliku _esp_32_akwizycja_FreeRTOS.ino_ 
Urządzenie próbuje nawiązać połączenie WiFi z siecią, której kluczowe dane przedstawiono powyżej. W przypadku trzech nieudanych prób, użytkownik informowany jest o problemie i proszony o sprawdzenie dostępności sieci. 
Jeśli połączenie zostanie nawiązane, podawane jest IP urządzenia, a nastęnie uruchamiany jest serwer umożliwiający podgląd odczytywanych przez czujnik danych
Klient chcący poznać dane, wchodzi na stronę, której adres wyświetlony został na serial monitorze. Strona automatycznie odświeża się, tak aby pokazać użytkownikowi aktualne dane pomiarowe.
## Autorzy
Autorkami programu są Barbara Parzonka i Katarzyna Matuszek
