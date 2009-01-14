#ifndef __SPEEDWAY_H_
#define __SPEEDWAY_H_


#include "ltkc.h"
#include "hardware-abstractor.h"


typedef struct {
    int antenna[5];  // there are only 4 antennas, but they start counting at 1
} node_data_t;


extern bionet_hab_t *hab;

extern int show_messages;
extern LLRP_tSConnection *pConn;
extern LLRP_tSTypeRegistry *pTypeRegistry;


// reader functions.
int speedway_connect(const char* reader_ip);
int speedway_configure(void);
int configure_gpi(void);
int set_gpo(int gpo_num, int state);
int read_gpis(int gpi[4]);
int checkConnectionStatus();
int scrubConfiguration();
void get_reader_config(void);

/**
 * @brief Poll for messages from the Speedway, process if found.
 *
 * @return 1 if it got a message
 * @return 0 if there was no message
 */
int poll_for_report();
void handle_tag_report_data(LLRP_tSTagReportData *pTagReportData);

int addROSpec();
int startROSpec();
void handleAntennaEvent(LLRP_tSAntennaEvent *pAntennaEvent);
int deleteAllROSpecs();
int enableROSpec();
void handleReaderExceptionEvent();
void handleReaderEventNotification(
	LLRP_tSReaderEventNotificationData *pNftData);
int resetConfigurationToFactoryDefaults();
int freeMessage(LLRP_tSMessage *pMessage);
int checkLLRPStatus(LLRP_tSLLRPStatus *pLLRPStatus, char *pWhatStr);
LLRP_tSMessage *recvMessage(int nMaxMS);
LLRP_tSMessage *transact(LLRP_tSMessage *pSendMsg);
void printXMLMessage(LLRP_tSMessage *pMessage);
int freeMessage(LLRP_tSMessage *pMessage);

#endif 



