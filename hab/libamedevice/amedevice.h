
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef AMEDEVICE_H
#define AMEDEVICE_H


#include <bluetooth/bluetooth.h>
#include <inttypes.h>


#define PROFILE_NAME "ZV-SPP"

#define AME_CHANNEL_NUMBER 1

#define DEVICE_DELAY 23


//  Command Codes

#define CMD_RETURN_STATUS                  0xE0
#define CMD_LOAD_CONFIGURATION             0xE1
#define CMD_RETURN_CONFIGURATION           0xE2
#define CMD_RETURN_DATA                    0xE3        // EnableStreaming, EnableSampling
#define CMD_RETURN_ERROR_INFO              0xE4
#define CMD_RETURN_FIRMWARE_RELEASE        0xE5
#define CMD_LOAD_BLUETOOTH_ADDRESS         0xE6
#define CMD_RETURN_BLUETOOTH_ADDRESS       0xE7
#define CMD_RESET_ERROR_INFO               0xE8
#define CMD_LOAD_NAME                      0xE9
#define CMD_RETURN_NAME                    0xEA
#define CMD_NEW_DATA_AVAILABLE             0xEB
#define CMD_ERASE                          0xEC        // StopStreaming, StopSampling
#define CMD_RETURN_DEVICE_TYPE             0xF1
#define CMD_CLOSE_CONNECTION               0xF3
#define CMD_READ_BATTERY_STATUS            0xF5
#define CMD_ENABLE_SNIFF_MODE              0xF6
#define CMD_KEEP_ALIVE                     0xF9


#define CMD_ENABLE_SAMPLING                 CMD_RETURN_DATA
#define CMD_ENABLE_STREAMING                CMD_RETURN_DATA
#define CMD_STOP_SAMPLING                   CMD_ERASE
#define CMD_STOP_STREAMING                  CMD_ERASE

#define CMD_SIZE                            8
#define SUM8_MASK                           0xDF  // ??


//  Response Codes

#define RSP_HEADER                         0xFF

#define RSP_STATUS_WAITING                 0x00
#define RSP_STATUS_SUCCESS                 0x01
#define RSP_STATUS_MODE_ERROR              0x02
#define RSP_STATUS_STREAMING_ENABLED       0x03
#define RSP_STATUS_SAMPLING_ENABLED        0x04
#define RSP_STATUS_COMMAND_ERROR           0x07
#define RSP_STATUS_GOT_COMMAND             0x08

//  Adaptor Codes

#define ADAPTER_MODE_BOOT                  0x00
#define ADAPTER_MODE_SETUP                 0x01
#define ADAPTER_MODE_NORMAL                0x02
#define ADAPTER_MODE_MONITOR               0x04
#define ADAPTER_MODE_FAILURE               0x08


//  Enable Register bit definitions

#define ENABLE_REG_BROADCAST_ENABLED       0x01
#define ENABLE_REG_AUTO_NOTIFY_ENABLED     0x02
#define ENABLE_REG_SECURITY_ENABLED        0x04
#define ENABLE_REG_TEST_MODE_ENABLED       0x08


//  Status Register bits definitions

#define STATUS_REG_NO_DATA_AVAILABLE       0x00
#define STATUS_REG_NEW_DATA_AVAILABLE      0x01
#define STATUS_REG_MEASUREMENT_IN_PROG     0x02


// Sample Rate Codes

#define SAMPLE_RATE_RESYNCH                0
#define SAMPLE_RATE_100                    100
#define SAMPLE_RATE_200                    200
#define SAMPLE_RATE_300                    300
#define SAMPLE_RATE_400                    400
#define SAMPLE_RATE_500                    500
#define SAMPLE_RATE_600                    600
#define SAMPLE_RATE_700                    700
#define SAMPLE_RATE_800                    800
#define SAMPLE_RATE_900                    900
#define SAMPLE_RATE_1000                   1000



// Device Parameter Codes

#define SIZEOF_CACHED_PARAMETERS            100
#define PARAM_ALL                           0    // SIZE = 99
#define PARAM_COMMAND_ID                    1    // SIZE = 1
#define PARAM_ERROR_BITS                    2    // SIZE = 1
#define PARAM_COMMAND_STATUS                3    // SIZE = 1
#define PARAM_ADAPTOR_MODE                  4    // SIZE = 1
#define PARAM_STATUS_PARAMETERS             5    // SIZE = 4
#define PARAM_CONFIGURATION                 6    // SIZE = 5
#define PARAM_ERROR_INFO                    7    // SIZE = 5
#define PARAM_FIRMWARE_RELEASE              8    // SIZE = 2
#define PARAM_HOST_BT_ADDR                  9    // SIZE = 6
#define PARAM_PIN_CODE                     10    // SIZE = 16
#define PARAM_LOCAL_BT_NAME                11    // SIZE = 20
#define PARAM_DEVICE_ID                    12    // SIZE = 1
#define PARAM_LOCAL_BT_ADDR                13    // SIZE = 6
#define PARAM_OPTIONAL_PARAM               14    // SIZE = 20
#define PARAM_BATTERY_STATUS               15    // SIZE = 8
#define PARAM_KEEP_ALIVE_TIMEOUT           16    // SIZE = 2


typedef struct
{
        uint8_t commandId;
        uint8_t errorBits;
        uint8_t commandStatus;
        uint8_t adaptorMode;
        uint8_t statusParameters[4];
        uint8_t configuration[5];
        uint8_t errorInfo[5];
        uint8_t firmwareRelease[2];
        uint8_t hostBtAddr[6];
        uint8_t pinCode[16];
        uint8_t localBtName[20];
        uint8_t deviceId;
        uint8_t localBtAddr[6];
        uint8_t optionalParam[20];
        uint8_t batteryStatus[8];
        uint8_t keepAliveTimeout[2];

} __attribute__ ((packed)) ame_device_pararmeter_t;


//  Checksum algorithm used by amd devices
uint8_t sum8(char* data, int size);

int ame_write_command(int fd, char* buffer, int size);

int ame_read_command(int fd, char* buffer, int size);

int ame_is_modem_command(char* buffer);

int ame_command_return_status(int fd);

int ame_command_load_config(int fd);

int ame_command_return_config(int fd);

int ame_command_return_data(int fd, uint16_t sample_rate, uint8_t gain);

int ame_command_return_error(int fd);

int ame_command_return_firmware_revision(int fd);

int ame_command_load_bdaddr(int fd, bdaddr_t* addr, char* pin);

int ame_command_return_bdaddr(int fd);

int ame_command_reset_error(int fd);

int ame_command_load_name(int fd);

int ame_command_return_name(int fd);

char* ame_response_return_name(int fd);

int ame_command_erase(int fd);

int ame_command_return_type(int fd);

int ame_command_close_connection(int fd);

int ame_command_return_battery_status(int fd);

float ame_parse_battery_voltage(char* buffer, int size);

int ame_command_keep_alive(int fd, uint16_t timeout);

#endif
