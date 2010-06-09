// proxrcmds.c

#include <stdio.h>
#include "proxrport.h"
#include "proxrcmds.h"

/** @brief sends command to proxr controller
 *  @param command to be sent
 *  @return void
 */
void send_command(int cmd)
{
    write_port((char)cmd);
}

/** @brief readies the controller for a command
 *  @param void
 *  @return void
 */
void enter_cmd_mode(void)
{
    send_command(254);
}

/** @brief gets data from the proxr controller
 *  @param void
 *  @return returns the data ready to be read from the controller
 */
int get_data(void)
{
    char data[1];
    int idata;
 
    read_port(data);
    idata = (int)data[0];

    return idata;
}

/** @brief sets the reporting mode of the proxr controller
 *  @param takes a boolean argument. True corresponds to reporting mode on and false
 *   corresponds to off.
 *  @return void
 */
void set_reporting_mode(bool on)
{
    if(on)
    {
        enter_cmd_mode();
        send_command(27);
    }
    else
    {
        enter_cmd_mode();
        send_command(28);
    }
}

/** @brief sets the automatic relay refresh
 *  @param takes a boolean argument. True corresponds to automatic relay refresh on and
 *   false corresponds to off.
 *  @return void
 */
void set_auto_relay_refresh(bool on)
{
    if(on)
    {
        enter_cmd_mode();
        send_command(25);
    }
    else
    {
        enter_cmd_mode();
        send_command(26);
    }
}

/** @brief stores the current automatic relay refresh state
 *  @param void
 *  @return void
 */
void store_auto_relay_refresh()
{
    enter_cmd_mode();
    send_command(35);
}

/** @brief turns on all the relays in a bank
 *  @param takes an integer argument. The argument determines which bank to turn the
 *   relays on in.
 *  @return void
 */
void turn_bank_relays_on(int bank)
{
    enter_cmd_mode();
    send_command(130);
    send_command(bank);
}

/** @brief turns off all the relays in a bank
 *  @param takes an integer argument. The argument determines which bank to turn the
 *   relays off in.
 *  @return void
 */
void turn_bank_relays_off(int bank)
{
    enter_cmd_mode();
    send_command(129);
    send_command(bank);
}

/** @brief sets a selected potentiometer to a value
 *  @param takes two integer arguments. the first parameter determines which 
 *   potentiometer to set with a valid range of 0-15. the second determines the value
 *   for the potentiometer to be set at with a valid range of 0-255.
 *  @return void
 */
void set_potentiometer(int bank, int value)
{
    if(bank < 0 || bank > 15)
    {
        printf("set_potentiometer: invalid bank. valid range 0-15.\n"); 
    }
    else if(value < 0 || value > 255)
    {
        printf("set_potentiometer: invalid POT value. valid range 0-255.\n");
    }
    else
    {
        enter_cmd_mode();
        send_command(170);
        send_command(bank);
        send_command(value);
    }
}

/** @brief sets all potentiometers to a value
 *  @param takes integer argument. Sets all potentiometers to value;
 *  @return void
 */
 void set_all_potentiometers(int value)
 {
     enter_cmd_mode();
     send_command(171);
     send_command(value);
 }

