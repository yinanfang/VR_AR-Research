
void setup() {
  Serial.begin(115200);
  Serial.flush();
}

void loop() {
  if (Serial.available()) {
    byte c = Serial.read();
    if (c == 'x') {      // 'x' is end of input message
      Serial.write('0');
      Serial.write('1');
      Serial.write('2');
      Serial.write('x');
//      Serial.send_now();
    }
  }
}

