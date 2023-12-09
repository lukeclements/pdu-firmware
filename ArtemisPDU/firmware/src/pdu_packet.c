#include "pdu_packet.h"
#include "definitions.h"

void decode_pdu_packet(const char *input)
{
    PDU_Type packetType = input[0] - PDU_CMD_OFFSET;
    
    switch (packetType)
    {
        case CommandPing:
        {
            handlePing();
            WDT_Clear();
            break;
        }
        case CommandSetSwitch:
        {
            handleSetSwitch(input[1] - PDU_CMD_OFFSET, input[2] - PDU_CMD_OFFSET);
            WDT_Clear();
            break;
        }
        case CommandGetSwitchStatus:
        {
            handleGetSwitchStatus(input[1] - PDU_CMD_OFFSET);
            WDT_Clear();
            break;
        }
        case CommandSetTRQ:
        {
            handleSetTRQ(input[1] - PDU_CMD_OFFSET, input[2] - PDU_CMD_OFFSET);
            WDT_Clear();
            break;
        }
        case CommandGetTRQTelem:
        {
            handleGetTRQTelem();
            WDT_Clear();
            break;
        }
        default:
        {
            break;
        }
    };
}

void handlePing()
{
    struct pdu_nop_packet packet;
    packet.type = DataPong;
    
    char reply[sizeof(struct pdu_nop_packet)];
    memcpy(reply, &packet, sizeof(struct pdu_nop_packet));
    transmit(reply, sizeof(struct pdu_nop_packet));
}

void handleSetSwitch(PDU_SW sw, uint8_t sw_state)
{
    if (sw_state == 1)
    {
        switch (sw)
        {
            case All:
                enableAllGPIOs();
                break;
            case SW_3V3_1:
                SW_3V3_EN1_Set();
                break;
            case RFM23_RADIO:
                SW_3V3_EN2_Set();
                break;
            case SW_5V_1:
                SW_5V_EN1_Set();
                break;
            case HEATER:
                SW_5V_EN2_Set();
                break;
            case SW_5V_3:
                SW_5V_EN3_Set();
                break;
            case SW_12V:
                SW_12V_EN1_Set();
                SW_5V_EN4_Set();
                break;
            case VBATT:
                SW_VBATT_EN_Set();
                break;
            case BURN1:
                BURN1_EN_Set();
                BURN_5V_Set();
                break;
            case BURN2:
                BURN2_EN_Set();
                BURN_5V_Set();
                break;
            default:
                break;
        }
    } else {
        switch (sw)
        {
            case All:
                disableAllGPIOs();
                break;
            case SW_3V3_1:
                SW_3V3_EN1_Clear();
                break;
            case RFM23_RADIO:
                SW_3V3_EN2_Clear();
                break;
            case SW_5V_1:
                SW_5V_EN1_Clear();
                break;
            case HEATER:
                SW_5V_EN2_Clear();
                break;
            case SW_5V_3:
                SW_5V_EN3_Clear();
                break;
            case SW_12V:
                SW_12V_EN1_Clear();
                SW_5V_EN4_Clear();
                break;
            case VBATT:
                SW_VBATT_EN_Clear();
                break;
            case BURN1:
                if(!PORT_PinRead(BURN2_EN_PIN))
                    BURN_5V_Clear();
                BURN1_EN_Clear();
                break;
            case BURN2:
                if(!PORT_PinRead(BURN1_EN_PIN))
                    BURN_5V_Clear();
                BURN2_EN_Clear();
                break;
            default:
                break;
        }
    }
    handleGetSwitchStatus(sw);
}

