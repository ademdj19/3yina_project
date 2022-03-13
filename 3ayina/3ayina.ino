//// including needed libraries
#include <Q2HX711.h>
#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "UI.h"


//// defining needed constants
/*| DHT constants  |----------------------------*/
#define DHTPIN1 6        // Master DHT pin 
#define DHTTYPE1 DHT11   // DHT22 if using dht 22
#define DHTPIN2 7        // Slave DHT pin
#define DHTTYPE2 DHT11   // DHT22 if using dht 22 
/*----------------------------------------------*/

/*| LCD constants |---------------------------------------*/
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int relay_motor_pin = 13;
const int relay_heater_pin = A1;
// array of function pointers for UI elements
const UI::GeneralFunction UI::doActionsArray [5] =
{
  &UI::lcd_dht_master,
  &UI::lcd_dht_slave,
  &UI::lcd_period,
  &UI::lcd_mass,
  &UI::lcd_start_cycle
};
const int scene_count = 5;
/*--------------------------------------------------------*/
/*-------| visual feedback |--------*/
String press_up = "Press UP";
String to_start = "to start" ;
/*----------------------------------*/
/*| LOADCELL constants |--------*/
const byte hx711_data_pin = 10;
const byte hx711_clock_pin = 9;
#define RAZ 8
const float weight_cof = 31456;
/*------------------------------*/

/*| buttons input pin A0 default |------*/
const int buttons_pin = A0;
/*--------------------------------------*/
//// creating instances of classes
// creating instanceس of class DHT
DHT dht1 = DHT(DHTPIN1, DHTTYPE1);
DHT dht2 = DHT(DHTPIN2, DHTTYPE2);

// creating instance of UI Static class
UI ui;
// creating instance of class hx711
Q2HX711 hx711(hx711_data_pin, hx711_clock_pin);

// creating instance of class LiquidCystal
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//// global variables
float hum_Master, hum_Slave, temp_Master, temp_Slave = 0;
float pre_hum_Master, pre_hum_Slave, pre_temp_Master, pre_temp_Slave = 0;
/*--| USER INPUTS |--*/
float desired_temp = 0;
float pre_desired_temp = 0;
float final_mass = 0;
float pre_final_mass = 0;
long millis_passed = 0;
long millis_checkpoint = 0;
/*-------------------*/
/* remise a zero */
float raz_mass = 0;
/* weighting period */
int period = 10;
int pre_period = 0;
long start_time = 0;
/* scene index */
unsigned int scene_index = 0;
/* start cycle true when the program cycle starts */
boolean start_cycle = false;
/* weight and previous weight */
float mass_in_g;
float pre_mass_in_g;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  dht1.begin();
  dht2.begin();
  pinMode(RAZ, INPUT);
  pinMode(relay_motor_pin, OUTPUT);
}

void loop() {
  /*---|reading humidity and temperature from Master and Slave|---*/
  hum_Master = dht1.readHumidity();
  temp_Master = dht1.readTemperature();

  hum_Slave = dht2.readHumidity();
  temp_Slave = dht2.readTemperature();
  /*---| getting pressed button |---*/
  int button_pressed = get_button();
  if (button_pressed > -1) {
    handle(button_pressed);
    delay(150);
  }
  // mesuring weight
  mass_in_g = (hx711.read() - raz_mass) / weight_cof;
  if (digitalRead(RAZ) == LOW) Raz();

  // updating lcd if an action happened or a one of the environment variables changed value
  if (button_pressed != -1 || digitalRead(RAZ) == LOW || a_value_changed()) {
    UI::GeneralFunction f = UI::doActionsArray [scene_index];
    // call the function
    (ui.*f) ();
  }
  /* cycle --------------------------------------------------------------------------------------*/
  if (start_cycle) {
    float seconds_passed = (millis() - start_time) / 1000;
    // phase variable holds 1 or o representing relay controlling motor ON or OFF respectively
    int phase = ((int)(seconds_passed) / period) % 2;
    digitalWrite(relay_motor_pin, phase);
    if (temp_Master >= desired_temp) digitalWrite(relay_heater_pin, LOW);
    if (temp_Master < desired_temp - 2) digitalWrite(relay_heater_pin, HIGH);
    if (mass_in_g <= final_mass) {
      start_cycle = false;
      press_up = "Press UP";
      to_start = "to start" ;
    }
  }
  /*---------------------------------------------------------------------------------------------*/

}
void Raz() {
  raz_mass = hx711.read();
}
int get_button() {
  /*
     this fnction returns integers from 0 to 4 [0, 1, 2, 3] representing up,down,left,right buttons respectively
     analog values should be changed to match resistors used
     example :
     resistors to button bind :
      +--------+------------+--------------+--------------+
      | button |  resistor  |  analog      | button value |
      +--------+------------+--------------+--------------+
      | UP     : 300k ohms -+> 10 to 250   |       0      |
      +--------+------------+--------------+--------------+
      | DOWN   : 100k ohms -+> 250 to 500  |       1      |
      +--------+------------+--------------+--------------+
      | LEFT   : 30k  ohms -+> 500 to 750  |       2      |
      +--------+------------+--------------+--------------+
      | RIGHT  : 220  ohms -+> 750 to 1024 |       3      |
      +--------+------------+--------------+--------------+
  */
  int values[] = {10, 250, 500, 750, 900};
  int analog_value = analogRead(buttons_pin);
  // there is never a pretty way to make this
  // if more buttons needed add an if statment
  // greetings from adem
  if (analog_value <= values[0])return -1;
  if (analog_value > values[1] && analog_value <= values[2]) return 0;
  if (analog_value > values[2] && analog_value <= values[3]) return 1;
  if (analog_value > values[3] && analog_value <= values[4]) return 2;
  if (analog_value > values[4]) return 3;
}


void handle(int button) {
  if (button == 2) {
    scene_index -= 1;
    scene_index = (scene_index) % scene_count;

  } else if (button == 3) {
    scene_index ++;
    scene_index = (scene_index) % scene_count;
  } else if (button == 1) {
    // if in temperature and humidity scene is active
    if (scene_index == 0 || scene_index == 1) {
      desired_temp -= 0.5;
    }
    // if mass scene is active
    else if (scene_index == 3) {
      final_mass -= 1;
    }
    // if period scene is active
    else if (scene_index == 2) {
      if (period > 10)period -= 1;
    }
    if (start_cycle) {
      start_cycle = false;
      press_up = "Press UP";
      to_start = "to start" ;
    }
  } else if (button == 0) {
    if (scene_index == 0 || scene_index == 1) {
      desired_temp += 0.5;
    } else if (scene_index == 3) {
      final_mass += 1;
    } else if (scene_index == 2) {
      period += 1;
    }
    // if in start cycle
    else if (scene_index == 4) {
      start_cycle = true;
      start_time = millis();
      press_up = "Down to ";
      to_start = "STOP... ";
    }
  }
}

boolean a_value_changed() {
  boolean changed = (hum_Master != pre_hum_Master)
                    || (hum_Slave != pre_hum_Slave)
                    || (temp_Master != pre_temp_Master)
                    || (temp_Slave != pre_temp_Slave)
                    || (desired_temp != pre_desired_temp)
                    || (final_mass != pre_final_mass)
                    || (mass_in_g != pre_mass_in_g);
  pre_hum_Master = hum_Master;
  pre_hum_Slave = hum_Slave;
  pre_temp_Master = temp_Master;
  pre_temp_Slave = temp_Slave;
  pre_desired_temp = desired_temp;
  pre_final_mass = final_mass;
  pre_mass_in_g = mass_in_g;
  return changed;
}
