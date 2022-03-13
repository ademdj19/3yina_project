#ifndef MyClass_h
#define MyClass_h
#include "Arduino.h"
class UI
{
  public:
    UI();
    void lcd_dht_master();
    void lcd_dht_slave();
    void lcd_period();
    void lcd_mass();
    void lcd_start_cycle();
    typedef void (UI::*GeneralFunction) ();
    static const GeneralFunction doActionsArray[5];

};
#endif
