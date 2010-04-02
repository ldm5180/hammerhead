
// Copyright (c) 2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "../env-node/env_msg.h"


configuration EnvGatewayAppC { }
implementation 
{
    components EnvGatewayC, MainC, LedsC, ActiveMessageC, SerialActiveMessageC;

#if defined(PLATFORM_MICAZ)
    components CC2420ActiveMessageC as Radio;
#elif defined(PLATFORM_IRIS)
    components ActiveMessageC as Radio;
#else
#error "ENV Gateway application only supports MicaZ and Iris nodes."
#endif

    EnvGatewayC.Boot -> MainC;
    EnvGatewayC.SerialControl -> SerialActiveMessageC;
    EnvGatewayC.RadioControl -> ActiveMessageC;
    EnvGatewayC.LowPowerListening -> Radio;
    EnvGatewayC.Leds -> LedsC;

    /* collection of sensor messages */
    components CollectionC, 
	new SerialAMSenderC(AM_ENV_GENERAL_MSG) as GeneralForwarder;
    
    EnvGatewayC.CollectionControl -> CollectionC;
    EnvGatewayC.RootControl -> CollectionC;
    EnvGatewayC.GeneralReceive -> CollectionC.Receive[COL_ENV_GENERAL_MSG];
    EnvGatewayC.GeneralForward -> GeneralForwarder;

    components CC2420ControlC;
    EnvGatewayC.CC2420Config -> CC2420ControlC;
}
