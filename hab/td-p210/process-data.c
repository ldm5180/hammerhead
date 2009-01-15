
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <glib.h>

#include "td-radio.h"
#include "td-radio-scan.h"

static int expected_segment_number = 0;

void scan_info();

/*
 * 
 */
int process_data(char *buffer) 
{
	bool debug = true;
	int j = 0;

	if (debug) {
		g_message("process_data(): enter");
	}

	tdScan_t *scan = (tdScan_t *) buffer;

	if (debug) {
		g_message("\tscanNumber: %ld", scan->scanNumber);
		g_message("\tsegmentNumber: %d", scan->segmentNumber);
		g_message("\tnumScanPointsTotal: %ld", scan->numScanPointsTotal); 
		g_message("\tmaxScanPointsSegment: %d", scan->maxScanPointsSegment); 
		g_message("\tnumScanPointsSegment: %ld", scan->numScanPointsSegment); 
		g_message("\tscanRateRx: %d", scan->scanRateRx); 
		g_message("\tscanRateTx: %d", scan->scanRateTx); 
	}

	if (scan->scanStatus == NO_LOCK) {
		g_message("packet is \"scanless\"");

		return NO_SCAN;
	}

	if (scan->segmentNumber != expected_segment_number) {
		/*
		 * If the expected segment doesn't arrive, then the entire scan
		 * is thrown out and we wait for the next scan's segment starting with
		 * zero.
		 *
		 * This should only happen when connecting in the middle of a current
		 * scan.
		 */
		g_warning("Segment number %d does not match %d.", 
			scan->segmentNumber, expected_segment_number);
		g_warning("Total segments, %d", scan->segmentsTotal);

		expected_segment_number = 0;

		return WRONG_SEGMENT;
	}

	if (debug) {
		g_message("\tsN * mSPS = %d", (scan->segmentNumber * 
			scan->maxScanPointsSegment));
		g_message("\tscan->scanData %hn", scan->scanData);
	}

	g_message("\tsegmentNumber: %d", scan->segmentNumber);
	g_message("\tmaxScanPointsSegment: %d", scan->maxScanPointsSegment); 
	g_message("\tscanData: %hn", scan->scanData);
	g_message("\tnumScanPointsSegment: %ld", scan->numScanPointsSegment); 

	memcpy((void *) &complete_scan[scan->segmentNumber * 
		scan->maxScanPointsSegment], (const void *) scan->scanData,
		(scan->numScanPointsSegment * sizeof(short signed int)));

	g_message("\tafter memcpy");

	if (scan->segmentNumber < (scan->segmentsTotal - 1)) {
		g_message("segment %d < segmentsTotal: %d, get another segment",
			scan->segmentNumber, (scan->segmentsTotal - 1));
		expected_segment_number = scan->segmentNumber + 1;

		return NEXT_SEGMENT;
	}
	else {
		expected_segment_number = 0;

		++current_range_count;

		scan_points = scan->numScanPointsTotal;

		//memcpy(scan_data, complete_scan, scan->numScanPointsTotal);
		for (j = 1; j < scan_points; j++) {
			scan_data[j] = (double) complete_scan[j];
		}
	}

	if (debug) {
		g_message("process_data(): enter");
	}

	return SCAN_COMPLETE;
}

int do_output() {
	double *data_out;

	mxArray *plhs;

	//plhs[0] (double *)mxMalloc(1 * numberOfScanPoints, mxReal);




	return 0;
}


void scan_info() {
	g_message("Scan count, %ld", current_range_count);

	return;
}


