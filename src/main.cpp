/* jeszcze nie skończony sterownik świateł w łazience na arduino nano*/

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int wlamanie = 2;          // pin 2 jako antywłamanie
const int czujka = 4;            // pin 4 jako czujka PIR
const int krancowkaDrzwi = 7;    // pin 7 jako wejście krańcówki
const int wlacznikGora = 10;     // pin 10 jako wejście przycisku œiatła 1
const int wlacznikLustro = 11;   // pin 10 jako wejście przycisku œiatła 2
const int wlacznikPrysznic = 12; // pin 10 jako wejście przycisku œiatła 3
const int Up = A7;               // wejście przycisku UP
const int Down = A6;             // wejscie przycisku DOWN
const int Ok = A3;               // wejście przycisku OK
// const int swiatloGora = 13;                   //wyjście światło 1 do debugowania
const int swiatloGora = A0;     // wyjście światło góra
const int swiatloLustro = A1;   // wyjście światło nad lustrem
const int swiatloPrysznic = A2; // wyjście światło nad prysznicem

boolean stanWlamania;                // zmienna do której będzie wpisywana wartość odczytana z wejścia antywłamaniowego
boolean stanWlacznikaGora;           // zmienna do której wpisywany jest stan włącznika światła głównego
boolean stanWlacznikaLustro;         // zmienna do której wpisywany jest stan włącznika światła nad lustrem
boolean stanWlacznikaPrysznic;       // zmienna do której wpisywany jest stan włącznika światła nad prysznicem
boolean stanKrancowki;               // zmienna do której wpisywany jest stan włącznika krańcowego przy drzwiach
boolean stanCzujki;                  // zmienna do której wpisywany jest stan uruchomionej czujki
boolean uzytkownikWszedlWyszedl = 0; // zmienna do wskazania czy drzwi już raz zostały otwarte

unsigned long staryCzas;
unsigned long nowyCzas;
long sekundy;

void setup()
{
  Serial.begin(9600);
  pinMode(wlamanie, INPUT);         // wejście włącznika antywłamaniowego czujki, nie potrzebne ale dałem w razie co
  pinMode(czujka, INPUT);           // wejscie czujki PIR
  pinMode(krancowkaDrzwi, INPUT);   // wejście krańcówki od drzwi
  pinMode(wlacznikGora, INPUT);     // wejscie włącznika światła głównego
  pinMode(wlacznikLustro, INPUT);   // wejscie włącznika światła nad lustrem
  pinMode(wlacznikPrysznic, INPUT); // wejscie włącznika światła nad prysznicem
  pinMode(Up, INPUT_PULLUP);        // wejście przycisku UP w menu
  pinMode(Down, INPUT_PULLUP);      // wejscie przycisku DOWN w menu
  pinMode(Ok, INPUT_PULLUP);        // wejscie przycisku Ok w menu
  pinMode(swiatloGora, OUTPUT);     // wyjście na przekaźnik światła 1
  pinMode(swiatloLustro, OUTPUT);   // wyjście na przekaźnik światła 2
  pinMode(swiatloPrysznic, OUTPUT); // wyjście na przekaźnik światła 3

  digitalWrite(swiatloGora, 0);     // na starcie wyłączam wszystkie światła
  digitalWrite(swiatloLustro, 0);   // na starcie wyłączam wszystkie światła
  digitalWrite(swiatloPrysznic, 0); // na starcie wyłączam wszystkie światła

  lcd.init(); // zainicjuj lcd
  lcd.clear();
  lcd.backlight();
}

void loop()
{

  stanWlacznikaGora = digitalRead(wlacznikGora);         // odczytuję czy któryś z włączników został naciśnięty
  stanWlacznikaLustro = digitalRead(wlacznikLustro);     // odczytuję czy któryś z włączników został naciśnięty
  stanWlacznikaPrysznic = digitalRead(wlacznikPrysznic); // odczytuję czy któryś z włączników został naciśnięty
  stanKrancowki = digitalRead(krancowkaDrzwi);           // odczytuję czy drzwi zostały drzwi
  stanCzujki = digitalRead(czujka);                      // odczytuję czy naruszono czujkę
  stanWlamania = digitalRead(wlamanie);
  if (uzytkownikWszedlWyszedl == 0)
  {
    digitalWrite(swiatloGora, stanWlacznikaGora);
  }                                                     // włączam lub wyłączam manualnie światła
  digitalWrite(swiatloLustro, stanWlacznikaLustro);     // w zależności od
  digitalWrite(swiatloPrysznic, stanWlacznikaPrysznic); // włączonego lub wyłączonego włącznika

  debugging();

  if (stanWlacznikaGora == 0 && stanWlacznikaLustro == 0 && stanWlacznikaPrysznic == 0)
  { // jeżeli nikt nie zapalił żadnego światła i
    if (stanKrancowki == 1 && uzytkownikWszedlWyszedl == 0)
    {                               // jeżlei otworzono drzwi i nikogo nie ma w łazience to:
      digitalWrite(swiatloGora, 1); // zaświecam światło główne i
      uzytkownikWszedlWyszedl = 1;  // zaznaczam że ktoś wszedł do łazienki
      do
      {                                              // wchodzę w pętlę po to by
        stanKrancowki = digitalRead(krancowkaDrzwi); // tylko sprawdzać czy nikt nie chce z łazienki wyjść
        debugging();
      } while (stanKrancowki == 1); // i siedzę w pętli póki nie zostaną otworzone drzwi
      delay(50);
    }
  }

  if (stanKrancowki == 1 && uzytkownikWszedlWyszedl == 1)
  { // jeżeli otworzono drzwi i ktoś w środku był to
    do
    {                                              // wchodzę w pętlę po to by
      stanKrancowki = digitalRead(krancowkaDrzwi); // tylko sprawdzać czy drzwi nie zostały zamknięte
      debugging();
    } while (stanKrancowki == 1); // i jeśli zostały zamknięte to opuszczam pętlę,
    uzytkownikWszedlWyszedl = 0;  // zaznaczam że ktoś wyszedł z łazienki
    digitalWrite(swiatloGora, 0); // i gaszę światło
    delay(50);
  }

  // tu wpisać odliczanie zadanego czasu do wyłączania światła

  //**********************************************************************************************************//
  //****************************** odliczanie 1 sekundy ******************************************************//
  nowyCzas = millis(); // wpisuję wartość aktualnego czasu systemu do zmiennej newTime
  if (stanCzujki == 0)
  {
    if (nowyCzas - staryCzas >= 1000)
    {                       // jeżeli różnica między aktualnym czasem a poprzednim jest większa lub równa 1sek to
      staryCzas = nowyCzas; // wpisuję nową wartość czasu do starego czasu
      sekundy++;            // zwiększam wartość zmiennej czasObrotu
    }
    if (sekundy == 300)
    {
      sekundy = 0;
      digitalWrite(swiatloGora, 0);
    }
  }
  //**********************************************************************************************************//
}

void debugging()
{
  Serial.print("sWG-");
  Serial.println(stanWlacznikaGora);
  Serial.print("sWL-");
  Serial.println(stanWlacznikaLustro);
  Serial.print("sWP-");
  Serial.println(stanWlacznikaPrysznic);
  Serial.print("sK-");
  Serial.println(stanKrancowki);
  Serial.print("uWW-");
  Serial.println(uzytkownikWszedlWyszedl);
  // stanCzujki = digitalRead(czujka);
  Serial.print("sCz-");
  Serial.println(stanCzujki);
  Serial.println("  ");
  // delay(500);
}
