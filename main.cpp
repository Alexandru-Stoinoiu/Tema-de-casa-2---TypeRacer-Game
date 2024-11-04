#include <Arduino.h>
const int pinLedRosu = 11;
const int pinLedVerde = 10;
const int pinLedAlbastru = 9;
const int pinButonStartStop = 3;
const int pinButonDificultate = 2;

enum StareJoc { IDLE, COUNTDOWN, RUNNING, STOP };
enum Dificultate { EASY, MEDIUM, HARD };

StareJoc stareJoc = IDLE;
Dificultate dificultate = EASY;

const char* dictionarCuvinte[] = {"robot", "arduino", "led", "buton", "cod", "proiect"};
const int numarCuvinte = sizeof(dictionarCuvinte) / sizeof(dictionarCuvinte[0]);

char cuvantCurent[20];
unsigned long ultimaApasareStart = 0;
unsigned long ultimaApasareDificultate = 0;
const unsigned long debounceDelay = 50;
unsigned long timpulAparitieiCuvantului = 0;
unsigned long inceputRunda = 0;
unsigned long delayDificultate = 3000;

int cuvinteCorecte = 0; 
const unsigned long durataRunda = 30000; 
bool cuvantGresit = false;

void setLedAlb() {
    analogWrite(pinLedRosu, 255);
    analogWrite(pinLedVerde, 255);
    analogWrite(pinLedAlbastru, 255);
}

void setLedVerde() {
    analogWrite(pinLedRosu, 0);
    analogWrite(pinLedVerde, 255);
    analogWrite(pinLedAlbastru, 0);
}

void setLedRosu() {
    analogWrite(pinLedRosu, 255);
    analogWrite(pinLedVerde, 0);
    analogWrite(pinLedAlbastru, 0);
}

void setup() {
    pinMode(pinLedRosu, OUTPUT);
    pinMode(pinLedVerde, OUTPUT);
    pinMode(pinLedAlbastru, OUTPUT);
    pinMode(pinButonStartStop, INPUT_PULLUP);
    pinMode(pinButonDificultate, INPUT_PULLUP);

    Serial.begin(9600);
    setLedAlb();
    randomSeed(analogRead(0)); 
}
void numaratoareInversa() {
    for (int i = 3; i > 0; i--) {
        Serial.print(i);
        Serial.print("\n");
        setLedAlb();
        delay(500);
        setLedRosu();
        delay(500);
    }
    stareJoc = RUNNING;
    setLedVerde();
}

void alegeCuvantNou() {
    int index = random(numarCuvinte);
    strcpy(cuvantCurent, dictionarCuvinte[index]);
    Serial.print("Word: ");
    Serial.println(cuvantCurent);
    delay(delayDificultate);
    cuvantGresit = false;                  
}

void handleStartButton() {
    int stareButon = digitalRead(pinButonStartStop);

    if (stareButon == LOW && (millis() - ultimaApasareStart > debounceDelay)) {
        ultimaApasareStart = millis();

        if (stareJoc == IDLE) {
            stareJoc = COUNTDOWN;
            numaratoareInversa();
            Serial.println("Start!");
            setLedVerde();
            inceputRunda = millis();  
            cuvinteCorecte = 0;       
            alegeCuvantNou();         
        } else if (stareJoc == RUNNING) {
            stareJoc = STOP;
            Serial.println("Stop!");
            setLedAlb();
        } else if (stareJoc == STOP) {
            delay(2000);
            stareJoc = IDLE;
            setLedAlb();
        }
    }
}

void handleDifficultyButton() {
    int stareButonDificultate = digitalRead(pinButonDificultate);

    if (stareJoc == IDLE && stareButonDificultate == LOW && (millis() - ultimaApasareDificultate > debounceDelay)) {
        ultimaApasareDificultate = millis();
        switch (dificultate) {
            case EASY:
                dificultate = MEDIUM;
                delayDificultate = 4000;
                Serial.println("Medium mode on!");
                break;
            case MEDIUM:
                dificultate = HARD;
                delayDificultate = 3000;
                Serial.println("Hard mode on!");
                break;
            case HARD:
                dificultate = EASY;
                delayDificultate = 5000;
                Serial.println("Easy mode on!");
                break;
        }
        delay(500);
    }
}

void gestioneazaCuvinte() {
    if (millis() - timpulAparitieiCuvantului >= delayDificultate) {
        Serial.println("Time expired!");
        setLedRosu(); 
        alegeCuvantNou();
    }
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();

        if (input.equals(cuvantCurent)) {
            Serial.println("Corect!");
            setLedVerde();
            cuvinteCorecte++; 
            alegeCuvantNou();
        } else {
            Serial.println("Wrong. Try again!");
            setLedRosu();
            cuvantGresit = true;
        }
    }
}
void verificaSfarsitRunda() {
    if (millis() - inceputRunda >= durataRunda) {
        Serial.print("Round is over! Corect words: ");
        Serial.println(cuvinteCorecte);
        stareJoc = STOP;
        setLedAlb(); 
    }
}
void loop() {
    handleStartButton();
    handleDifficultyButton();

    if (stareJoc == RUNNING) {
        gestioneazaCuvinte();
        verificaSfarsitRunda();
    }
}



