String data;
int led1 = 13;
int led2 = 12;
int led3 = 11;
int led4 = 10;
int button = 2;

int button_priv_state = LOW;

void setup() {
  // put your setup code here, to run once:
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(button, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  int button_state = digitalRead(button);

  if (button_state == HIGH && button_priv_state == LOW) {
    Serial.println("read");
    Serial.flush();
  }

  button_priv_state = button_state;
  
  if (Serial.available() > 0) {
    data = Serial.readStringUntil('\n');
  }

  if (data[0] == '4') {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, HIGH);
  } else if (data[0] == '3') {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, LOW);
  } else if (data[0] == '2') {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
  } else if (data[0] == '1') {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
  } else if (data[0] == '0') {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
  }
}

