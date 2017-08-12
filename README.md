# sendGcodetouarm7

Esplora Sends Gcode to uArm (Swift Pro via Serial Monitor)
   
This sketch uses Esplora's keyboard emulation to control uArm Swift Pro (4 DOF robot arm) via serial terminal. When the Esplora joystick or switch buttons are pressed, Esplora sends keybaord message in G-Gode -
   
   joystick controls x, y axes;
   switch left/right controls 4th motor angle;
   switch up/down controls z-axis;
   joystick button down controls suction;
   slider controls movement speed;
   light sensor resets to a starting position.
   
   
Step 1: Compile this sketch to Esplora. Keep it connected to the PC but don't touch buttons yet. If you have TFT LCD it displays which button is pressed.
   
Step 2: Connect uArm Swift(Pro) to PC, power on.
   
Step 3: From Arduino IDE, connect port to uArm (like /dev/cu.usbmodem!a13441) and select board as Mega 2560.
   
Step 4: Open serial terminal from Arduino IDE. Check baud rate = 115200 and "Newline" is selected. uArm will echo text in terminal and is ready to receive commands
   
Step 5: Put cursor on serial terminal input box and press a button on Esplora. A string like #25 G0 X180 Y-180 Z75 F10000

Step 6: It moves! (Careful to move joystick only when cursor is in serial terminal input box, else you get gcode all over the place.

Caution: closing serial terminal will make uArm go limp and lose all motor tension. Hold the uArm or put a cushion around to protect the effector.

Created: 10 August 2017
Author: Eric Ong (ebto on GitHub)
