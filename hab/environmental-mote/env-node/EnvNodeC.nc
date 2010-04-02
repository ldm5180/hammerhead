
// Copyright (c) 2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "env_msg.h"


#define DEFAULT_INTERVAL (1000 * 30)  /* milliseconds (ms) */
#define INTERVALS_BEFORE_CHECK 2
module EnvNodeC
{
    uses
    {
	interface Timer<TMilli> as SensorsCheck;
	interface Read<uint16_t> as Light;
	interface Read<uint16_t> as Voltage;
	interface Read<uint16_t> as Temperature;
	interface Leds;
	interface Boot;
	interface Send as GeneralRoot;
	interface StdControl as CollectionControl;
	interface SplitControl as RadioControl;
	interface LowPowerListening;
	interface CC2420Config;
    } /* uses */
} /* module EnvNodeC */


implementation
{
    int intervals = 0;

    void yellow_led_toggle() 
    {
	call Leds.led2Toggle(); //outgoing msg LED
    }
    
    void green_led_toggle() 
    {
	call Leds.led1Toggle(); //settings LED
    }
    
    void red_led_toggle() 
    {
	call Leds.led0Toggle(); //error LED
    }


    /* message buffers */
    message_t general_msgbuf;


    event void GeneralRoot.sendDone(message_t* msg, error_t ok) 
    {
	yellow_led_toggle();
    }

    
    event void CC2420Config.syncDone(error_t error)
    {
	call RadioControl.start();
    }

    event void Boot.booted()
    {
	call CC2420Config.setChannel(20);	
	call CC2420Config.sync();
    } /* Boot.booted() */


    event void RadioControl.startDone(error_t ok) 
    {
	call Leds.led1On();

	if (SUCCESS == ok)
	{
	    call CollectionControl.start();
	    //call LowPowerListening.setLocalWakeupInterval(60000);
	    call SensorsCheck.startPeriodic(DEFAULT_INTERVAL);
	}
	else
	{
	    red_led_toggle();
	}
    } /* RadioControl.startDone() */

    
    event void RadioControl.stopDone(error_t ok) {}


    event void SensorsCheck.fired()
    {
	env_general_msg_t* general_msg;
	
	intervals++;
        if (intervals >= 2) {
	    intervals = 0;
	} else {
	    return;
        }
	
	yellow_led_toggle();

	general_msg = call GeneralRoot.getPayload(&general_msgbuf, sizeof(env_general_msg_t));

        general_msg->node_id = TOS_NODE_ID;

	call Light.read();
    } /* SensorsCheck.fired() */


    event void Light.readDone(error_t ok, uint16_t val) 
    {
	env_general_msg_t* general_msg;

	if (SUCCESS != ok)
	{
	    red_led_toggle();
	    return;
	}

	general_msg = call GeneralRoot.getPayload(&general_msgbuf,
						  sizeof(env_general_msg_t));
	general_msg->photo = val;

	call Voltage.read();
    } /* Light.readDone() */


    event void Voltage.readDone(error_t ok, uint16_t val)
    {
	env_general_msg_t* general_msg;

	if (SUCCESS != ok)
	{
	    red_led_toggle();
	    return;
	}

	general_msg = call GeneralRoot.getPayload(&general_msgbuf,
						  sizeof(env_general_msg_t));
	general_msg->volt = val;

	call Temperature.read();
    } /* Voltage.readDone() */

    
    event void Temperature.readDone(error_t ok, uint16_t val) 
    {
	env_general_msg_t* general_msg;

	if (SUCCESS != ok)
	{
	    red_led_toggle();
	    return;
	}

	general_msg = call GeneralRoot.getPayload(&general_msgbuf,
						  sizeof(env_general_msg_t));
	general_msg->temp = val;

	call GeneralRoot.send(&general_msgbuf, sizeof(*general_msg));
	call Leds.led1Off();
    } /* Temperature.readDone() */

} /* implementation */
