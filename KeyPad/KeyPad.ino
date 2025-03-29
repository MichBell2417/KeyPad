#include <Keypad.h>

#include <Keyboard.h>

const byte ROWS = 3; //four rows
const byte COLS = 3; //four columns
//define the cymbols on the buttons of the keypads
char scheme[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
};
byte rowPins[ROWS] = {5, 6, 7}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {2, 3, 4}; //connect to the column pinouts of the keypad

byte switchLedPin=9;
byte ledPin=10;
byte selfRstPin=8;
bool ledStatus=false;
bool switchStatus=false;
bool classicModality=true;

//initialize an instance of class NewKeypad
Keypad keypad = Keypad( makeKeymap(scheme), rowPins, colPins, ROWS, COLS); 

void setup() {
  Keyboard.begin();
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(selfRstPin, OUTPUT);
  pinMode(switchLedPin, INPUT);
  digitalWrite(selfRstPin, HIGH);
  digitalWrite(ledPin, LOW);
}

long eseguito=0;
long timePressed=0;
void loop() {
  //eseguiamo il comando solo quando il pulsante viene rilasciato e ripremuto
  if(!switchStatus && digitalRead(switchLedPin)==HIGH){
    switchLedStatus();
    switchStatus=true; //segnamo che il pulsante è stato pigiato
    timePressed=millis(); //segnamo quando è stato pigiato
  }else if(switchStatus && digitalRead(switchLedPin)==LOW){
    switchStatus=false; //segnamo che il pulsante è stato pigiato
  }
  //se passano 3Sec reset del disp.
  if(switchStatus && millis()-timePressed>3000){
    Serial.println("RESET");
    digitalWrite(selfRstPin, LOW);
    delay(25);
    digitalWrite(selfRstPin, HIGH);
  }
  if(classicModality){
    /*
    dovrebbe essere superflua perche il software scrive nella porta seriale e larduino è costantemente in ascolto
    if(millis()-eseguito>1000){
      Serial.print("ACKR"); //richiesta di acknowledgment al software
      eseguito=millis();
    }*/
    classicMod();
  }else{
    slaveMod();
  }
}

//Modalità con software
void slaveMod(){
  String command=leggiSeriale();
  
}

//Modalità classica
char key; //indica qual'è il carattere contenuto
bool pressioneContinuata; //indica se la key può esser considerata come premuta o no
void classicMod(){
  pressioneContinuata=true;
  String mes=leggiSeriale();
  //se viene attivato lo switch o ricevuto un messaggio contenente "l" si cambia lo stato del LED
  if(mes=="l"){
    switchLedStatus();
  }
  key = keypad.getKey();
  if (key){
    String keyName="KEY_F"+String((int(key)-48)+12);
    //Serial.println("rilevata pressione sul pulsante: "+keyName);
    switch (int(key-48)){
      case 1:
        Serial.println(KEY_F13);
        Keyboard.press(KEY_F13);
        break;
      case 2:
        Serial.println(KEY_F14);
        Keyboard.press(KEY_F14);
        break;
      case 3:
        Serial.println(KEY_F15);
        Keyboard.press(KEY_F15);
        break;
      case 4:
        Serial.println(KEY_F16);
        Keyboard.press(KEY_F16);
        break;
      case 5:
        Serial.println(KEY_F17);
        Keyboard.press(KEY_F17);
        break;
      case 6:
        Serial.println(KEY_F18);
        Keyboard.press(KEY_F18);
        break;
      case 7:
        Serial.println(KEY_F19);
        Keyboard.press(KEY_F19);
        pressioneContinuata=false;
        break;
      case 8:
        Serial.println(KEY_F20);
        Keyboard.press(KEY_F20);
        pressioneContinuata=false;
        break;
      case 9:
        Serial.println(KEY_F21);
        Keyboard.press(KEY_F21);
        pressioneContinuata=false;
        break;
    }
    delay(100);
  }
  KeyState stato=keypad.getState();
  while(stato!=IDLE && pressioneContinuata){
    keypad.getKey();
    stato=keypad.getState();
    if(digitalRead(switchLedPin)==HIGH){
      switchLedStatus();
    }
    delay(50);
  }
  Keyboard.releaseAll();
  delay(50);

}

bool checkForSoftwareStatus(String ACK){ //controlliamo se vi è una risposta
  char acknowledgment[ACK.length() + 1];
  ACK.toCharArray(acknowledgment, ACK.length() + 1);
  if(acknowledgment[0]=='A' && acknowledgment[1]=='C' && acknowledgment[2]=='K'){
    if(classicModality){
      Serial.println("software detected");
      classicModality=false; //passiamo nella modalità slave
    }
    //se il messaggio contiene anche delle istruzioni
    if(ACK.length()>3){
      Serial.println("trovate istruzioni");
      char instruction[ACK.length()-2];
      //iteriamo il vettore per tutta la lunghezza del messaggio
      for(int i=3;i<ACK.length();i++){
        instruction[i-3]=acknowledgment[i];
      }
      //salviamo le eventuali istruzioni
      saveInstructions(instruction);
    }else{
      Serial.println("istruzioni mancanti");
    }
      
    return true;
  }else if(ACK.equals("QUIT")){
    Serial.println("Quitting software");
    classicModality=true;
    return true;
  }
  return false;
}

void saveInstructions(char instruction[]){
  //TODO: interpretare i comandi e salvare la configurazione per ogni pulsante
}

String leggiSeriale(){
  if(Serial.available()){
    String parola="";
    while(Serial.available()){
      char lettera=Serial.read();
      parola+=lettera;
    }
    //controlliamo se il messagio contiene lo stato del software
    if(checkForSoftwareStatus(parola)){
      //se è un messaggio del software ignoriamo eventuali comandi manuali
      return "";
    }
    return parola;
  }
  return "";
}

long cambioLed;
void switchLedStatus(){
  if(millis()-cambioLed>200){
    ledStatus=!ledStatus;
    Serial.print("cambio LED a: ");
    Serial.println(ledStatus);
    digitalWrite(ledPin, ledStatus);
    cambioLed=millis();
  }
}
