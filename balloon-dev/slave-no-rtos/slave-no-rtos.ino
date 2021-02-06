String msg1 = "";
String msg2 = "";
String msg3 = "";

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
}

void loop() {
  if (Serial1.available()){
    msg1 = Serial1.readStringUntil(',');
    msg2 = Serial1.readStringUntil(',');
    msg3 = Serial1.readStringUntil(',');
  }
  Serial.print(msg1);
  Serial.print("\t");
  Serial.print(msg2);
  Serial.print("\t");
  Serial.print(msg3);
  Serial.print("\n");
}
