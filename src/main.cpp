#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>                                                                                     // biblioteka do karty SD
#include "/Users/wojciechkubiak/Documents/PlatformIO/Projects/test-stand-comunication/lib/HX711-master/HX711.h" // biblioteka do hamowni

#define DOUT 3
#define CLK 2
#define LED 15
#define PYRO 16
#define BUZZER 17

///////////////////   --------------   SETTING !!!   -----------------    ///////////////////////////

#define CALIB_FACTOR 9600 // <----------- tu musisz wpisac wspolczynnik kalibracji

#define TIME_FOR_RUN 5000 // czas na ucieczke liczony w [ms]

#define FIRE 5000 // czas przez ktorey bedzie sie palic grzala w [ms]

//////////////////------------------------------------------------------------/////////////////////////////

HX711 scale(DOUT, CLK);
File plik;
SoftwareSerial mySerial(9, 10); //RX, TX

int temp_time = 0;
float trust = 0;
bool triger = true;

void logger(float);

void setup()
{
  Serial.begin(9600);   //uruchom UART o prędkości 9600 baud
  mySerial.begin(9600); //urochomienie transmisji z HC12

  pinMode(LED, OUTPUT);   // definicja leda
  digitalWrite(LED, LOW); // led wylaczony

  pinMode(PYRO, OUTPUT);   //definicja zapalnika
  digitalWrite(PYRO, LOW); //zapalnik wylaczony

  pinMode(BUZZER, OUTPUT); //definicja buzzera

  scale.set_scale(CALIB_FACTOR); // ustawienie wspolczynnika kalibracji
  scale.tare();                  //taruj wage

  if (!SD.begin(4)) //sprawdź czy nie ma karty na pinie ChipSelect 4
  {
    Serial.println("Nie wykryto karty(ERR)"); //błąd wykrycia karty
    while (1)
      ; // zatrzymaj program
  }

  Serial.println("Karta Wykryta (3/3))"); //Karta wykryta
  digitalWrite(LED, HIGH);

  if (SD.exists("readouts.txt")) //sprawdź czy istnieje plik o nazwie readouts.txt
  {

    Serial.println("Plik o podanej nazwie istnieje !");
  }

  else //jeżeli nie istnieje to
  {

    plik = SD.open("readouts.txt", FILE_WRITE); //utwórz plik
    Serial.println("Utworzono plik o nazwie readouts.txt");
  }

  //
  //delay(3000);
  while (triger)
  {
    //   if (mySerial.available() > 1)
    //   { //Jesli sa dane do odczytu
    //     String password = mySerial.readString();
    //     Serial.println(password);
    if (mySerial.available() > 1)
    {
      Serial.println("strzelamy");
      triger = false;
    }
    // }
  }
  // while (millis() < TIME_FOR_RUN)
  // {
  //   tone(17, 2000, 500); // wlacz buzzer na 0.5s
  //   delay(1000);
  // }

  temp_time = millis(); //aktualny czas sytsemu
  if ((millis() - temp_time) > TIME_FOR_RUN)
  {
    tone(17, 2000, 500); // wlacz buzzer na 1.0s
    delay(970);
  }

  digitalWrite(PYRO, HIGH);
  temp_time = millis(); //aktualny czas sytsemu
}

void loop()
{
  if ((millis() - temp_time) > FIRE)
  {
    digitalWrite(PYRO, LOW);
  }

  trust = scale.get_units();
  Serial.print("trust: ");
  Serial.print(trust);
  Serial.println(" N");
  Serial.println();

  mySerial.println(trust); //wyslij ciag
  delay(3000);
  logger(trust); //zapisz na karte
}

void logger(float trust)
{

  plik = SD.open("readouts.txt", FILE_WRITE); //otwórz plik readouts.txt
  plik.println(trust);                        //zapisz dane
  plik.close();                               //zamknij/zapisz plik
  delay(150);
  Serial.println("Zapisano !"); //poinformuj o zapisaniu pliku
}