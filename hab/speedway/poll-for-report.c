
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "hardware-abstractor.h"

#include "ltkc.h"
#include "speedway.h"


#define RECEIVE_TIMEOUT 100


static void process_ro_access_report(LLRP_tSRO_ACCESS_REPORT *report) {
    LLRP_tSTagReportData *pTagReportData;
    int ni;


    // set all nodes to "not seen on any antenna"
    for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni ++) {
        bionet_node_t *node = bionet_hab_get_node_by_index(hab, ni);
        node_data_t *node_data = bionet_node_get_user_data(node);
        int i;

        if (node == reader_node) continue;

        for (i = 1; i <= 4; i ++) {
            node_data->antenna[i] = 0;
        }
    }


    // handle each TagReportData entry separately
    for(
        pTagReportData = report->listTagReportData;
        NULL != pTagReportData;
        pTagReportData = (LLRP_tSTagReportData *)pTagReportData->hdr.pNextSubParameter
    ) {
        handle_tag_report_data(pTagReportData);
    }


    // update seen-on-antenna information for all nodes
    for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni ++) {
        bionet_node_t *node = bionet_hab_get_node_by_index(hab, ni);
        node_data_t *node_data = bionet_node_get_user_data(node);
        int i;
        int node_changed;
        int node_still_here;

        if (node == reader_node) continue;

        node_changed = 0;
        node_still_here = 0;

        for (i = 1; i <= 4; i ++) {
            char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

            bionet_resource_t *resource;
            bionet_datapoint_t *d;
            bionet_value_t *v;

            int value;
            int r;

            sprintf(resource_id, "Antenna-%d", i);
            resource = bionet_node_get_resource_by_id(node, resource_id);
            if (resource == NULL) {
                g_warning("cannot find Resource %s:%s", bionet_node_get_id(node), resource_id);
                continue;
            }

            d = bionet_resource_get_datapoint_by_index(resource, 0);
            if (d != NULL) {
                v = bionet_datapoint_get_value(d);
                if (v == NULL) {
                    g_warning("cannot get datapoint value of Resource %s", bionet_resource_get_local_name(resource));
                    continue;
                }
                r = bionet_value_get_binary(v, &value);
                if (r != 0) {
                    g_warning("cannot get binary value from Resource %s", bionet_resource_get_local_name(resource));
                    continue;
                }
            }

            if ((d == NULL) || (value != node_data->antenna[i])) {
                value = node_data->antenna[i];
                bionet_resource_set_binary(resource, value, NULL);
                node_changed = 1;
            }

            if (value != 0) node_still_here = 1;
        }

        if (!node_still_here) {
            free(node_data);
            bionet_node_set_user_data(node, NULL);
            bionet_hab_remove_node_by_id(hab, bionet_node_get_id(node));
            hab_report_lost_node(bionet_node_get_id(node));
            bionet_node_free(node);
            ni --;
        } else if (node_changed) {
            hab_report_datapoints(node);
        }
    }
}




int poll_for_report() {
    LLRP_tSMessage *pMessage = NULL;
    const LLRP_tSTypeDescriptor *pType;

    // wait a little bit for a message
    //
    // FIXME: it'd be nice if the LLRP library exposed a file descriptor so
    // we could roll it into select in our main loop
    pMessage = recvMessage(RECEIVE_TIMEOUT);
    if (pMessage == NULL) {
        // timeout
        return 0;
    }


    /*
     * What happens here depends on what kind of message was received.
     * Use the type label (pType) to discriminate message types.
     */
    pType = pMessage->elementHdr.pType;

    /*
     * If this is a tag report, then process it.
     */
    if (&LLRP_tdRO_ACCESS_REPORT == pType) {
        process_ro_access_report((LLRP_tSRO_ACCESS_REPORT *)pMessage);
        freeMessage(pMessage);
        return 1;
    }

    /*
     * If this is a reader event ...
     */
    else if (&LLRP_tdREADER_EVENT_NOTIFICATION == pType) {
        LLRP_tSREADER_EVENT_NOTIFICATION *pNtf;
        LLRP_tSReaderEventNotificationData *pNtfData;

        pNtf = (LLRP_tSREADER_EVENT_NOTIFICATION *) pMessage;

        pNtfData = LLRP_READER_EVENT_NOTIFICATION_getReaderEventNotificationData(pNtf);

        if (pNtfData != NULL) {
            handleReaderEventNotification(pNtfData);
        } else {
            // Should never happen.
            g_warning("READER_EVENT_NOTIFICATION  without data");
        }
    } else {
        /*
         * Something unexpected happened.
         */
        g_warning("Ignoring unexpected message during monitor: %s", pType->pName);
    }

    freeMessage(pMessage);

    return 1;
}

