#ifndef __SPEEDWAY_H_
#define __SPEEDWAY_H_

#include "ltkc.h"
#include "hardware-abstractor.h"

extern int g_verbose;
extern LLRP_tSConnection *pConn;
extern LLRP_tSTypeRegistry *pTypeRegistry;

// bionet functions.
int add_node(bionet_hab_t *hab);
void cb_set_resource(const char *node_id, const char *resource_id, 
	const char *value);

// reader functions.
int speedway_connect(const char* reader_ip);
int checkConnectionStatus();
int scrubConfiguration();
int awaitAndPrintReport();
int addROSpec();
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
int startROSpec();
void printTagReportData(LLRP_tSRO_ACCESS_REPORT *pRO_ACCESS_REPORT);
LLRP_tSMessage *transact(LLRP_tSMessage *pSendMsg);
void printXMLMessage(LLRP_tSMessage *pMessage);
void printOneTagReportData(LLRP_tSTagReportData *pTagReportData);
int freeMessage(LLRP_tSMessage *pMessage);

#endif 



