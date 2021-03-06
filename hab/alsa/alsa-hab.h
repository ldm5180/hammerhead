
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


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
// A Producer Stream reads audio from Alsa and publishes it, using
// hab_publish_stream(), to all interested Bionet Clients.  It has a single
// Alsa handle that it reads the audio data from.  It keeps track of how
// many clients are subscribed to this Producer Stream using the
// stream-subscription and stream-unsubscription events and callbacks.
//

typedef struct {
    alsa_t *alsa;
    int num_clients;
} producer_info_t;


// 
// A Consumer Stream reads audio data from multiple Clients, and sends each
// Client's data to a separate Alsa handle.  Alsa mixes it all together.
//

typedef struct {
    char *id;
    alsa_t *alsa;
} client_t;

typedef struct {
    GSList *clients;
} consumer_info_t;


// 
// local to the alsa-hab, each stream gets a stream_info_t as its user_data
//

typedef struct {
    char *device;
    union {
        producer_info_t producer;
        consumer_info_t consumer;
    } info;
} stream_info_t;




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


/**
 * @brief Open an Alsa device.
 *
 * @param[in] device The device to open (you get this from
 *     discover_alsa_hardware()).
 *
 * @param[in] direction SND_PCM_STREAM_PLAYBACK or SND_PCM_STREAM_CAPTURE.
 *
 * @return The alsa_t corresponding to the Alsa device on success; NULL on
 *     failure.
 */
alsa_t *open_alsa_device(char *device, snd_pcm_stream_t direction);


void close_alsa_device(alsa_t *alsa);
int xrun_handler(snd_pcm_t *handle, int err);


/**
 * @brief Checks to see if an Alsa handle is ready for I/O.
 *
 * @param[in] alsa The Alsa handle to poll.
 */
int check_alsa_poll(alsa_t *alsa);


void disconnect_client(bionet_stream_t *stream, client_t *client);

void cb_stream_subscription(const char *client_id, const bionet_stream_t *stream);
void cb_stream_unsubscription(const char *client_id, const bionet_stream_t *stream);

void cb_stream_data(const char *client_id, bionet_stream_t *stream, const void *data, unsigned int size);




#endif // __ALSA_HAB_H

