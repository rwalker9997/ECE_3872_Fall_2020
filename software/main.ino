// There-Beats, Junior Design Project (ECE 3872)
// Based on code auto-generated by Proteus Visual Designer for Arduino

// Peripheral Configuration Code (Do Not Edit)
#pragma GCC push_options
#pragma GCC optimize ("Os")

#include <core.h> // Required by cpu
#include <cpu.h>
#include <Generic.h>
#include <motor_shield_v1.h>
#include <Ultrasonic.h> // Required by UR1
#include <Grove_Ultrasonic.h>

#pragma GCC pop_options

// Peripheral Constructors
CPU &cpu = Cpu;
GenericButton resetButton = GenericButton (4);
DCMotorV1 motor1 = DCMotorV1 (7, 5, 8);
DCMotorV1 motor2 = DCMotorV1 (9, 6, 10);
GroveUltrasonic distSensor = GroveUltrasonic ();
GenericRgbLedCc rgbLed = GenericRgbLedCc (11, 12, 13);
GenericRotaryAngleSensor modeRotary = GenericRotaryAngleSensor (A2, 300);
#define buzzerPin 2
GenericBuzzer buzzer = GenericBuzzer (buzzerPin);

void peripheral_setup () {
   motor1.begin ();
   motor2.begin ();
   distSensor.begin (0);
}

// End of generated code

// Global variables
enum State {
   IDLE,
   RECORD,
   PLAY_LIVE,
   PLAY_RECORD
} state = IDLE;

// Helper function declarations
void reset_recording();
void loop_idle();
void loop_record();
void loop_play_live();
void loop_play_record();

// Entry Points and Interrupt Handlers
void setup () {
   peripheral_setup();
}

// Main loop method manages state changes and calls current state handlers
void loop () {
   // Use mode selection dial to set current state
   int pot = modeRotary.readAngle(); // potentiometer value ranging 0 to 300
   if (pot < 75)
      state = State::IDLE;
   else if (pot < 150)
      state = State::RECORD;
   else if (pot < 225)
      state = State::PLAY_LIVE;
   else
      state = State::PLAY_RECORD;
      
   // Reset button takes precedence over mode selection dial. Stay idle while pressed
   if (resetButton()) {
      reset_recording();
      state = State::IDLE;
   }
   
   // Run state loop handler for current state
   switch (state) {
      case State::IDLE:
	 loop_idle();
	 break;
      case State::RECORD:
	 loop_record();
	 break;
      case State::PLAY_LIVE:
	 loop_play_live();
	 break;
      case State::PLAY_RECORD:
	 loop_play_record();
	 break;
   }
}


void reset_recording() {
   // Hard-coded led flashing sequence for now
   rgbLed.set(255, 255, 255);
   delay(100);
   rgbLed.set(0, 0, 0);
   delay(100);
   rgbLed.set(255, 255, 255);
   delay(100);
   rgbLed.set(0, 0, 0);
   delay(100);
}

void loop_idle() {
   // Blue LED when idling
   rgbLed.set(0, 0, 255);
   // Turn off motor
   motor1.stop();
   // Turn off speaker
   buzzer.off();
   cpu.noTone(buzzerPin);
}

void loop_record() {
   // Red LED when recording (blinking?)
   rgbLed.set(255, 0, 0);
   // Turn off motor
   motor1.stop();
   // Generate sound
   buzzer.on();
   int cm=analogRead(A3); // TODO: reading from distance sensor not working yet
   cpu.tone(buzzerPin,cm / 2);
}

void loop_play_live() {
   // Run motor when playing live
   motor1.run(1,255);
   // Turn off speaker
   buzzer.off();
   cpu.noTone(buzzerPin);
   
   // Green LED blinking when playing live (synchronous right now, maybe use timer interrupts?)
   rgbLed.set(0, 255, 0);
   delay(50);
   rgbLed.set(0, 0, 0);
   delay(50);
}

void loop_play_record() {
   // Green LED when playing recording
   rgbLed.set(0, 255, 0);
   // Run motor
   motor1.run(0, 255);
   // Generate sound
   buzzer.on();
   cpu.tone(buzzerPin,400);
}