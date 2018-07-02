#define heartratePin A1
#include "DFRobot_Heartrate.h"


DFRobot_Heartrate heartrate(DIGITAL_MODE); ///< ANALOG_MODE or DIGITAL_MODE


int MIN = 300;	// minimalny zarejestrowany puls
int MAX = 0;	// maksymalny zarejestrowany puls
int delta = 0;	// r�nica pomi�rdzy pobliskimi pomiarami

int tabPuls[300];			// tablica zawieraj�ca 300 poprawnych pomiar�w
int licznikPomiarow = NULL;	// licznik wykonywanych pomiar�w
int sredniPuls = NULL;		// �rednia warto�� pulsu z 300 pomiar�w

const int Tachycardia = 100;	// warto�� powy�ej, kt�rej stwierdza si� Tachycardi�
const int Bradycardia = 60;		// warto�� poni�ej, kt�rej stwierdza si� Bradycardi�
int sr = 0;						// �rednia warto�� pulsu z 300 pomiar�w
int licznikSkokow = 0;			// liczba du�ych skok�w pomi�dzy s�siednimi pomiarami 
int sredniaSkokow = 0;

/*-----------------s r e d n i a---------------------------------
*	Funkcja wyliczaj�ca �redni� warto�� pulsu z 300 pomiar�w.
*----------------------------------------------------------------*/
int srednia() {
	double suma = 0;
	for (int i = 0; i < 300; i++) {
		suma += tabPuls[i];
	}
	return suma / 300;
}

/*------------------- z e r u j ---------------------------------
*	Funkcja zeruj�ca tablic� z pomiarami pulsu.
*----------------------------------------------------------------*/
void zeruj()
{
	for (int i = 0; i < 300; i++)
	{
		tabPuls[i] = 0;
	}
}

/*----------------- l i c z D e l t e -----------------------------
*	Funkcja wyliczaj�ca r�nic� w warto�ci pomi�dzy s�sienimi 
*	pomiarami pulsu, je�eli r�nica jest zbyt du�a wypisuje 
*	odpowiedni komunikat oraz zwi�ksza licznik skok�w.
*----------------------------------------------------------------*/
void liczDelte() 
{
	int suma = 0;
	int DELTA = 0;
	for (int i = 0; i < 299; i++)
	{
		DELTA = tabPuls[i] - tabPuls[i + 1];
		if (DELTA > 10) {
			Serial.print("UWAGA!!!!    Zbyt duze skoki pomiedzy s�siednimi wartosciami, delta: ");
			Serial.println(DELTA);
			Serial.print("Pomiar A: ");
			Serial.println(tabPuls[i]);
			Serial.print("Pomiar B: ");
			Serial.println(tabPuls[i + 1]);
			Serial.println("");
			licznikSkokow++;
			suma += DELTA;
		}
	}
	sredniaSkokow = suma / licznikSkokow;
}

void setup() {
	Serial.begin(115200);
	Serial.println("P U L S O M E T R");
	zeruj();
}

void loop() {

	uint8_t rateValue;
	heartrate.getValue(heartratePin); 
	rateValue = heartrate.getRate(); 

	if (rateValue && (licznikPomiarow < 300)) 
	{
		Serial.print("Akutalny puls: ");
		Serial.println(rateValue);
		if (rateValue > MAX) { MAX = rateValue; }	// pozyskanie maksymalnego zarejestrowanego pulsu
		if (rateValue < MIN) { MIN = rateValue; }	// pozyskanie minimalnego zarejestrowanego pulsu

		tabPuls[licznikPomiarow] = rateValue;		// wpisanie poprawnego pomiaru do tablicy pomiar�w 
		licznikPomiarow++;
	}
	else {
		/*-----------------------------------------------------------
		 *	Wypisanie raportu po zako�czeniu badania pulsu.
		 *---------------------------------------------------------*/
		if (licznikPomiarow == 300)
		{
			Serial.println(" - - < < ZAKONCZONO POMIAR PULSU > > - - ");
			Serial.println("");
			delta = MAX - MIN;
			
			Serial.print("Roznica pomiedzy MIN i MAX pulsem = ");
			Serial.println(delta);
			Serial.print("Minimalny zarejestrowany puls =");
			Serial.println(MIN);
			Serial.print("Maksymalny zarejestrowany puls =");
			Serial.println(MAX);
			liczDelte();
			sr = srednia();
			Serial.print("Sredni puls to: ");
			Serial.println(sr);
			if (sr >= Tachycardia) {
				Serial.println("Tachykardia - czestoskurcz - przyspieszona akcja serca (>100).");
			}
			else if (sr < Bradycardia) {
				Serial.println("Bradykardia - sredni puls ponizej normy (<60 uderzen na minute).");
			}
			else {
				Serial.println("Sredni puls w normie.");
			}
			Serial.println("");
			Serial.print("Liczba duzych skokow pomiedzy pomiarami pulsu: ");
			Serial.println(licznikSkokow);
			if (sredniaSkokow)
			{
				Serial.print("Srednia wartosc duzych skokow tetna: ");
				Serial.println(sredniaSkokow);
			}
			if (licznikSkokow > 10) {
				Serial.println("Zbyt czeste skoki pulsu.  Brak miarowosci tetna.");
			}
			else {
				Serial.println("Miarowosc t�tna zachowana.");
			}
			delay(5000000);

		}
	}

	

	
	delay(20);
}
