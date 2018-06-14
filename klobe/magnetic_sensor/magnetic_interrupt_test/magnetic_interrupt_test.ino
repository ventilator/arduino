const byte ledPin = 24;
const byte firePin = 7;
const byte interruptPin = 2;
volatile byte state = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(firePin, OUTPUT);
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE );
}

void loop() {
  digitalWrite(ledPin, state);
  digitalWrite(firePin, state);
}

void blink() {
  state = !state;
}
