
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


#include "../mmod.h"
#include "mmod_msg.h"


#define MSEC_PER_SEC 1000

#define SEND_GENERAL_MSG 0x01
#define SEND_ACCEL_MSG   0x02

#define CHECK_X_ACCEL 0x01
#define CHECK_Y_ACCEL 0x02

#define DEFAULT_LIGHT_THRESHOLD       10
#define DEFAULT_ACCEL_THRESHOLD       20
#define DEFAULT_TEMPERATURE_THRESHOLD 5
#define DEFAULT_VOLTAGE_THRESHOLD     3
#define DEFAULT_ACCEL_SAMPLES         4
#define DEFAULT_INTERVAL              5 /* milliseconds (ms) */
#define DEFAULT_ACCEL_STREAM_INTERVAL 500 /* microseconds (us) */
#define MAX_ACCEL_SAMPLES 10

module MmodNodeC
{
    uses
    {
	interface Timer<TMilli> as AccelCheck;
	interface Timer<TMilli> as SettingsCheck;
	interface Read<uint16_t> as Light;
	interface Read<uint16_t> as Voltage;
	interface Read<uint16_t> as Temperature;
	interface ReadStream<uint16_t> as ReadAccelXStream;
	interface ReadStream<uint16_t> as ReadAccelYStream;
	interface Leds;
	interface Boot;
	interface DisseminationValue<mmod_settings_msg_t> as SettingsValue;
	interface Send as GeneralRoot;
	interface Send as SettingsRoot;
	interface StdControl as CollectionControl;
	interface StdControl as DisseminationControl;
	interface SplitControl as RadioControl;
	interface LowPowerListening;
    } /* uses */
} /* module MmodNodeC */


