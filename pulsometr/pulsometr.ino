#define heartratePin A1
#include "DFRobot_Heartrate.h"


DFRobot_Heartrate heartrate(DIGITAL_MODE); ///< ANALOG_MODE or DIGITAL_MODE


int MIN = 300;	// minimalny zarejestrowany puls
int MAX = 0;	// maksymalny zarejestrowany puls
int delta = 0;	// ró¿nica pomiêrdzy pobliskimi pomiarami

int tabPuls[300];			// tablica zawieraj¹ca 300 poprawnych pomiarów
int licznikPomiarow = NULL;	// licznik wykonywanych pomiarów
int sredniPuls = NULL;		// œrednia wartoœæ pulsu z 300 pomiarów

const int Tachycardia = 100;	// wartoœæ powy¿ej, której stwierdza siê Tachycardiê
const int Bradycardia = 60;		// wartoœæ poni¿ej, której stwierdza siê Bradycardiê
int sr = 0;						// œrednia wartoœæ pulsu z 300 pomiarów
int licznikSkokow = 0;			// liczba du¿ych skoków pomiêdzy s¹siednimi pomiarami 


/*-----------------s r e d n i a---------------------------------
*	Funkcja wyliczaj¹ca œredni¹ wartoœæ pulsu z 300 pomiarów.
*----------------------------------------------------------------*/
int srednia() {
	double suma = 0;
	for (int i = 0; i < 300; i++) {
		suma += tabPuls[i];
	}
	return suma / 300;
}

/*------------------- z e r u j ---------------------------------
*	Funkcja zeruj¹ca tablicê z pomiarami pulsu.
*----------------------------------------------------------------*/
void zeruj()
{
	for (int i = 0; i < 300; i++)
	{
		tabPuls[i] = 0;
	}
}

/*----------------- l i c z D e l t e -----------------------------
*	Funkcja wyliczaj¹ca ró¿nicê w wartoœci pomiêdzy s¹sienimi 
*	pomiarami pulsu, je¿eli ró¿nica jest zbyt du¿a wypisuje 
*	odpowiedni komunikat oraz zwiêksza licznik skoków.
*----------------------------------------------------------------*/
void liczDelte() 
{
	int DELTA = 0;
	for (int i = 0; i < 299; i++)
	{
		DELTA = tabPuls[i] - tabPuls[i + 1];
		if (DELTA > 10) {
			Serial.print("UWAGA!!!!    Zbyt duze skoki pomiedzy s¹siednimi wartosciami, delta: ");
			Serial.println(DELTA);
			Serial.print("Pomiar A: ");
			Serial.println(tabPuls[i]);
			Serial.print("Pomiar B: ");
			Serial.println(tabPuls[i + 1]);
			Serial.println("");
			licznikSkokow++;
		}
	}
}

void setup() {
	Serial.begin(115200);
	Serial.println("P U L S O M E T R");
	zeruj();
}

void loop() {

	uint8_t rateValue;
	heartrate.getValue(heartratePin); ///< A1 foot sampled values
	rateValue = heartrate.getRate(); ///< Get heart rate value 

	if (rateValue &&(licznikPomiarow<300)) 
	{
		Serial.print("Akutalny puls: ");
		Serial.println(rateValue);
		if (rateValue > MAX) { MAX = rateValue; }	// pozyskanie maksymalnego zarejestrowanego pulsu
		if (rateValue < MIN) { MIN = rateValue; }	// pozyskanie minimalnego zarejestrowanego pulsu

		tabPuls[licznikPomiarow] = rateValue;		// wpisanie poprawnego pomiaru do tablicy pomiarów 
		licznikPomiarow++;
	}
	else {
		/*-----------------------------------------------------------
		 *	Wypisanie raportu po zakoñczeniu badania pulsu.
		 *---------------------------------------------------------*/
		if (licznikPomiarow == 300)
		{
			Serial.println(" - - < < ZAKONCZONO POMIAR PULSU > > - - ");
			Serial.println("");
			delta = MAX - MIN;
			
			Serial.print("Ró¿nica pomiêdzy MIN i MAX pulsem = ");
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
				Serial.println("Tachykardia - czêstoskurcz - przyœpieszona kacja serca (>100).");
			}
			else if (sr < Bradycardia) {
				Serial.println("Bradykardia - œredni puls pomi¿ej normy (<60 uderzeñ na minutê).");
			}
			else {
				Serial.println("Sredni puls w normie.");
			}
			Serial.println("");
			Serial.print("Liczba du¿ych skoków pomiêdzy pomiarami pulsu: ");
			Serial.println(licznikSkokow);
			if (licznikSkokow > 10) {
				Serial.println("Zbyt czêste skoki pulsu.  Brak regularnoœci têtna.");
			}
			delay(5000000);

		}
	}

	

	
	delay(20);
}
