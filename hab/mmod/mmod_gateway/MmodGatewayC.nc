
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#define NUM_MSG_BUFS 4

module MmodGatewayC
{
    uses
    { 
	interface Boot;
	interface SplitControl as SerialControl;
	interface SplitControl as RadioControl;
	interface LowPowerListening;

	interface DisseminationUpdate<mmod_settings_msg_t> as SettingsUpdate;
	interface Receive as SettingsReceive;

	interface StdControl as CollectionControl;
	interface StdControl as DisseminationControl;
	interface RootControl;
	interface Receive as GeneralReceive;
	interface AMSend as GeneralForward;
	interface Receive as AccelReceive;
	interface AMSend as AccelForward;
	interface Receive as SettingsNodeReceive;
	interface AMSend as SettingsNodeForward;
	interface CC2420Config;
	interface Leds;
    }
} /* module MmodGatewayC */


implementation
{
    void yellow_led_toggle() 
    {
	call Leds.led2Toggle();
    } /* yellow_led_toggle() */
    
    void green_led_toggle() 
    {
	call Leds.led1Toggle();
    } /* green_led_toggle() */
    
    void red_led_toggle() 
    {
	call Leds.led0Toggle();
    } /* red_led_toggle() */

    
    message_t fwd_msg[NUM_MSG_BUFS];
    bool fwd_busy[NUM_MSG_BUFS] = { FALSE };

    void msg_send_done(message_t* msg, error_t error)
    {
	uint8_t msg_index = 0;

	while ((NUM_MSG_BUFS > msg_index) && (msg != &fwd_msg[msg_index]))
	{
	    msg_index++;
	}

	if (NUM_MSG_BUFS > msg_index)
	{
	    /* the message has been sent and the buffer is no longer "busy" */
	    fwd_busy[msg_index] = FALSE;
	}
    } /* msg_send_done() */

    event void CC2420Config.syncDone(error_t error) 
    {
	call RadioControl.start();
    }

    event void Boot.booted()
    {
	call SerialControl.start();
	call CC2420Config.setChannel(20);	
	call CC2420Config.sync();
    } /* Boot.booted() */


    /* nothing interesting about starting/stopping the serial controller */
    event void SerialControl.startDone(error_t error) {}
    event void SerialControl.stopDone(error_t error) {}


    /* start up our collector and disseminator and make sure we are the root
     * for settings dissemination and sensor collections. */
    event void RadioControl.startDone(error_t error) 
    {
	if (SUCCESS == error)
	{
	    call LowPowerListening.setLocalDutyCycle(200); //really?
	    call DisseminationControl.start();
	    call CollectionControl.start();
	    call RootControl.setRoot();
	}
    } /* RadioControl.startDone() */

    event void RadioControl.stopDone(error_t error) {}

    
    /* settings recv'd need to be dissemination to the nodes */
    event message_t* SettingsReceive.receive(message_t* msg, 
					     void* payload,
					     uint8_t len)
    {
	mmod_settings_msg_t *new_settings = payload;

	yellow_led_toggle();

	if (sizeof(*new_settings) == len)
	{
	    call SettingsUpdate.change(new_settings);
	    yellow_led_toggle();
	}

	return msg;
    } /* SettingsReceive.receive() */


    event message_t* GeneralReceive.receive(message_t* msg, 
					    void* payload,
					    uint8_t len)
    {
	mmod_general_msg_t* new_general = payload;
	uint8_t msg_index = 0;

	red_led_toggle();

	while ((NUM_MSG_BUFS > msg_index) && (fwd_busy[msg_index]))
	{
	    msg_index++;
	}
	
	if ((NUM_MSG_BUFS > msg_index) && (sizeof(*new_general) == len))
	{
	    /* copy the payload into from the collector to the serial msg
	     * buffer and send the serial message */

	    /* first get a ptr to the payload of the fwd_msg to use */
	    mmod_general_msg_t* fwd_general = 
		call GeneralForward.getPayload(&fwd_msg[msg_index], 
					       sizeof(mmod_general_msg_t));
	    if (NULL != fwd_general)
	    {
		/* copy the data */
		*fwd_general = *new_general;
		/* if the send works, then set the buffer as "busy" */
		if (SUCCESS == call GeneralForward.send(AM_BROADCAST_ADDR, 
							&fwd_msg[msg_index], 
							sizeof(*fwd_general)))
		{
		    fwd_busy[msg_index] = TRUE;
		}
	    }
	}

	return msg;
    } /* GeneralReceive.receive() */


    event void GeneralForward.sendDone(message_t* msg, error_t error)
    {
	msg_send_done(msg, error);
	red_led_toggle();
    } /* GeneralForward.sendDone() */


    event message_t* SettingsNodeReceive.receive(message_t* msg, 
					    void* payload,
					    uint8_t len)
    {
	mmod_settings_msg_t* new_settings = payload;
	uint8_t msg_index = 0;
	
	red_led_toggle();

	while ((NUM_MSG_BUFS > msg_index) && (fwd_busy[msg_index]))
	{
	    msg_index++;
	}
	
	if ((NUM_MSG_BUFS > msg_index) && (sizeof(*new_settings) == len))
	{
	    /* copy the payload into from the collector to the serial msg
	     * buffer and send the serial message */

	    /* first get a ptr to the payload of the fwd_msg to use */
	    mmod_settings_msg_t* fwd_settings = 
		call SettingsNodeForward.getPayload(&fwd_msg[msg_index], 
						    sizeof(mmod_settings_msg_t));
	    if (NULL != fwd_settings)
	    {
		/* copy the data */
		*fwd_settings = *new_settings;
		/* if the send works, then set the buffer as "busy" */
		if (SUCCESS == 
		    call SettingsNodeForward.send(AM_BROADCAST_ADDR, 
						  &fwd_msg[msg_index], 
						  sizeof(*fwd_settings)))
		{
		    fwd_busy[msg_index] = TRUE;
		}
	    }
	}

	return msg;
    } /* SettingsNodeReceive.receive() */

    
    event void SettingsNodeForward.sendDone(message_t* msg, error_t error)
    {
	msg_send_done(msg, error);
	red_led_toggle();
    } /* SettingsNodeForward.sendDone() */


    event message_t* AccelReceive.receive(message_t* msg, 
					    void* payload,
					    uint8_t len)
    {
	mmod_accel_msg_t* new_accel = payload;
	uint8_t msg_index = 0;
	
	green_led_toggle();

	while ((NUM_MSG_BUFS > msg_index) && (fwd_busy[msg_index]))
	{
	    msg_index++;
	}
	
	if ((NUM_MSG_BUFS > msg_index) && (sizeof(*new_accel) == len))
	{
	    /* copy the payload into from the collector to the serial msg
	     * buffer and send the serial message */

	    /* first get a ptr to the payload of the fwd_msg to use */
	    mmod_accel_msg_t* fwd_accel = 
		call AccelForward.getPayload(&fwd_msg[msg_index], 
					       sizeof(mmod_accel_msg_t));
	    if (NULL != fwd_accel)
	    {
		/* copy the data */
		*fwd_accel = *new_accel;
		/* if the send works, then set the buffer as "busy" */
		if (SUCCESS == call AccelForward.send(AM_BROADCAST_ADDR, 
							&fwd_msg[msg_index], 
							sizeof(*fwd_accel)))
		{
		    fwd_busy[msg_index] = TRUE;
		}
	    }
	}

	return msg;
    } /* AccelReceive.receive() */


    event void AccelForward.sendDone(message_t* msg, error_t error)
    {
	msg_send_done(msg, error);
	green_led_toggle();
    } /* AccelForward.sendDone() */

} /* implementation */
