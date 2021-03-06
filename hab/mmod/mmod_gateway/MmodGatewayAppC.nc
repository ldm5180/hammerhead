
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "../mmod_node/mmod_msg.h"


configuration MmodGatewayAppC { }
implementation 
{
    components MmodGatewayC, MainC, LedsC, ActiveMessageC, SerialActiveMessageC;

#if defined(PLATFORM_MICAZ)
    components CC2420ActiveMessageC as Radio;
#elif defined(PLATFORM_IRIS)
    components ActiveMessageC as Radio;
#else
#error "MMOD Gateway application only supports MicaZ and Iris nodes."
#endif

    MmodGatewayC.Boot -> MainC;
    MmodGatewayC.SerialControl -> SerialActiveMessageC;
    MmodGatewayC.RadioControl -> ActiveMessageC;
    MmodGatewayC.LowPowerListening -> Radio;
    MmodGatewayC.Leds -> LedsC;


    /* dissemination of settings messages */
    components DisseminationC;
    MmodGatewayC.DisseminationControl -> DisseminationC;
    /* wire a disseminator to send new settings to nodes and a serial recv'r to
     * get the msgs to disseminate from the host */
    components new DisseminatorC(mmod_settings_msg_t, DIS_MMOD_SETTINGS_MSG),
	new SerialAMReceiverC(AM_MMOD_SETTINGS_MSG) as SettingsReceiver;

    MmodGatewayC.SettingsReceive -> SettingsReceiver;
    MmodGatewayC.SettingsUpdate -> DisseminatorC;


    /* collection of sensor messages */
    components CollectionC, 
	new SerialAMSenderC(AM_MMOD_GENERAL_MSG) as GeneralForwarder, 
	new SerialAMSenderC(AM_MMOD_ACCEL_MSG) as AccelForwarder,
	new SerialAMSenderC(AM_MMOD_SETTINGS_MSG) as SettingsNodeForwarder;
    
    MmodGatewayC.CollectionControl -> CollectionC;
    MmodGatewayC.RootControl -> CollectionC;
    MmodGatewayC.GeneralReceive -> CollectionC.Receive[COL_MMOD_GENERAL_MSG];
    MmodGatewayC.GeneralForward -> GeneralForwarder;
    MmodGatewayC.AccelReceive -> CollectionC.Receive[COL_MMOD_ACCEL_MSG];
    MmodGatewayC.AccelForward -> AccelForwarder;
    MmodGatewayC.SettingsNodeReceive -> CollectionC.Receive[COL_MMOD_SETTINGS_MSG];
    MmodGatewayC.SettingsNodeForward -> SettingsNodeForwarder;

    components CC2420ControlC;
    MmodGatewayC.CC2420Config -> CC2420ControlC;
}
