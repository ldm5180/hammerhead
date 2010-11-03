
// Copyright (c) 2008-2010, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef __SPEEDWAY_H_
#define __SPEEDWAY_H_


#include "ltkc.h"
#include "hardware-abstractor.h"


typedef struct {
    int still_here;
} node_data_t;

extern GMainLoop *main_loop;

extern bionet_hab_t *hab;

extern int show_messages;
extern int gpi_delay;
extern int gpi_polarity;
extern int num_scans;
extern int scan_idle;
extern int scan_timeout;
extern int immediate_trigger;
extern int null_trigger;
extern int gpi_trigger;
extern int simple_report;

extern llrp_u16_t rf_sensitivity;
extern llrp_u16_t rf_transmitpower;
extern llrp_u16_t antenna_id;
extern llrp_u16_t scrub_config;
extern llrp_u16_t rf_sense_index;
extern llrp_u16_t rf_txpower_index;

extern int use_sense_index;
extern int use_txpower_index;

//extern int tag_direction;

extern LLRP_tSConnection *pConn;
extern LLRP_tSTypeRegistry *pTypeRegistry;

extern int scans_left_to_do;


extern bionet_node_t *reader_node;
int make_reader_node(void);
int read_from_bionet(GIOChannel *unused_channel, GIOCondition unused_condition, gpointer unused_data);


// reader functions.
int speedway_connect(const char* reader_ip);
int speedway_configure(void);
int configure_reader(void);
int set_gpo(int gpo_num, int state);
int read_gpis(int gpi[4]);
int checkConnectionStatus();
int scrubConfiguration();
void get_reader_config(void);
int get_reader_capabilities (void);

/**
 * @brief Poll for messages from the Speedway, process if found.
 *
 * @return TRUE (1)
 */
int poll_reader();

void handle_tag_report_data(LLRP_tSTagReportData *pTagReportData);

int addROSpec_Immediate();
int addROSpec_Null();
int addROSpec_GPI();
int startROSpec();
int deleteAllROSpecs();
int enableROSpec();

LLRP_tSROReportSpec* getConfiguredROReportSpec(void);
int enableImpinjExtensions (void);
int addDirectionReporting (void);

void handleReaderEventNotification( LLRP_tSReaderEventNotificationData *pNftData);
void handleAntennaEvent(LLRP_tSAntennaEvent *pAntennaEvent);
void handleReaderExceptionEvent();
void handle_gpi_event(LLRP_tSGPIEvent *pGPIEvent);
void handle_interrupt(int sig);

int resetConfigurationToFactoryDefaults();
int freeMessage(LLRP_tSMessage *pMessage);
int checkLLRPStatus(LLRP_tSLLRPStatus *pLLRPStatus, char *pWhatStr);
LLRP_tSMessage *recvMessage(int nMaxMS);
LLRP_tSMessage *transact(LLRP_tSMessage *pSendMsg);
void printXMLMessage(LLRP_tSMessage *pMessage);
int freeMessage(LLRP_tSMessage *pMessage);

void usage(void);

#endif 



