#define BUFFER_SIZE	10
char inputBuff[BUFFER_SIZE];

// Vitesse: 100/255
unsigned char right_speed = 64;
unsigned char left_speed = 64;
// Sens
bool right_to_front = 1;
bool left_to_front = 1;

void setup() {
	// Turn the Serial Protocol ON
	Serial.begin(9600);

	// Vitesse moteur gauche
	pinMode(9, OUTPUT);
	// Direction moteur gauche
	pinMode(7, OUTPUT);

	// Vitesse moteur droit
	pinMode(10, OUTPUT);
	// Direction moteur droit
	pinMode(8, OUTPUT);
}


void loop() {
	// Sens
	digitalWrite(7, (right_to_front?HIGH:LOW)); // left
	digitalWrite(8, (left_to_front?HIGH:LOW)); // right

	// Vitesse
	analogWrite(9, left_speed);
	analogWrite(10, right_speed);

	char readedByte;
	unsigned short int i = 0;
	do {
		while (!Serial.available())
			delay(2);
		readedByte = Serial.read();
		inputBuff[i++] = readedByte;
	} while ((readedByte != -1) && (readedByte != '\0') && (readedByte != '\n'));
	inputBuff[i] = '\0';

	char whichMotor = inputBuff[0];
	int motorSpeed = atoi(inputBuff + 1);
	bool to_front = motorSpeed >= 0;
	if (!to_front)
		motorSpeed = -motorSpeed;

	switch (whichMotor) {
		case 'L':
			/*Serial.print("Left speed: ");
			Serial.print(motorSpeed);
			Serial.println(to_front ? "To Front" : "To Forward");*/
			left_speed = motorSpeed;
			left_to_front = to_front;
			break;
		case 'R':
			/*Serial.print("Right speed: ");
			Serial.print(motorSpeed);
			Serial.println(to_front ? "To Front" : "To Forward");*/
			right_speed = motorSpeed;
			right_to_front = to_front;
			break;
	}

	//sensorVal = map(analogRead(A0),0,1023,0,100); // pour plus tard

}
