/*
   Esplora Sends Gcode to uArm (Swift Pro via Serial Monitor)

   This sketch uses Esplora's keyboard emulation to control
   uArm Swift Pro (4 DOF robot arm) via serial terminal.

   When the Esplora joystick or switch buttons are pressed,
   Esplora sends keybaord message in G-Gode -
   joystick controls x, y axes;
   switch left/right controls 4th motor angle;
   switch up/down controls z-axis;
   joystick button down controls suction;
   slider controls movement speed;
   light sensor resets to a starting position.

   Step 1: Compile this sketch to Esplora.
   Keep it connected to the PC but don't touch buttons yet.
   If you have TFT LCD it displays which button is pressed.

   Step 2: Connect uArm Swift(Pro) to PC, power on.

   Step 3: From Arduino IDE, connect port to uArm (like /dev/cu.usbmodem!a13441)
   and select board as Mega 2560.

   Step 4: Open serial terminal from Arduino IDE.
   Check baud rate = 115200 and "Newline" is selected. uArm will
   echo text in terminal and is ready to receive commands

   Step 5: Put cursor on serial terminal input box and press a button on Esplora.
   A string like #25 G0 X180 Y-180 Z75 F10000

   Step 6: It moves! (Careful to move joystick only when cursor is in serial
   terminal input box, else you get gcode all over the place.

   Caution: closing serial terminal will make uArm go limp and lose all motor
   tension. Hold the uArm or put a cushion around to protect the effector.

   Created: 10 August 2017
   Author: Eric Ong (ebto on GitHub)

*/

//#include <SoftwareSerial.h>
#include <Esplora.h>
#include <Keyboard.h>
#include <TFT.h>
#include <SPI.h>

//boolean buttonStates[4];
//
//const byte buttons[] = {
//  JOYSTICK_DOWN,    //X-axis down
//  JOYSTICK_LEFT,    //Y-axis left
//  JOYSTICK_UP,      //X-axis up
//  JOYSTICK_RIGHT   //Y-axis right
//  //SWITCH_RIGHT,     // 4
//  //SWITCH_LEFT,      // 2
//  //SWITCH_UP,        // 3
//  //SWITCH_DOWN       // 1
//};

int angle = 90;
int toggle = PRESSED;

//String rx;
//int zVal = 0;
//int xyzVal[4] = {100, 6, 5, 10000};

void setup() {

  DisplaySplash(); // display sketch name

  // Open serial communications
  while (!Serial) {
    // wait for serial port to connect.
    Serial.begin(115200);
  }

  //  if (Serial.available() > 0) {   // is a character available?
  //    rx = Serial.readString();     // get the character "N0 V1"
  //  }

  Keyboard.begin();
  // Move to default resting position
  resetPosition();

}

void loop() {

  // reset uarm to predefined coordinate when light sensor is covered
  if (Esplora.readLightSensor() <= 250) {
    resetPosition();
  }

  // toggle end effector on/off
  if (Esplora.readJoystickButton() == PRESSED) {
    if (toggle == PRESSED) {
      readEffector(toggle);
      toggle = RELEASED;
    } else {
      readEffector(toggle);
      toggle = PRESSED;
    }
  }

  // control wrist with R/L switch button
  while (Esplora.readButton(SWITCH_LEFT) == PRESSED) {
    moveWrist(-5);
  }

  while (Esplora.readButton(SWITCH_RIGHT) == PRESSED) {
    moveWrist(5);
  }

  while (Esplora.readButton(SWITCH_UP) == PRESSED) {
    budgeBy(0, 0, 2);
  }

  while (Esplora.readButton(SWITCH_DOWN) == PRESSED) {
    budgeBy(0, 0, -2);
  }

  while (Esplora.readButton(JOYSTICK_LEFT) == PRESSED) {
    budgeBy(0, 2, 0);
  }

  while (Esplora.readButton(JOYSTICK_RIGHT) == PRESSED) {
    budgeBy(0, -2, 0);
  }

  while (Esplora.readButton(JOYSTICK_UP) == PRESSED) {
    budgeBy(2, 0, 0);
  }

  while (Esplora.readButton(JOYSTICK_DOWN) == PRESSED) {
    budgeBy(-2, 0, 0);
  }

  if (Esplora.readButton(JOYSTICK_DOWN || JOYSTICK_LEFT || JOYSTICK_RIGHT || JOYSTICK_UP  ||
                         SWITCH_LEFT || SWITCH_RIGHT || SWITCH_UP || SWITCH_DOWN) == RELEASED) {
    Keyboard.releaseAll();
  }

  //delay(100);
  //  Keyboard.releaseAll();

  //  // Iterate through all the buttons:
  //  for (byte thisButton = 0; thisButton < 4; thisButton++) {
  //
  //    boolean lastState = buttonStates[thisButton];
  //    boolean newState = Esplora.readButton(buttons[thisButton]);
  //
  //    if (lastState != newState) { // Something changed!
  //      /*
  //        The Keyboard library allows you to "press" and "release" the
  //        keys as two distinct actions. These actions can be
  //        linked to the buttons we're handling.
  //
  //      */
  //      if (newState == PRESSED) {
  //
  //        EsploraTFT.stroke(ST7735_GREEN);
  //
  //        //        if (buttons[thisButton] == JOYSTICK_DOWN || JOYSTICK_LEFT || JOYSTICK_RIGHT || JOYSTICK_UP) {
  //        //          int xyVal[3] = {Esplora.readJoystickX(), Esplora.readJoystickY(), Esplora.readSlider()};
  //        //
  //        //          //budgeBy(xyVal[1],xyVal[0],0,xyVal[2]);
  //        //
  //        //          moveToXY(
  //        //            map(xyVal[1], 511, -512, 5, 356),
  //                      // joystick y axis (left/right) mapped to uArm x axis (forward/backward)
  //        //            map(xyVal[0], -512, 511, -250, 250),
  //                      // joystick x axis (up/down) mapped to uArm y axis (left/right)
  //        //            map(xyVal[2], 1023, 0, 1000, 30000)
  //        //          );
  //        //        }else {
  //        //          // nothing here
  //        //        }
  //
  //      } else if (newState == RELEASED) {
  //        Keyboard.releaseAll();
  //      }
  //
  //      // Store the new button state, so you can sense a difference later:
  //      buttonStates[thisButton] = newState;
  //
  //    } // if laststate != newstate
  //  } // for..loop end

}  // void loop end

