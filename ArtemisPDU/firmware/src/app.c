/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "definitions.h"
#include "pdu_packet.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

#define APP_DATA_LEN         512
    
char APP_INIT_MSG[] = "\r\nAPP INITIALIZED\r\n";

// USART Definitions
// *****************************************************************************
#define RX_BUFFER_SIZE 256
char newline[] = "\r\n";
char errorMessage[] = "\r\n**** USART error has occurred ****\r\n";
char receiveBuffer[RX_BUFFER_SIZE] = {};
int data = 0;
uint16_t rxCounter = 0;
// *****************************************************************************

void USART_READ(void);

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
//    SERCOM3_USART_Write(&APP_INIT_MSG[0], sizeof(APP_INIT_MSG));
    disableAllGPIOs();
    RTC_Initialize();
    RTC_Timer32Start();
    
    WDT_TimeoutPeriodSet(WDT_CONFIG_PER_CYC16384_Val);
    WDT_Enable();
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    USART_READ();
}

void USART_READ(void) {    
    /* Check if there is a received character */
    if(SERCOM3_USART_ReceiverIsReady() == true)
    {
        if(SERCOM3_USART_ErrorGet() == USART_ERROR_NONE)
        {
            SERCOM3_USART_Read(&data, 1);

            if(data == '\r' || data == '\n')
            {
                SERCOM3_USART_Write("\0", 1);
                rxCounter = 0;
                decode_pdu_packet(receiveBuffer);
            }
            else
            {
                receiveBuffer[rxCounter++] = data;
            }
        }
    }
}

void enableAllGPIOs(void) {
    BURN_5V_Set();
    BURN1_EN_Set();
    BURN2_EN_Set();
    SW_12V_EN1_Set();
    SW_3V3_EN1_Set();
    SW_3V3_EN2_Set();
    SW_5V_EN1_Set();
    SW_5V_EN2_Set();
    SW_5V_EN3_Set();
    SW_5V_EN4_Set();
    SW_VBATT_EN_Set();
//    WDT_WDI_Set();
    IN1_Set();
    IN2_Set();
    IN3_Set();
    IN4_Set();
    IN5_Set();
    IN6_Set();
    IN7_Set();
    IN8_Set();
    TRQ1_Set();
    TRQ2_Set();
    FAULT1_Set();
    FAULT2_Set();
    SLEEP1_Set();
    SLEEP2_Set();
}

void disableAllGPIOs(void) {
    BURN_5V_Clear();
    BURN1_EN_Clear();
    BURN2_EN_Clear();
    SW_12V_EN1_Clear();
    SW_3V3_EN2_Clear();
    SW_3V3_EN1_Clear();
    SW_5V_EN2_Clear();
    SW_5V_EN1_Clear();
    SW_5V_EN3_Clear();
    SW_5V_EN4_Clear();
    SW_VBATT_EN_Clear();
//    WDT_WDI_Clear();
    IN1_Clear();
    IN2_Clear();
    IN3_Clear();
    IN4_Clear();
    IN5_Clear();
    IN6_Clear();
    IN7_Clear();
    IN8_Clear();
    TRQ1_Clear();
    TRQ2_Clear();
    FAULT1_Clear();
    FAULT2_Clear();
    SLEEP1_Clear();
    SLEEP2_Clear();
}

/*******************************************************************************
 End of File
 */