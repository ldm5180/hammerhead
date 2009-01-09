
//
// Copyright (C) 2008-2009, Regents of the University of Colorado.
//

#ifndef __ALSA_HAB_H
#define __ALSA_HAB_H


#include <alsa/asoundlib.h>
#include <glib.h>

#include "hardware-abstractor.h"




// 
// the bionet audio format, expressed in ALSA terms
//

#define ALSA_CHANNELS (1)
#define ALSA_FORMAT   (SND_PCM_FORMAT_S16_LE)
#define ALSA_RATE     (9600)


// 
// other ALSA config info
//

#define ALSA_BUFFER_TIME_US (500 * 1000)
#define ALSA_PERIOD_TIME_US (100 * 1000)




//
// this holds info about an open alsa device
//

typedef struct {
    snd_pcm_t *pcm_handle;

    // these are for accessing the alsa stream
    // actually point into our big global list of pollfds, yuck
    struct pollfd *pollfd;
    int num_pollfds;

    uint buffer_time_us;
    snd_pcm_uframes_t buffer_size;

    uint period_time_us;
    snd_pcm_uframes_t period_size;

    int bytes_per_frame;
    int16_t *audio_buffer;
    int audio_buffer_frames;
} alsa_t;




// 
// local (to the alsa-hab) information, each stream gets a stream_info_t as
// its user_data
//

typedef struct {
    alsa_t *alsa;
    int num_clients;
} producer_info_t;

typedef struct {
    GSList *clients;
} consumer_info_t;

typedef struct {
    char *device;
    union {
        producer_info_t producer;
        consumer_info_t consumer;
    } info;
} stream_info_t;


//
// with ALSA Capture devices (aka bionet audio producers, aka microphones)
// we always wait for ALSA to produce some noise
//
// with ALSA Playback devices (aka bionet audio consumers, aka speakers) we
// wait for ALSA to consume the noise, unless there's no noise available
// from the client, in which case we pause ALSA and wait for the client to
// produce some noise for us
//

typedef enum {
    WAITING_FOR_CLIENT = 0,
    WAITING_FOR_ALSA
} who_are_we_waiting_for_t;


typedef struct {
    char *id;
    who_are_we_waiting_for_t waiting;
    alsa_t *alsa;
} client_t;




// each node (sound card) gets one of these
typedef struct {
    int hardware_is_still_there;
} node_user_data_t;




// 
// this hab
//

extern bionet_hab_t *this_hab;




void show_state(void);
void show_global_pollfds(void);
void show_client(client_t *client);


/**
 * @brief Rescan the list of available Alsa devices.
 *
 * This function gets run periodically (every 5 seconds) from the main
 * loop.
 *
 * For all Alsa devices that have been connected to the computer since last
 * we checked, we create a new Node with Streams for the first Alsa PCM
 * endpoints.
 *
 * For all Alsa devices that have been disconnected since last we checked,
 * we close the Alsa devices and report the corresponding Node missing.
 */
int discover_alsa_hardware(void);


/**
 * @brief Read from an Alsa device, publish to a Producer Stream
 *
 * @param[in] stream The Stream to publish on.
 */
int read_producer_stream(bionet_stream_t *stream);


alsa_t *open_alsa_device(char *device, snd_pcm_stream_t direction);
void close_alsa_device(alsa_t *alsa);
int xrun_handler(snd_pcm_t *handle, int err);
int check_alsa_poll(alsa_t *alsa);

int handle_client(bionet_stream_t *stream, client_t *client);
void disconnect_client(bionet_stream_t *stream, client_t *client);

void cb_stream_subscription(const char *client_id, const bionet_stream_t *stream);




#endif // __ALSA_HAB_H