implementation
{
    mmod_settings_msg_t settings; /* incoming settings msg buffer */
    uint16_t cur_accel_avg_x = 0;
    uint16_t avg_accum_x = 0;
    uint16_t num_readings_in_accum_x;
    uint16_t cur_accel_avg_y = 0;
    uint16_t avg_accum_y = 0;
    uint16_t num_readings_in_accum_y;
    uint16_t num_readings_needed;

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
//    message_t accel_msgbuf;
    message_t settings_msgbuf;

    /* time period control */
    uint32_t num_periods_per_sec;
    uint32_t period = 0;

    /* busy flags */
    bool gm_busy = FALSE; /* general message */
    bool am_busy = FALSE; /* accel message */
    bool ax_busy = FALSE; /* accel-x buffer */
    bool ay_busy = FALSE; /* accel-y buffer */
    bool sm_busy = FALSE; /* settings message */

    /* number of samples taken this round */
    uint16_t num_x_samples;
    uint16_t num_y_samples;

    /* local storge of new values for general msg */
    mmod_general_msg_t tmp_general_msg;

    /* accel samples for X and Y verticies */
    uint16_t accel_x_samples[MAX_ACCEL_SAMPLES];
    uint16_t accel_y_samples[MAX_ACCEL_SAMPLES];

    /* send needed flags */
    uint8_t need_send = 0;

    /* seconds since heartbeat */
    uint8_t hb_sec = 0;

    event void GeneralRoot.sendDone(message_t* msg, error_t ok) 
    {
	need_send &= ~SEND_GENERAL_MSG;
	gm_busy = FALSE;
	yellow_led_toggle();
    }

#if 0
    event void AccelRoot.sendDone(message_t *msg, error_t ok) 
    {
	need_send &= ~SEND_ACCEL_MSG;
	am_busy = FALSE;
	yellow_led_toggle();
    }
#endif
    
    event void SettingsRoot.sendDone(message_t *msg, error_t ok)
    {
	sm_busy = FALSE;
	yellow_led_toggle();
    }

   
    event void Boot.booted()
    {
	settings.node_id = TOS_NODE_ID;
	settings.thres_light = DEFAULT_LIGHT_THRESHOLD;
	settings.thres_accel = DEFAULT_ACCEL_THRESHOLD;
	settings.thres_temp = DEFAULT_TEMPERATURE_THRESHOLD;
	settings.thres_volt = DEFAULT_VOLTAGE_THRESHOLD;
	settings.sample_interval = DEFAULT_INTERVAL; //milliseconds (ms)
	settings.num_accel_samples = DEFAULT_ACCEL_SAMPLES;
	settings.accel_sample_interval = DEFAULT_ACCEL_STREAM_INTERVAL;
	settings.heartbeat_time = DEFAULT_HEARTBEAT_TIME;
	settings.accel_flags = ACCEL_FLAG_X;
	num_readings_needed = 64;
	num_periods_per_sec = 
	    ((uint32_t)MSEC_PER_SEC / (nx_uint32_t)settings.sample_interval);
	call RadioControl.start();
    } /* Boot.booted() */


    event void RadioControl.startDone(error_t ok) 
    {
	mmod_settings_msg_t *settings_msg;

	call Leds.led1On();

	if (SUCCESS == ok)
	{
	    call DisseminationControl.start();
	    call CollectionControl.start();
	    call AccelCheck.startPeriodic(settings.sample_interval);
	    call LowPowerListening.setLocalDutyCycle(200);

	    /* send out a quick settings msg */
	    yellow_led_toggle();
	    sm_busy = TRUE;
	    settings_msg = 
		call SettingsRoot.getPayload(&settings_msgbuf,
					    sizeof(mmod_settings_msg_t));
	    *settings_msg = settings;
	    call SettingsRoot.send(&settings_msgbuf, sizeof(*settings_msg));
	    call SettingsCheck.startPeriodic(5000);
	}
	else
	{
	    red_led_toggle();
	}
    } /* RadioControl.startDone() */

    
    event void RadioControl.stopDone(error_t ok) {}


    event void SettingsValue.changed()
    {
	mmod_settings_msg_t *settings_msg;
	const mmod_settings_msg_t* new_settings;

	green_led_toggle();
	new_settings = call SettingsValue.get();

	if (TOS_NODE_ID != new_settings->node_id)
	{
	    green_led_toggle();
	    return;
	}

	/* copy everything over but the node id, we know who we are */
	settings.thres_light = new_settings->thres_light;
	settings.thres_accel = new_settings->thres_accel;
	settings.thres_temp = new_settings->thres_temp;
	settings.thres_volt = new_settings->thres_volt;
	settings.sample_interval = new_settings->sample_interval;
	settings.num_accel_samples = new_settings->num_accel_samples;
	settings.accel_sample_interval = new_settings->accel_sample_interval;
	settings.heartbeat_time = new_settings->heartbeat_time;
	if (settings.accel_flags != new_settings->accel_flags)
	{
	    mmod_general_msg_t* general_msg;
	    general_msg = 
		call GeneralRoot.getPayload(&general_msgbuf,
					    sizeof(mmod_general_msg_t));
	    general_msg->accel_x = 0;
	    general_msg->accel_y = 0;
	    tmp_general_msg.accel_x = 0;
	    tmp_general_msg.accel_y = 0;
	}
	settings.accel_flags = new_settings->accel_flags;

	/* ensure the sample period timer is correct */
	num_periods_per_sec = 
	    (uint32_t)MSEC_PER_SEC / (nx_uint32_t)settings.sample_interval;
	call AccelCheck.startPeriodic(settings.sample_interval);

	/* send out a settings message to let the HAB know that the changes
	 * have been implemented */
	if (!sm_busy)
	{
	    sm_busy = TRUE;
	    yellow_led_toggle();
	    settings_msg = 
		call SettingsRoot.getPayload(&settings_msgbuf,
					     sizeof(mmod_settings_msg_t));
	    *settings_msg = settings;
	    call SettingsRoot.send(&settings_msgbuf, sizeof(*settings_msg));
	}

	need_send |= SEND_GENERAL_MSG;
	green_led_toggle();
    } /* SettingsValue.changed() */


    event void SettingsCheck.fired()
    {
	mmod_settings_msg_t *settings_msg;	

	if (sm_busy)
	{
	    return;
	}

	sm_busy = TRUE;
	yellow_led_toggle();
	settings_msg = 
	    call SettingsRoot.getPayload(&settings_msgbuf,
					sizeof(mmod_settings_msg_t));
	*settings_msg = settings;
	call SettingsRoot.send(&settings_msgbuf, sizeof(*settings_msg));	
    } /* SettingsCheck.fired() */


    /* every time the timer fires, check accel, check if other items need to
     * be checked and do it if needed. see if a new message needs to be sent
     * and send it if needed */
    event void AccelCheck.fired()
    {
	mmod_general_msg_t* general_msg;
	
        /* check if there are any messages to send and send em */
	if ((!gm_busy) && (need_send & SEND_GENERAL_MSG))
	{
	    gm_busy = TRUE;
	    hb_sec = 0; /* reset the heartbeat second counter */
	    yellow_led_toggle();

	    general_msg = 
		call GeneralRoot.getPayload(&general_msgbuf,
					    sizeof(mmod_general_msg_t));

	    general_msg->node_id = TOS_NODE_ID;
	    general_msg->volt = tmp_general_msg.volt;
	    general_msg->temp = tmp_general_msg.temp;

	    if (ACCEL_FLAG_X & settings.accel_flags)
	    {
		general_msg->accel_x = tmp_general_msg.accel_x;
		tmp_general_msg.accel_x = 0; /* reset me */
	    }
	    else
	    {
		tmp_general_msg.accel_x++;
		general_msg->accel_x = tmp_general_msg.accel_x;
	    }

	    if ((ACCEL_FLAG_X & settings.accel_flags)
		&& (0 == (ACCEL_FLAG_Y & settings.accel_flags)))
	    {
		tmp_general_msg.accel_y++;
		general_msg->accel_y = tmp_general_msg.accel_y;
	    }
	    else
	    {
		general_msg->accel_y = tmp_general_msg.accel_y;
		tmp_general_msg.accel_y = 0;
	    }
	    general_msg->photo = tmp_general_msg.photo;
	    general_msg->accel_flags = settings.accel_flags;

	    call GeneralRoot.send(&general_msgbuf, sizeof(*general_msg));
	    call Leds.led1Off();
	}
#if 0
	else if ((!am_busy) && (need_send & SEND_ACCEL_MSG))
	{
	    am_busy = TRUE;
	    yellow_led_toggle();
	    call AccelRoot.send(&accel_msgbuf, sizeof(mmod_accel_msg_t));
	}
#endif

	/* every Nth period, check the light and temp */
	if (0 == period)
	{
	    period++;
	    if ((settings.heartbeat_time <= hb_sec) && (!gm_busy))
	    {
		need_send |= SEND_GENERAL_MSG;
	    }
	    if ((!gm_busy) && (0 == (need_send & SEND_GENERAL_MSG)))
	    {
		call Light.read();
		call Voltage.read();
		call Temperature.read();
	    }

	    /* a second has passed so bump the heartbeat second counter */
	    hb_sec++;
	}
	else
	{
	    /* if we have surpassed the number of sample periods in 1/2 sec then
	     * note reset the period counter. otherwise bump it. use 1/2 sec so
	     * that the heartbeat is sent about 1/2 the requested time and we
	     * are safer to not miss any */
	    period = (period >= (num_periods_per_sec>>1)) ? 0 : period + 1;
	}

//	if ((0 == (need_send & SEND_ACCEL_MSG) && (!am_busy)))
//	{
	/* read the X accelerometer */
	if ((ACCEL_FLAG_X & settings.accel_flags) && (!ax_busy))
	{
	    ax_busy = TRUE;
	    num_x_samples = settings.num_accel_samples;
	    call ReadAccelXStream.postBuffer(accel_x_samples, 
					     num_x_samples);
	    call ReadAccelXStream.read(settings.accel_sample_interval);
	}
	
	/* read the Y accelerometer */
	if ((ACCEL_FLAG_Y & settings.accel_flags) && (!ay_busy))
	{
	    ay_busy = TRUE;
	    num_y_samples = settings.num_accel_samples;
	    call ReadAccelYStream.postBuffer(accel_y_samples, 
					     num_y_samples);
	    call ReadAccelYStream.read(settings.accel_sample_interval);
	}
//	}
    } /* AccelCheck.fired() */


    event void Light.readDone(error_t ok, uint16_t val) 
    {
	mmod_general_msg_t* general_msg;
	uint16_t d;

	if (SUCCESS != ok)
	{
	    red_led_toggle();
	    return;
	}

	general_msg = call GeneralRoot.getPayload(&general_msgbuf,
						  sizeof(mmod_general_msg_t));

	/* get the absolute difference between last sent and current */
	if (general_msg->photo < val)
	{
	    d = val - general_msg->photo;
	}
	else
	{
	    d = general_msg->photo - val;
	}

	/* flag a general message to be sent if we have hit the threshold */
//	if ((settings.thres_light < d) && (!gm_busy))
//	{
//	    need_send |= SEND_GENERAL_MSG;
//	}

	/* always put things in the tmp msg. this gets copied into real msg
	 * when it is sent. this is so that heartbeats can always contain
	 * up-to-date info but we are not comparing against moving target */
	if (!gm_busy)
	{
	    tmp_general_msg.photo = val;
	}
    } /* Light.readDone() */


    event void Voltage.readDone(error_t ok, uint16_t val)
    {
	mmod_general_msg_t* general_msg;
	uint16_t d;

	if (SUCCESS != ok)
	{
	    red_led_toggle();
	    return;
	}

	general_msg = call GeneralRoot.getPayload(&general_msgbuf,
						  sizeof(mmod_general_msg_t));

	/* get the absolute difference between last sent and current */
	if (general_msg->volt < val)
	{
	    d = val - general_msg->volt;
	}
	else
	{
	    d = general_msg->volt - val;
	}

	/* flag a general message to be sent if we have hit the threshold */
//	if ((settings.thres_volt < d) && (!gm_busy))
//	{
//	    need_send |= SEND_GENERAL_MSG;
//	}

	/* always put things in the tmp msg. this gets copied into real msg
	 * when it is sent. this is so that heartbeats can always contain
	 * up-to-date info but we are not comparing against moving target */
	if (!gm_busy)
	{
	    tmp_general_msg.volt = val;
	}	
    } /* Voltage.readDone() */

    
    event void Temperature.readDone(error_t ok, uint16_t val) 
    {
	mmod_general_msg_t* general_msg;
	uint16_t d;

	if (SUCCESS != ok)
	{
	    red_led_toggle();
	    return;
	}

	general_msg = call GeneralRoot.getPayload(&general_msgbuf,
						  sizeof(mmod_general_msg_t));

	/* get the absolute difference between last sent and current */
	if (general_msg->temp < val)
	{
	    d = val - general_msg->temp;
	}
	else
	{
	    d = general_msg->temp - val;
	}

	/* flag a general message to be sent if we have hit the threshold */
//	if ((settings.thres_temp < d) && (!gm_busy))
//	{
//	    need_send |= SEND_GENERAL_MSG;
//	}

	/* always put things in the tmp msg. this gets copied into real msg
	 * when it is sent. this is so that heartbeats can always contain
	 * up-to-date info but we are not comparing against moving target */
	if (!gm_busy)
	{
	    tmp_general_msg.temp = val;
	}
    } /* Temperature.readDone() */


    task void check_accel_x()
    {
	uint8_t i;
	uint16_t avg = 0;
	uint16_t tmp;

	for (i = 0; i < num_x_samples; i++)
	{
	    avg += accel_x_samples[i];
	}
	avg /= num_x_samples;

	avg_accum_x += avg;
	num_readings_in_accum_x++;

	/* if there are enough to make an average, record it */
	if (num_readings_in_accum_x >= num_readings_needed)
	{
	    cur_accel_avg_x = avg_accum_x >> 6;
	    num_readings_in_accum_x = avg_accum_x = 0;
	}
	else if (0 == cur_accel_avg_x)
	{
	    ax_busy = FALSE;
	    /* if there is no average yet, return */
	    return;
	}

	
#if 0
	if (!am_busy)
	{
	    am_busy = TRUE;
	}
	else
	{
	    ax_busy = FALSE;
	    return;
	}

	//get the accel msg payload ptr
	accel_msg = call AccelRoot.getPayload(&accel_msgbuf,
					      sizeof(mmod_accel_msg_t));
	accel_msg->node_id = TOS_NODE_ID;

	if ((cur_accel_avg_x > avg)
	    && (settings.thres_accel < cur_accel_avg_x - avg))
	{
	    tmp = cur_accel_avg_x - avg;
	    if (((tmp > accel_msg->accel_x) && 
		 (tmp - accel_msg->accel_x > settings.thres_accel))
		|| 
		((tmp <= accel_msg->accel_x) && 
		 (accel_msg->accel_x - tmp > settings.thres_accel)))
	    {
		accel_msg->accel_x = tmp;
		//need_send |= SEND_ACCEL_MSG;
	    }
	}
	else if ((avg >= cur_accel_avg_x)
		 && (settings.thres_accel < avg - cur_accel_avg_x))
	{
	    tmp = avg - cur_accel_avg_x;
	    if (((tmp > accel_msg->accel_x) && 
		 (tmp - accel_msg->accel_x > settings.thres_accel))
		|| 
		((tmp <= accel_msg->accel_x) && 
		 (accel_msg->accel_x - tmp > settings.thres_accel)))
	    {
		accel_msg->accel_x = tmp;
		//need_send |= SEND_ACCEL_MSG;
	    }
	}
#endif

	/* update general_msg regardless so it gets sent in next heartbeat */
	if (!gm_busy)
	{
	    if (cur_accel_avg_x > avg)
	    {
		tmp = cur_accel_avg_x - avg;
	    }
	    else
	    {
		tmp = avg - cur_accel_avg_x;
	    }
	    
	    /* only report the highest over the time period, this is reset 
             * when a message is sent */
	    if (tmp_general_msg.accel_x < tmp)
	    {
//		if ((tmp - tmp_general_msg.accel_x) > settings.thres_accel)
//		{
//		    need_send |= SEND_GENERAL_MSG;
//		}
		tmp_general_msg.accel_x = tmp;
	    }
	}
	
	am_busy = FALSE;
	ax_busy = FALSE;
    } /* check_accel_x() */


    task void check_accel_y()
    {
	uint8_t i;
	uint16_t avg = 0;
	uint16_t tmp;
	for (i = 0; i < num_y_samples; i++)
	{
	    avg += accel_y_samples[i];
	}
	avg /= num_y_samples;

	avg_accum_y += avg;
	num_readings_in_accum_y++;

	/* if there are enough to make an average, record it */
	if (num_readings_in_accum_y >= num_readings_needed)
	{
	    cur_accel_avg_y = avg_accum_y >> 6;
	    num_readings_in_accum_y = avg_accum_y = 0;
	}
	else if (0 == cur_accel_avg_y)
	{
	    ay_busy = FALSE;
	    /* if there is no average yet, return */
	    return;
	}

#if 0	
	if (!am_busy)
	{
	    am_busy = TRUE;
	}
	else
	{
	    ay_busy = FALSE;
	    return;
	}

	//get the accel msg payload ptr
	accel_msg = call AccelRoot.getPayload(&accel_msgbuf,
					      sizeof(mmod_accel_msg_t));
	accel_msg->node_id = TOS_NODE_ID;

	if ((cur_accel_avg_y > avg)
	    && (settings.thres_accel < cur_accel_avg_y - avg))
	{
	    tmp = cur_accel_avg_y - avg;
	    if (tmp != accel_msg->accel_y)
	    {
		accel_msg->accel_y = tmp;
		//need_send |= SEND_ACCEL_MSG;
	    }
	}
	else if ((avg >= cur_accel_avg_y)
		 && (settings.thres_accel < avg - cur_accel_avg_y))
	{
	    tmp = cur_accel_avg_y - avg;
	    if (tmp != accel_msg->accel_y)
	    {
		accel_msg->accel_y = tmp;
		//need_send |= SEND_ACCEL_MSG;
	    }
	}
#endif

	/* update general_msg regardless so it gets sent in next heartbeat */
	if (!gm_busy)
	{
	    if (cur_accel_avg_y > avg)
	    {
		tmp = cur_accel_avg_y - avg;
	    }
	    else
	    {
		tmp = avg - cur_accel_avg_y;
	    }
	    
	    if (tmp_general_msg.accel_y < tmp)
	    {
//		if ((tmp - tmp_general_msg.accel_y) > settings.thres_accel)
//		{
//		    need_send |= SEND_GENERAL_MSG;
//		}
		tmp_general_msg.accel_y = tmp;
	    }
	}
	
	am_busy = FALSE;
	ay_busy = FALSE;
    } /* check_accel_y() */


    event void ReadAccelXStream.readDone(error_t ok, uint32_t usActualPeriod)
    {
	if (SUCCESS == ok)
	{
	    post check_accel_x();
	}
	else
	{
	    red_led_toggle();
	}
    }


    event void ReadAccelYStream.readDone(error_t ok, uint32_t usActualPeriod)
    {
	if (SUCCESS == ok)
	{
	    post check_accel_y();
	}
	else
	{
	    red_led_toggle();
	}
    }


    event void ReadAccelXStream.bufferDone(error_t ok, 
					   uint16_t* buf, 
					   uint16_t cnt) {}
    event void ReadAccelYStream.bufferDone(error_t ok, 
					   uint16_t* buf, 
					   uint16_t cnt) {}
} /* implementation */