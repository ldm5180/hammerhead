#ifndef __SPEEDWAY_H_
#define __SPEEDWAY_H_


#include "ltkc.h"
#include "hardware-abstractor.h"


typedef struct {
    int antenna[5];  // there are only 4 antennas, but they start counting at 1
} node_data_t;

extern GMainLoop *main_loop;

extern bionet_hab_t *hab;

extern int show_messages;
extern int gpi_delay;

extern LLRP_tSConnection *pConn;
extern LLRP_tSTypeRegistry *pTypeRegistry;

extern int reader_is_busy;


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

/**
 * @brief Poll for messages from the Speedway, process if found.
 *
 * @return TRUE (1)
 */
int poll_reader();

void handle_tag_report_data(LLRP_tSTagReportData *pTagReportData);

int addROSpec();
int startROSpec();
int deleteAllROSpecs();
int enableROSpec();

void handleReaderEventNotification( LLRP_tSReaderEventNotificationData *pNftData);
void handleAntennaEvent(LLRP_tSAntennaEvent *pAntennaEvent);
void handleReaderExceptionEvent();
void handle_gpi_event(LLRP_tSGPIEvent *pGPIEvent);

int resetConfigurationToFactoryDefaults();
int freeMessage(LLRP_tSMessage *pMessage);
int checkLLRPStatus(LLRP_tSLLRPStatus *pLLRPStatus, char *pWhatStr);
LLRP_tSMessage *recvMessage(int nMaxMS);
LLRP_tSMessage *transact(LLRP_tSMessage *pSendMsg);
void printXMLMessage(LLRP_tSMessage *pMessage);
int freeMessage(LLRP_tSMessage *pMessage);

void usage(void);

#endif 