void handleGetSwitchStatus(PDU_SW sw)
{
    if (sw == All)
    {
        struct pdu_sw_telem telem;
        telem.type = DataSwitchTelem;
        telem.sw_state[0] = PORT_PinRead(SW_3V3_EN1_PIN);
        telem.sw_state[1] = PORT_PinRead(SW_3V3_EN2_PIN);
        telem.sw_state[2] = PORT_PinRead(SW_5V_EN1_PIN);
        telem.sw_state[3] = PORT_PinRead(SW_5V_EN2_PIN);
        telem.sw_state[4] = PORT_PinRead(SW_5V_EN3_PIN);
        telem.sw_state[5] = PORT_PinRead(SW_5V_EN4_PIN);
        telem.sw_state[6] = PORT_PinRead(SW_12V_EN1_PIN) && PORT_PinRead(SW_5V_EN4_PIN);
        telem.sw_state[7] = PORT_PinRead(SW_VBATT_EN_PIN);
        telem.sw_state[8] = PORT_PinRead(BURN1_EN_PIN) && PORT_PinRead(BURN_5V_PIN);
        telem.sw_state[9] = PORT_PinRead(BURN2_EN_PIN) && PORT_PinRead(BURN_5V_PIN);
        
        char reply[sizeof(struct pdu_sw_telem)];
        memcpy(reply, &telem, sizeof(struct pdu_sw_telem));
        transmit(reply, sizeof(struct pdu_sw_telem));
        return;
    }
    
    struct pdu_sw_packet packet;
    packet.type = DataSwitchStatus;
    packet.sw = sw;
    switch (sw)
    {
        case SW_3V3_1:
            packet.sw_state = PORT_PinRead(SW_3V3_EN1_PIN);
            break;
        case RFM23_RADIO:
            packet.sw_state = PORT_PinRead(SW_3V3_EN2_PIN);
            break;
        case SW_5V_1:
            packet.sw_state = PORT_PinRead(SW_5V_EN1_PIN);
            break;
        case HEATER:
            packet.sw_state = PORT_PinRead(SW_5V_EN2_PIN);
            break;
        case SW_5V_3:
            packet.sw_state = PORT_PinRead(SW_5V_EN3_PIN);
            break;
        case SW_12V:
            packet.sw_state = PORT_PinRead(SW_12V_EN1_PIN) && PORT_PinRead(SW_5V_EN4_PIN);
            break;
        case VBATT:
            packet.sw_state = PORT_PinRead(SW_VBATT_EN_PIN);
            break;
        case BURN1:
            packet.sw_state = PORT_PinRead(BURN1_EN_PIN) && PORT_PinRead(BURN_5V_PIN);
            break;
        case BURN2:
            packet.sw_state = PORT_PinRead(BURN2_EN_PIN) && PORT_PinRead(BURN_5V_PIN);
            break;
        default:
            break;
    }
    char reply[sizeof(struct pdu_sw_packet)];
    memcpy(reply, &packet, sizeof(struct pdu_sw_packet));
    transmit(reply, sizeof(struct pdu_sw_packet));
}

void handleSetTRQ(TRQ_SELECT select, TRQ_CONFIG config)
{  
    switch (select)
    {
        case TRQ1:
        {
            if (config == SLEEP)
                SLEEP1_Clear();
            break;
        }
        case TRQ2:
        {
            if (config == SLEEP)
                SLEEP2_Clear();
            break;
        }
        /* Solar Panel 1 Torque Coil 1  */
        case TRQ1A:
        {
            if (config == SLEEP)
            {
                return;
            }
            
            SLEEP1_Set();
            switch (config)
            {
                case MOTOR_COAST_FAST_DECAY:
                {
                    IN1_Clear();
                    IN2_Clear();
                    break;
                }
                case DIR_REVERSE:
                {
                    IN1_Clear();
                    IN2_Set();
                    break;
                }
                case DIR_FORWARD:
                {
                    IN1_Set();
                    IN2_Clear();
                    break;
                }
                case MOTOR_BREAK_SLOW_DECAY:
                {
                    IN1_Set();
                    IN2_Set();
                    break;
                }
            }
            break;
        }
        /* Solar Panel 1 Torque Coil 2  */
        case TRQ1B:
        {
            if (config == SLEEP)
            {
                return;
            }
            
            SLEEP1_Set();
            switch (config)
            {
                case MOTOR_COAST_FAST_DECAY:
                {
                    IN3_Clear();
                    IN4_Clear();
                    break;
                }
                case DIR_REVERSE:
                {
                    IN3_Clear();
                    IN4_Set();
                    break;
                }
                case DIR_FORWARD:
                {
                    IN3_Set();
                    IN4_Clear();
                    break;
                }
                case MOTOR_BREAK_SLOW_DECAY:
                {
                    IN3_Set();
                    IN4_Set();
                    break;
                }
            }
            break;
        }
        /* Solar Panel 2 Torque Coil 1  */
        case TRQ2A:
        {
            if (config == SLEEP)
            {
                return;
            }
            
            SLEEP2_Set();
            switch (config)
            {
                case MOTOR_COAST_FAST_DECAY:
                {
                    IN5_Clear();
                    IN6_Clear();
                    break;
                }
                case DIR_REVERSE:
                {
                    IN5_Clear();
                    IN6_Set();
                    break;
                }
                case DIR_FORWARD:
                {
                    IN5_Set();
                    IN6_Clear();
                    break;
                }
                case MOTOR_BREAK_SLOW_DECAY:
                {
                    IN5_Set();
                    IN6_Set();
                    break;
                }
            }
            break;
        }
        /* Solar Panel 2 Torque Coil 2  */
        case TRQ2B:
        {
            if (config == SLEEP)
            {
                return;
            }
            
            SLEEP2_Set();
            switch (config)
            {
                case MOTOR_COAST_FAST_DECAY:
                {
                    IN7_Clear();
                    IN8_Clear();
                    break;
                }
                case DIR_REVERSE:
                {
                    IN7_Clear();
                    IN8_Set();
                    break;
                }
                case DIR_FORWARD:
                {
                    IN7_Set();
                    IN8_Clear();
                    break;
                }
                case MOTOR_BREAK_SLOW_DECAY:
                {
                    IN7_Set();
                    IN8_Set();
                    break;
                }
            }
            break;
        }
    }
    handleGetTRQTelem();
}

