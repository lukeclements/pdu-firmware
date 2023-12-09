#ifndef _PDU_PACKET_H    /* Guard against multiple inclusion */
#define _PDU_PACKET_H

#include "string.h"
#include "stdint.h"

#define PDU_CMD_OFFSET 48

enum PDU_Type
{
    NOP,
    CommandPing,
    CommandSetSwitch,
    CommandGetSwitchStatus,
    CommandSetTRQ,
    CommandGetTRQTelem,
    DataPong,
    DataSwitchStatus,
    DataSwitchTelem,
    DataTRQTelem,
};
typedef uint8_t PDU_Type;

enum PDU_SW
{
    None,
    All,
    SW_3V3_1,
    RFM23_RADIO,
    SW_5V_1,
    HEATER,
    SW_5V_3,
    SW_12V,
    VBATT,
    BURN1,
    BURN2,
};
typedef uint8_t PDU_SW;

enum TRQ_SELECT
{
    TRQ1,
    TRQ2,
    TRQ1A,
    TRQ1B,
    TRQ2A,
    TRQ2B,
};
typedef uint8_t TRQ_SELECT;

enum TRQ_CONFIG
{
    SLEEP,
    MOTOR_COAST_FAST_DECAY,
    DIR_REVERSE,
    DIR_FORWARD,
    MOTOR_BREAK_SLOW_DECAY,
};
typedef uint8_t TRQ_CONFIG;

struct __attribute__((packed)) pdu_nop_packet
{
    PDU_Type type;
};

struct __attribute__((packed)) pdu_sw_packet
{
    PDU_Type type;
    PDU_SW sw;
    uint8_t sw_state;
};

struct __attribute__ ((packed)) pdu_sw_telem
{
    PDU_Type type;
    uint8_t sw_state[10];
};

struct __attribute__((packed)) pdu_hbridge_packet
{
    PDU_Type type;
    TRQ_SELECT select;
    TRQ_CONFIG config;
};

struct __attribute__((packed)) pdu_hbridge_telem
{
    PDU_Type type;
    TRQ_CONFIG trq_state[4];
};

void decode_pdu_packet(const char *input);
void handlePing();
void handleSetSwitch(PDU_SW sw, uint8_t sw_state);
void handleGetSwitchStatus(PDU_SW sw);
void handleSetTRQ(TRQ_SELECT select, TRQ_CONFIG config);
void handleGetTRQTelem();
void transmit(char *buf, int len);

#endif /* _PDU_PACKET_H */

/* *****************************************************************************
 End of File
 */