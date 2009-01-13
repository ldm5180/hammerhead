#ifndef __SPEEDWAY_H_
#define __SPEEDWAY_H_

#include "ltkc.h"
#include "hardware-abstractor.h"

extern bionet_hab_t *hab;

extern int g_verbose;
extern LLRP_tSConnection *pConn;
extern LLRP_tSTypeRegistry *pTypeRegistry;

// bionet functions.
int add_node(void);
void cb_set_resource(const char *node_id, const char *resource_id, 
	const char *value);

// reader functions.
int speedway_connect(const char* reader_ip);
int checkConnectionStatus();
int scrubConfiguration();

int poll_for_report();

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
LLRP_tSMessage *transact(LLRP_tSMessage *pSendMsg);
void printXMLMessage(LLRP_tSMessage *pMessage);
int freeMessage(LLRP_tSMessage *pMessage);

#endif 