void handleGetTRQTelem()
{
    struct pdu_hbridge_telem packet;
    packet.type = DataTRQTelem;
    
    int configVals[2];
    
    configVals[0] = PORT_PinRead(IN1_PIN);
    configVals[1] = PORT_PinRead(IN2_PIN);
    if (!PORT_PinRead(SLEEP1_PIN))
    {
        packet.trq_state[0] = SLEEP;
    }
    else if (configVals[0] == 0 && configVals[1] == 0)
    {
        packet.trq_state[0] = MOTOR_COAST_FAST_DECAY;
    }
    else if (configVals[0] == 0 && configVals[1] == 1)
    {
        packet.trq_state[0] = DIR_REVERSE;
    }
    else if (configVals[0] == 1 && configVals[1] == 0)
    {
        packet.trq_state[0] = DIR_FORWARD;
    }
    else if (configVals[0] == 1 && configVals[1] == 1)
    {
        packet.trq_state[0] = MOTOR_BREAK_SLOW_DECAY;
    }
    
    configVals[0] = PORT_PinRead(IN3_PIN);
    configVals[1] = PORT_PinRead(IN4_PIN);
    if (!PORT_PinRead(SLEEP1_PIN))
    {
        packet.trq_state[1] = SLEEP;
    }
    else if (configVals[0] == 0 && configVals[1] == 0)
    {
        packet.trq_state[1] = MOTOR_COAST_FAST_DECAY;
    }
    else if (configVals[0] == 0 && configVals[1] == 1)
    {
        packet.trq_state[1] = DIR_REVERSE;
    }
    else if (configVals[0] == 1 && configVals[1] == 0)
    {
        packet.trq_state[1] = DIR_FORWARD;
    }
    else if (configVals[0] == 1 && configVals[1] == 1)
    {
        packet.trq_state[1] = MOTOR_BREAK_SLOW_DECAY;
    }
    
    configVals[0] = PORT_PinRead(IN5_PIN);
    configVals[1] = PORT_PinRead(IN6_PIN);
    if (!PORT_PinRead(SLEEP2_PIN))
    {
        packet.trq_state[2] = SLEEP;
    }
    else if (configVals[0] == 0 && configVals[1] == 0)
    {
        packet.trq_state[2] = MOTOR_COAST_FAST_DECAY;
    }
    else if (configVals[0] == 0 && configVals[1] == 1)
    {
        packet.trq_state[2] = DIR_REVERSE;
    }
    else if (configVals[0] == 1 && configVals[1] == 0)
    {
        packet.trq_state[2] = DIR_FORWARD;
    }
    else if (configVals[0] == 1 && configVals[1] == 1)
    {
        packet.trq_state[2] = MOTOR_BREAK_SLOW_DECAY;
    }
    
    configVals[0] = PORT_PinRead(IN7_PIN);
    configVals[1] = PORT_PinRead(IN8_PIN);
    if (!PORT_PinRead(SLEEP2_PIN))
    {
        packet.trq_state[3] = SLEEP;
    }
    else if (configVals[0] == 0 && configVals[1] == 0)
    {
        packet.trq_state[3] = MOTOR_COAST_FAST_DECAY;
    }
    else if (configVals[0] == 0 && configVals[1] == 1)
    {
        packet.trq_state[3] = DIR_REVERSE;
    }
    else if (configVals[0] == 1 && configVals[1] == 0)
    {
        packet.trq_state[3] = DIR_FORWARD;
    }
    else if (configVals[0] == 1 && configVals[1] == 1)
    {
        packet.trq_state[3] = MOTOR_BREAK_SLOW_DECAY;
    }
    
    char reply[sizeof(struct pdu_hbridge_telem)];
    memcpy(reply, &packet, sizeof(struct pdu_hbridge_telem));
    transmit(reply, sizeof(struct pdu_hbridge_telem));
}

void transmit(char *buf, int len)
{
    for(int i = 0; i < len; i++) {
        buf[i] += PDU_CMD_OFFSET;
    }
    
    SERCOM3_USART_Write(&buf[0], len);
    SERCOM3_USART_Write("\r\n", 2);
}
