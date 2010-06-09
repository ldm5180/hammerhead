// proxrcmds.h
#include <stdbool.h>

void enter_cmd_mode(void);
void send_command(int cmd);
int get_data(void);
void set_reporting_mode(bool on);
void set_auto_relay_refresh(bool on);
void store_auto_relay_refresh();
void turn_bank_relays_on(int bank);
void turn_bank_relays_off(int bank);
void set_potentiometer(int pot, int value);
void set_all_potentiometers(int value);