void DisplaySplash() {   // display first screen


  // initialize TFT LCD display
  EsploraTFT.begin();
  // clear the screen with a black background
  EsploraTFT.background(ST7735_BLACK);

  EsploraTFT.stroke(ST7735_WHITE);
  EsploraTFT.setTextSize(2);
  EsploraTFT.setTextWrap(true);
  EsploraTFT.text("ESPLORA", 0, 0);
  EsploraTFT.text("SENDS GCODE", 0, 20);
  EsploraTFT.text("TO UARM 7", 0, 40);
  EsploraTFT.text("x, y, z", 0, 60);
  EsploraTFT.text("wrist, effectr", 0, 80);
  EsploraTFT.text("slider", 0, 100);
  delay(3000);
  EsploraTFT.background(ST7735_BLACK);

  // set the font size for the loop
  EsploraTFT.setTextSize(2);
  // set the font color
  EsploraTFT.stroke(ST7735_GREEN);
  EsploraTFT.text("X-axis:", 0, 0);
  EsploraTFT.text("Y-axis:", 0, 20);
  EsploraTFT.text("Z-axis:", 0, 40);
  EsploraTFT.text("Wrist:", 0, 60);
  EsploraTFT.text("Speed:", 0, 80);
  EsploraTFT.text("Suction:", 0, 100);

}

void moveTo(int x, int y, int z, int f) {
  const String COMMAND = "#25 G0 ";

  Keyboard.print(COMMAND + "X" + x + " Y" + y + " Z" + z + " F" + f + "\n");

  displayVal(String(x), 100, 0);
  displayVal(String(y), 100, 20);
  displayVal(String(z), 100, 40);
  displayVal(String(f), 100, 80);

}

void budgeBy(int x, int y, int z) {
  const String COMMAND = "#25 G2204 ";

  int coord[4] = {x, y, z, Esplora.readSlider()};
  coord[0] = constrain(coord[0] + x, -350, 350);
  coord[1] = constrain(coord[1] + y, 50, 356);
  coord[2] = constrain(coord[2] + z, 0, 350);
  coord[3] = map(coord[3], 1023, 0, 1000, 30000);

  Keyboard.print(COMMAND + "X" + x + " Y" + y + " Z" + z + " F" + coord[3] + "\n");

  //  displayVal(String(x), 100, 0);
  //  displayVal(String(y), 100, 20);
  //  displayVal(String(z), 100, 40);
  //  displayVal(String(coord[3]), 100, 80);
}

void readEffector(boolean btn) {

  if (btn == LOW) {
    Keyboard.println("#25 M2231 V1\n");
    delay(100);
    displayVal("ON", 100, 100);
  } else if (btn == HIGH) {
    Keyboard.println("#25 M2231 V0\n");
    delay(100);
    displayVal("OFF", 100, 100);
  }
}

void moveWrist(int offset) {
  const String COMMAND = "#25 G2202 N3 V";
  angle = constrain(angle + offset, 0, 180);

  Keyboard.println(COMMAND + angle + "\n");
  displayVal(String(angle), 100, 60);
}

void displayVal(String txtval, int xPos, int yPos) {
  char Printout[6];
  String str[1];
  str[0] = String(txtval);
  str[0].toCharArray(Printout, 5);

  EsploraTFT.stroke(ST7735_WHITE);
  EsploraTFT.text(Printout, xPos,  yPos);
  delay(50);
  EsploraTFT.stroke(ST7735_BLACK);
  EsploraTFT.text(Printout, xPos, yPos);
}

void resetPosition() {
  moveTo(100, 5, 20, 10000);
  delay(1000);
  moveWrist(0);
  delay(1000);
  readEffector(RELEASED);
  //Play tune to notify reset command
  //PlayCoin();
}

void PlayCoin() {
  //play a tune on uArm
  Keyboard.println("M2210 F988 T100\n" );
  delay(100);
  Keyboard.println("M2210 F1319 T850\n" );
  delay(850);
}

