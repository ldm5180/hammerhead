
//
// Copyright (C) 2004-2008, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G and NNC06CB40C.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of GNU General Public License version 2, as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
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