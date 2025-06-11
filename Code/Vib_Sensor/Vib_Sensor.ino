void setup() {
  Serial.begin(9600);
  pinMode(3, INPUT); // Vibration sensor pin
}

void loop() {
  int vibrationValue = digitalRead(3);
  Serial.println(vibrationValue);
  delay(500);
}