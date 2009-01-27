
//
// Copyright (C) 2008-2009, Regents of the University of Colorado.
//


#include "mmod_msg.h"


configuration MmodNodeAppC {}

implementation
{
    components MmodNodeC, ActiveMessageC, MainC, LedsC, 
	new TimerMilliC() as AccelTimer,
	new TimerMilliC() as SettingsTimer;

#if defined(PLATFORM_MICAZ)
    components CC2420ActiveMessageC as Radio;
#elif defined(PLATFORM_IRIS)
    components ActiveMessageC as Radio;
#else
#error "MMOD Gateway application only supports MicaZ and Iris nodes."
#endif


    MmodNodeC.Boot -> MainC.Boot;
    MmodNodeC.AccelCheck -> AccelTimer;
    MmodNodeC.SettingsCheck -> SettingsTimer;
    MmodNodeC.Leds -> LedsC;
    MmodNodeC.RadioControl -> ActiveMessageC;
    MmodNodeC.LowPowerListening -> Radio;

    components CC2420ControlC;
    MmodNodeC.CC2420Config -> CC2420ControlC;

    /* wire up the sensors */
    components new PhotoC(), 
	new AccelXStreamC(), 
	new AccelYStreamC(), 
	new VoltageC(), 
	new TempC();

    MmodNodeC.Light -> PhotoC;
    MmodNodeC.Voltage -> VoltageC;
    MmodNodeC.Temperature -> TempC;
    MmodNodeC.ReadAccelXStream -> AccelXStreamC;
    MmodNodeC.ReadAccelYStream -> AccelYStreamC;

    components DisseminationC;
    MmodNodeC.DisseminationControl -> DisseminationC;

    components new DisseminatorC(mmod_settings_msg_t, DIS_MMOD_SETTINGS_MSG);
    MmodNodeC.SettingsValue -> DisseminatorC;


    components CollectionC, 
	new CollectionSenderC(COL_MMOD_GENERAL_MSG) as GeneralSender,  
	new CollectionSenderC(COL_MMOD_SETTINGS_MSG) as SettingsSender;
  
    MmodNodeC.GeneralRoot -> GeneralSender;
    MmodNodeC.SettingsRoot -> SettingsSender;
    MmodNodeC.CollectionControl -> CollectionC;
} /* implementation */
