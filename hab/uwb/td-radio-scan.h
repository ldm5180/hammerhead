
/*
 * The majority of this file was snipped from matrix.h.
 */

#define mxMAXNAM	32

typedef struct
{
	float	rangeInFeet;
	float	coreBoardTemperature;
	int		scanStatus;
	int		responderID;// this lists the one for this scan being passed up
	long	scanNumber;
	int		maxScanPointsSegment;
	long 	numScanPointsSegment;
	long	numScanPointsTotal;
	int		segmentNumber;
	int		segmentsTotal;
	int		scanRateTx;
	int		scanRateRx;
	int		scanRateFilter;
	int		le;
	int		dataRate; // added this recently
	short signed int scanData[COMPLETE_SCAN_SIZE];
	//long	scanData[WHOLE_RADIO_SCAN_SIZE];
	//
} tdScan_t;

typedef struct mxArray_tag mxArray;

struct mxArray_tag {
	char 	name[mxMAXNAM];
	int 	reserved1[2];
	void	*reserved2;
	int		number_of_dims;
	int		nelements_allocated;
	int		reserved3[3];
	union {
		struct {
			void *pdata;
			void *pimag_data;
			void *reserved4;
			int	reserved5[3];
		} number_array;
	} data;
};
