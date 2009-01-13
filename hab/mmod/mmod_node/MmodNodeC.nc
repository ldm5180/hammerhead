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

#define DEFAULT_ACCEL_THRESHOLD       10
#define DEFAULT_ACCEL_SAMPLES         16
#define DEFAULT_INTERVAL              5 /* milliseconds (ms) */
#define DEFAULT_ACCEL_STREAM_INTERVAL 200 /* microseconds (us) */
#define MAX_ACCEL_SAMPLES 16

module MmodNodeC
{
    uses
    {
	interface Timer<TMilli> as AccelCheck;
	interface Timer<TMilli> as SettingsCheck;
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
    uint8_t found = 0;

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
    message_t settings_msgbuf;

    /* time period control */
    uint32_t num_periods_per_sec;
    uint32_t period = 0;

    /* busy flags */
    bool gm_busy = FALSE; /* general message */
    bool ax_busy = FALSE; /* accel-x buffer */
    bool ay_busy = FALSE; /* accel-y buffer */
    bool sm_busy = FALSE; /* settings message */
    bool tgm_busy = FALSE; /* temp general message */
    bool sent_general = FALSE;
    bool is_recording = FALSE;
    bool just_sent = FALSE;
    bool first_time = TRUE;
    bool no_setting_send = FALSE;

    uint16_t count_below_thres = 0;

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


    void send_settings_msg()
    {
	mmod_settings_msg_t *settings_msg;
	if (FALSE == no_setting_send)
	{

	    red_led_toggle();
	    /* send out a quick settings msg */
	    sm_busy = TRUE;
	    settings_msg = 
		call SettingsRoot.getPayload(&settings_msgbuf,
					     sizeof(mmod_settings_msg_t));
	    *settings_msg = settings;
	    call SettingsRoot.send(&settings_msgbuf, sizeof(*settings_msg));
	    if (first_time)
	    {
		if (settings.heartbeat_time <= 20)
		{
		    call SettingsCheck.startPeriodic((settings.heartbeat_time >> 1) * 1000);
		}
		else
		{
		    call SettingsCheck.startPeriodic((settings.heartbeat_time - 10) * 1000);
		}	    
	    }
	    first_time = FALSE;
	}
	no_setting_send = FALSE;
    }

    void send_general_msg()
    {
	mmod_general_msg_t* general_msg;

	gm_busy = TRUE;
	hb_sec = 0; /* reset the heartbeat second counter */
	yellow_led_toggle();

	general_msg = 
	    call GeneralRoot.getPayload(&general_msgbuf,
					sizeof(mmod_general_msg_t));

	tgm_busy = TRUE;
	general_msg->node_id = TOS_NODE_ID;

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
	general_msg->accel_flags = settings.accel_flags;

	/* timer */
	general_msg->timestamp_id = tmp_general_msg.timestamp_id;
	general_msg->offset = tmp_general_msg.offset;

	tgm_busy = FALSE;

	call GeneralRoot.send(&general_msgbuf, sizeof(*general_msg));
	call Leds.led1Off();
    }



    event void SettingsRoot.sendDone(message_t *msg, error_t ok)
    {
	red_led_toggle();
	sm_busy = FALSE;
    }

   
    event void Boot.booted()
    {
	call Leds.led0Off();
	settings.node_id = TOS_NODE_ID;
	settings.thres_accel = DEFAULT_ACCEL_THRESHOLD;
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
	call Leds.led1On();

	if (SUCCESS == ok)
	{
	    call DisseminationControl.start();
	    call CollectionControl.start();
	    //call LowPowerListening.setLocalDutyCycle(200);

	    call SettingsCheck.startOneShot(1000);
	}
    } /* RadioControl.startDone() */

    
    event void RadioControl.stopDone(error_t ok) {}


    event void SettingsValue.changed()
    {
	const mmod_settings_msg_t* new_settings;
	mmod_general_msg_t* general_msg;

	new_settings = call SettingsValue.get();

	if ((TOS_NODE_ID != new_settings->node_id) && (new_settings->node_id != 0))
	{
	    if (tmp_general_msg.timestamp_id != new_settings->timestamp_id)
	    {
		no_setting_send = TRUE;
	    }
	}
	else if (new_settings->node_id == 0)
	{
	    call AccelCheck.stop();
	    if (tmp_general_msg.timestamp_id != new_settings->timestamp_id)
	    {
		no_setting_send = TRUE;
	    }
	    tmp_general_msg.timestamp_id = settings.timestamp_id = new_settings->timestamp_id;
	    tmp_general_msg.offset = 0;
	    
	    general_msg = 
		call GeneralRoot.getPayload(&general_msgbuf,
					    sizeof(mmod_general_msg_t));
	    general_msg->accel_x = 0;
	    general_msg->accel_y = 0;
	    tmp_general_msg.accel_x = 0;
	    tmp_general_msg.accel_y = 0;	    

	    call AccelCheck.startPeriodic(settings.sample_interval);

	    just_sent = FALSE;
	    found = FALSE;
	}

	if (tmp_general_msg.timestamp_id != new_settings->timestamp_id)
	{
	    no_setting_send = TRUE;
	}
	tmp_general_msg.timestamp_id = settings.timestamp_id = new_settings->timestamp_id;
	tmp_general_msg.offset = 0;

	/* copy everything over but the node id, we know who we are */
	settings.thres_accel = new_settings->thres_accel;
	settings.sample_interval = new_settings->sample_interval;
	settings.num_accel_samples = new_settings->num_accel_samples;
	settings.accel_sample_interval = new_settings->accel_sample_interval;
	settings.heartbeat_time = new_settings->heartbeat_time;

	settings.accel_flags = new_settings->accel_flags;

	if (settings.heartbeat_time <= 20)
	{
	    call SettingsCheck.startPeriodic((settings.heartbeat_time >> 1) * 1000);
	}
	else
	{
	    call SettingsCheck.startPeriodic((settings.heartbeat_time - 10) * 1000);
	}

        /* ensure the sample period timer is correct */
	num_periods_per_sec = 
	    (uint32_t)MSEC_PER_SEC / (nx_uint32_t)settings.sample_interval;

	/* send out a settings message to let the HAB know that the changes
	 * have been implemented */
	if (!sm_busy)
	{
	    send_settings_msg();
	}
    } /* SettingsValue.changed() */


    event void SettingsCheck.fired()
    {
	send_settings_msg();
    } /* SettingsCheck.fired() */


    /* every time the timer fires, check accel, check if other items need to
     * be checked and do it if needed. see if a new message needs to be sent
     * and send it if needed */
    event void AccelCheck.fired()
    {
	if ((found) && (!just_sent))
	{
	    send_general_msg();
	    just_sent = TRUE;
	}

	/* read the X accelerometer */
	if ((ACCEL_FLAG_X & settings.accel_flags) && (!ax_busy))
	{
	    green_led_toggle();
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
    } /* AccelCheck.fired() */


    task void check_accel_x()
    {
	uint8_t i, j;
	uint16_t avg[4] = {0};
	uint16_t tmp;
	uint16_t biggest = 0;

	tmp_general_msg.offset += settings.sample_interval;

	for (i = 0; i < num_x_samples; i++)
	{
	    for (j = 0; j <= 4; j++)
	    {
		avg[i] += accel_x_samples[i*j];
	    }
	    avg[i] /= num_x_samples;
	    avg_accum_x += avg[i];
	    num_readings_in_accum_x++;
	}


	/* if there are enough to make an average, record it */
	if (num_readings_in_accum_x >= num_readings_needed)
	{
	    cur_accel_avg_x = avg_accum_x >> 4;
	    num_readings_in_accum_x = avg_accum_x = 0;
	    call Leds.led1Off();
	}
	else if (0 == cur_accel_avg_x)
	{
	    ax_busy = FALSE;
	    /* if there is no average yet, return */
	    return;
	}

	for (i = 0; i < num_x_samples; i++)
	{
	    //get abs val
	    if (cur_accel_avg_x > avg[i])
	    {
		tmp = cur_accel_avg_x - avg[i];
	    }
	    else
	    {
		tmp = avg[i] - cur_accel_avg_x;
	    }
	
	    //if we just sent, report when we get back to a floor
	    if ((just_sent) && (tmp <= 4) && (found))
	    {
		tmp_general_msg.accel_x = tmp;
		send_general_msg();
		ax_busy = FALSE;
		call AccelCheck.stop();
		found = 0;
		return;
	    }
	    else if (just_sent)
	    {
		ax_busy = FALSE;
		return;
	    }

	    //find the biggest for the time period
	    if (tmp > biggest)
	    {
		biggest = tmp;
	    }
	}

        /* update general_msg regardless so it gets sent in next heartbeat */
	if (!tgm_busy) 
	{
	    /* only report the highest over the time period, this is reset 
             * when a message is sent */
	    if ((tmp_general_msg.accel_x < biggest) && (biggest > DEFAULT_ACCEL_THRESHOLD))
	    {
		tmp_general_msg.accel_x = biggest;
		found++;
	    }
	    else if (!is_recording)
	    {
		tmp_general_msg.accel_x = 0;
	    }
	}
	else
	{
	    red_led_toggle();
	}
	
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

	/* update general_msg regardless so it gets sent in next heartbeat */
	if (!tgm_busy)
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
		tmp_general_msg.accel_y = tmp;
	    }
	}
	else
	{
	    red_led_toggle();
	}
	
	ay_busy = FALSE;
    } /* check_accel_y() */


    event void ReadAccelXStream.readDone(error_t ok, uint32_t usActualPeriod)
    {
	green_led_toggle();
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
