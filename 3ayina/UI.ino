#include "Arduino.h"
#include "UI.h"
/*--| UI class responsible for displaying scenes respectively |--*/
UI::UI() {
  return;
};
void UI::lcd_dht_master() {
  /*--| DHT master lcd scene |--*/
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("<DHT1");
  lcd.print(" T="+String(desired_temp,1));
  lcd.setCursor(15,0);
  lcd.print(">");
  lcd.setCursor(0, 1);
  lcd.print(temp_Master, 2);
  lcd.print("C|");
  lcd.print(hum_Master, 2);
  lcd.print("%");
};
void UI::lcd_dht_slave() {
  /*--| DHT slave lcd scene |--*/
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("<DHT2");
  lcd.print(" T="+String(desired_temp,1));
  lcd.setCursor(15,0);
  lcd.print(">");
  lcd.setCursor(0, 1);
  lcd.print(temp_Slave, 2);
  lcd.print("C|");
  lcd.print(hum_Slave, 2);
  lcd.print("%");
};
void UI::lcd_period() {
  /*--| Period setting scene |--*/
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("<PERIOD        >");
  lcd.setCursor(0, 1);
  lcd.print("T=");
  lcd.print(period);
  lcd.print(" s");
};
void UI::lcd_mass() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("<MASS ");
  lcd.print(String(mass_in_g, 2) + " g");
  lcd.setCursor(15, 0);
  lcd.print(">");
  lcd.setCursor(0,1);
  lcd.print(final_mass);
  lcd.print(" g");
}
void UI::lcd_start_cycle() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.println("<"+press_up+"      >");
  lcd.setCursor(0,1);
  lcd.println(to_start);

}

// typedef for class function
typedef void (UI::*GeneralFunction) ();

static const GeneralFunction doActionsArray [5];
