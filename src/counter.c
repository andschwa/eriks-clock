void interruptCall();

int i = 0;
int interruptPin = 2; // digi pin 0 -> pin 2;


unsigned long NewTime = 0;
unsigned long OldTime = 0;
unsigned long period = 0;

double bpm = 0.0;
double avg = 0.0;

volatile int PinState = LOW;
volatile int State = LOW;

void setup()
{
  attachInterrupt(interruptPin, interruptCall, FALLING);
  Serial.begin(9600);
}

void loop()
{
 
}

void interruptCall()
{
 
  NewTime = millis();
  period = (NewTime - OldTime)/2;
  OldTime = NewTime;
  
  bpm = 60000.0/period;

if( i > 3)
  {  
    avg += bpm;
  
    Serial.print(i-3);
    Serial.print(" ");
    Serial.print(avg / (i - 3) , 5);
    Serial.print(" ");
    Serial.println(bpm, 10);
  }


}
