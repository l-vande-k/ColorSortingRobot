# ColorSortingRobot
The following repo contains the C++ programming necessary to drive a Fischertechnik pneumatic color sorting robot.

YouTube video showing successful operation:
https://youtu.be/AQcxmfMBEFc

Components:
- Brushed 8.4V DC Electric Motor to drive arm rotation
- DC Compressor
- 2 Digitally Controlled Solenoid Valves
- Rotory Encoder
- Limit Switch
- Tandem Piston-cylinders to convert positive pressure to negative pressure
- Piston-cylinder to lower arm
- Analog color sensor
- Phototransistor and LED for part detection
- Arduino Uno
    - Datasheet for ATmega328P: https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
- H-Bridge Dip IC (L293D)
    - Datasheet: https://www.ti.com/lit/ds/symlink/l293d.pdf?ts=1698445738188&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FL293D
- Pull up resistors (1kOhm)
- Elegoo jumper wires and dupont wires

Purpose Statement:
The goals for this robotics lab task were to explore the capabilities of the Arduino Uno board, construct an external circuit that supports the peripheral devices, ad interpret analog & digital signals. 

Description of Program and Circuit:
This color sorting robot was controlled by an Arduino Uno Rev3. The Arduino Uno is a microcontroller board that utilizes the ATmega328P. For more information on the ATmega328P, please reference the datasheet linked in the components list. 

The arm rotation motor is controlled by a simple H-bridge circuit. Motor speed is controlled with an analog output fromt the Arduino to the enable pin on the H-bridge. The direction of the motor is controlled by digital outputs on the Arduino. 

The rotary encoder is comprised of a limit switch that is installed adjacent to a 4-tooth gear which rotates as the motor moves. As the robot arm rotates, the gear and switch create high and low signals that can be interpreted as degrees of rotation. The arm is seated atop a gear with 58 teeth which is driven by a 10-tooth gear driven by the motor assembly. The 4-tooth gear rotates 1:1 with the 10-tooth gear. When the 58-tooth gear rotates one time, the 10-tooth gear will necessarily rotate 5.8 times. Likewise, so will the 4-tooth gear. We can find the exact number of interpretable signals using this relationship. For each rotation of the 4-tooth gear, we will get four presses from the limit switch. With each rotation of the robotic arm, that creates 23.2 high signals from the rotatry encoder. The code uses both the rising and falling edges of this high signal to achieve 46.4 signals along the rotation of the robotic arm. That creates a resolution of 360°/46.4 = 7.76° per count in the program. This resolution works well for the purposes of this project.
