
//------------ SmartRelaySwitch-----------------
//--------------@leoncioblues-------------------
//----------------30-10-2020--------------------
//---------------Version 1.0--------------------

#include <ezButton.h>
#include <EEPROM.h>

//pins config
ezButton button(0);    // PB0; PIN:5
const int MUTE = 3;    // PB3; PIN:2
const int LEDPIN = 4;  // PB4; PIN:3
const int RELAY1 = 1;  // PB1; PIN:6
const int RELAY10 = 2; // PB2; PIN:7

//button config
int button_press = 0;
bool isPressing = false;
bool isLongDetected = false;
unsigned long SHORT_PRESS_TIME = 30; // 10 milliseconds
unsigned long LONG_PRESS_TIME  = 1000; // 1seg hold the button to momentary on/off
unsigned long pressedTime  = 0;
unsigned long pressedTime2  = 0;
unsigned long pressedTime3  = 0;
unsigned long releasedTime = 0;
unsigned long pressDuration;


int addr = 47; // memory position

int pedal_mode = 0;   // pedal_mode =0 -> always ON   // pedal_mode =1 -> on/off - momentary


//flags
int pedal_state = LOW; // pedal state on or off
int a = 0; // turn to "1" when the fisrt loop is done or enter to change the mode.
int b = 0; // turn to "1" whe the mode is changed and is "0" when the pedal had no mode or use the last one mode.

//------------------------------------------------------------------------------

void setup() // initial setup
{
  button.setDebounceTime(25); // set debounce time to 15 milliseconds

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, pedal_state);

  pinMode(MUTE, OUTPUT);
  digitalWrite(MUTE, LOW);

  pinMode(RELAY1, OUTPUT);
  digitalWrite(RELAY1, LOW);
  pinMode(RELAY10, OUTPUT);
  digitalWrite(RELAY10, LOW);
}

//------------------------------------------------------------------------------

void loop()
{

  if (a == 0)
  {
    button.loop();
    button_press = !button.getState();

    pressedTime = millis();

    while ((pressedTime3 < 500) && button_press == 1 )
    {
      button.loop();
      pressedTime2 = millis();
      button_press = !button.getState();
      pressedTime3 = pressedTime2 - pressedTime;
    }

    if ( pressedTime3 >= 500 )
    {
      EEPROM.get(addr, pedal_mode);

      if (pedal_mode != 0 && pedal_mode != 1)
      {
        EEPROM.put(addr, 1);
        EEPROM.get(addr, pedal_mode);

        led_show();

        a = 1;
        b = 1;

      }

      pedal_mode = !pedal_mode;

      EEPROM.put(addr, pedal_mode);

      led_show();

      a = 1;
      b = 1;

    }
  }

  //------------------------------------------------------------------------------

  if (b == 0)
  {

    EEPROM.get(addr, pedal_mode);

    if (pedal_mode != 0 && pedal_mode != 1)
    {
      EEPROM.put(addr, 1);
      EEPROM.get(addr, pedal_mode);

      led_show();

      a = 1;
    }
    b = 1;
  }

  //------------------------------------------------------------------------------

  if (pedal_mode == 0) //always ON mode
  {
    digitalWrite(LEDPIN, HIGH);
    on_off_action(HIGH);
    while (1); // loop infinito
  }

  //------------------------------------------------------------------------------

  if (pedal_mode == 1) // on/off-momentary mode
  {
    a = 1;

    button.loop();

    if (button.isPressed())
    {
      pressedTime = millis();
      isPressing = true;
      isLongDetected = false;
      pressDuration = releasedTime - pressedTime;

      if ( pressDuration > SHORT_PRESS_TIME )
      {
        pedal_state = !pedal_state;

        on_off_action(pedal_state);

      }
    }

    if (isPressing == true && isLongDetected == false)
    {
      pressDuration = millis() - pressedTime;
    }

    if ( pressDuration >= LONG_PRESS_TIME )
    {
      isLongDetected = true;

      if (button.isReleased())
      {
        isPressing = true;
        releasedTime = millis();
        pedal_state = !pedal_state;

        on_off_action(pedal_state);

      }
    }
  }

}

//------------------------------------------------------------------------------


//----led blink when change mode--------
void led_show(void)
{
  int i, j = 1;
  for (i = 0; i < 8; i++)
  {
    digitalWrite(LEDPIN, j);
    delay (75);
    j = !j;
  }
}

//----on/off led and relay activation with mute signal----
void on_off_action(int pdl_st)
{
  digitalWrite(MUTE, HIGH );
  digitalWrite(LEDPIN, pdl_st);
  delay (5);
  digitalWrite(RELAY10, pdl_st );
  delay (20);
  digitalWrite(MUTE, LOW );
}
