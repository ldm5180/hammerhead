
// Copyright (c) 2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#define NUM_MSG_BUFS 4

module EnvGatewayC
{
    uses
    { 
	interface Boot;
	interface SplitControl as SerialControl;
	interface SplitControl as RadioControl;
	interface LowPowerListening;

	interface StdControl as CollectionControl;
	interface RootControl;
	interface Receive as GeneralReceive;
	interface AMSend as GeneralForward;
	interface CC2420Config;
	interface Leds;
    }
} /* module EnvGatewayC */


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
     * for sensor collections. */
    event void RadioControl.startDone(error_t error) 
    {
	if (SUCCESS == error)
	{
	    //call LowPowerListening.setLocalDutyCycle(200); //really?
	    call CollectionControl.start();
	    call RootControl.setRoot();
	}
    } /* RadioControl.startDone() */

    event void RadioControl.stopDone(error_t error) {}

    
    event message_t* GeneralReceive.receive(message_t* msg, 
					    void* payload,
					    uint8_t len)
    {
	env_general_msg_t* new_general = payload;
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
	    env_general_msg_t* fwd_general = 
		call GeneralForward.getPayload(&fwd_msg[msg_index], 
					       sizeof(env_general_msg_t));
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

} /* implementation */
