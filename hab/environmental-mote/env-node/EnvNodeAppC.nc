
// Copyright (c) 2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "env_msg.h"


configuration EnvNodeAppC {}

implementation
{
    components EnvNodeC, ActiveMessageC, MainC, LedsC, 
	new TimerMilliC() as SensorsTimer;

#if defined(PLATFORM_MICAZ)
    components CC2420ActiveMessageC as Radio;
#elif defined(PLATFORM_IRIS)
    components ActiveMessageC as Radio;
#else
#error "ENV Gateway application only supports MicaZ and Iris nodes."
#endif


    EnvNodeC.Boot -> MainC.Boot;
    EnvNodeC.SensorsCheck -> SensorsTimer;
    EnvNodeC.Leds -> LedsC;
    EnvNodeC.RadioControl -> ActiveMessageC;
    EnvNodeC.LowPowerListening -> Radio;

    components CC2420ControlC;
    EnvNodeC.CC2420Config -> CC2420ControlC;

    /* wire up the sensors */
    components new PhotoC(), 
	new VoltageC(), 
	new TempC();

    EnvNodeC.Light -> PhotoC;
    EnvNodeC.Voltage -> VoltageC;
    EnvNodeC.Temperature -> TempC;

    components CollectionC, 
	new CollectionSenderC(COL_ENV_GENERAL_MSG) as GeneralSender;

  
    EnvNodeC.GeneralRoot -> GeneralSender;
    EnvNodeC.CollectionControl -> CollectionC;
} /* implementation */
