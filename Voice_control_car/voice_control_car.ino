#include <AFMotor.h>

String command;
char encode[100];
int k=0, i=0;

AF_DCMotor motor1(1, MOTOR12_1KHZ);
AF_DCMotor motor2(2, MOTOR12_1KHZ);
AF_DCMotor motor3(3, MOTOR34_1KHZ);
AF_DCMotor motor4(4, MOTOR34_1KHZ);

void setup() {

  Serial.begin(9600);

}

void loop() {

  delay(10);
  while (Serial.available()) {
    command = "";
    command = Serial.readString();
    /*
     Có thể đưa ra mã nhị phân để kiểm tra xem có size bit nào không.
     */
    while(command[i]!='\0'){
      encode[k] = command[i];
      k++;
      i++;
    }
    
    /*
     hàm in ra mã nhị phân
     */
     //output(encode);
  }
  if (command == "*Move forward#" || command == "*move forward#") {
    forward();
  } else if (command == "*Move backward#" || command == "*move backward#") {
    backward();
  } else if (command == "*Turn left#" || command == "*turn left#") {
    left();
  } else if (command == "*Turn right#" || command == "*turn right#") {
    right();
  } 
  command = "";
}
/*
 Hàm xuất ra mã nhị phân
 */
void output(char x[]){
  
  int i = 0;
  while(x[i]!='\0')
  {
    Serial.println(x[i],BIN);
    delay(1000);
    i++;
  }
  
}

void forward() {

  motor1.setSpeed(255);
  motor1.run(FORWARD);
  motor2.setSpeed(255);
  motor2.run(FORWARD);
  motor3.setSpeed(255);
  motor3.run(FORWARD);
  motor4.setSpeed(255);
  motor4.run(FORWARD);
  delay(1500);
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);

}

void backward() {

  motor1.setSpeed(255);
  motor1.run(BACKWARD);
  motor2.setSpeed(255);
  motor2.run(BACKWARD);
  motor3.setSpeed(255);
  motor3.run(BACKWARD);
  motor4.setSpeed(255);
  motor4.run(BACKWARD);
  delay(1500);
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}

void right() {

  motor1.setSpeed(255);
  motor1.run(FORWARD);
  motor2.setSpeed(255);
  motor2.run(FORWARD);
  motor3.setSpeed(255);
  motor3.run(BACKWARD);
  motor4.setSpeed(255);
  motor4.run(BACKWARD);
  delay(790);
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}

void left() {

  motor1.setSpeed(255);
  motor1.run(BACKWARD);
  motor2.setSpeed(255);
  motor2.run(BACKWARD);
  motor3.setSpeed(255);
  motor3.run(FORWARD);
  motor4.setSpeed(255);
  motor4.run(FORWARD);
  delay(650);
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}
