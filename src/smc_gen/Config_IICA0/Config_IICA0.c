/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2021, 2024 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name        : Config_IICA0.c
* Component Version: 1.7.0
* Device(s)        : R7F100GGNxFB
* Description      : This file implements device driver for Config_IICA0.
***********************************************************************************************************************/
/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include "Config_IICA0.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
volatile uint8_t g_iica0_master_status_flag;      /* iica0 master flag */
volatile uint8_t * gp_iica0_rx_address;           /* iica0 receive buffer address */
uint16_t g_iica0_rx_len;                          /* iica0 receive data length */
volatile uint16_t g_iica0_rx_cnt;                 /* iica0 receive data count */
volatile uint8_t * gp_iica0_tx_address;           /* iica0 send buffer address */
volatile uint16_t g_iica0_tx_cnt;                 /* iica0 send data count */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_IICA0_Create
* Description  : This function initializes the IICA0 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_IICA0_Create(void)
{
    IICA0EN = 1U;    /* enables input clock supply */
    IICE0 = 0U;
    IICAMK0 = 1U;    /* disable INTIICA0 interrupt */
    IICAIF0 = 0U;    /* clear INTIICA0 interrupt flag */
    /* Set INTIICA0 level 1 priority */
    IICAPR10 = 0U;
    IICAPR00 = 1U;
    /* Set SCLA0, SDAA0 pin */
    PMCE6 &= 0xFCU;
    CCDE &= 0xCFU;
    P6 &= 0xFCU;
    PM6 |= 0x03U;
    SMC0 = 1U;
    IICWL0 = _15_IICA0_IICWL_VALUE;
    IICWH0 = _14_IICA0_IICWH_VALUE;
    DFC0 = 0U;    /* digital filter off */
    IICCTL01 |= _01_IICA_fCLK_HALF;
    SVA0 = _10_IICA0_MASTERADDRESS;
    STCEN0 = 1U;
    IICRSV0 = 1U;
    SPIE0 = 0U;
    WTIM0 = 1U;
    ACKE0 = 1U;
    IICAMK0 = 0U;
    IICE0 = 1U;
    LREL0 = 1U;
    /* Set SCLA0, SDAA0 pin */
    PM6 &= 0xFCU;

    R_Config_IICA0_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_IICA0_Stop
* Description  : This function stops IICA0 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_IICA0_Stop(void)
{
    IICE0 = 0U;    /* disable IICA0 operation */
}

/***********************************************************************************************************************
* Function Name: R_Config_IICA0_StopCondition
* Description  : This function stops the IICA0 condition.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_IICA0_StopCondition(void)
{
    SPT0 = 1U;    /* set stop condition flag */
}

/***********************************************************************************************************************
* Function Name: R_Config_IICA0_Master_Send
* Description  : This function starts to send data as master mode.
* Arguments    : adr -
*                    transfer address
*                tx_buf -
*                    transfer buffer pointer
*                tx_num -
*                    buffer size
*                wait -
*                    wait for start condition
* Return Value : status -
*                    MD_OK, MD_ERROR1 or MD_ERROR2
***********************************************************************************************************************/
MD_STATUS R_Config_IICA0_Master_Send(uint8_t adr, uint8_t * const tx_buf, uint16_t tx_num, uint8_t wait)
{
    MD_STATUS status = MD_OK;

    STT0 = 1U;    /* send IICA0 start condition */
    IICAMK0 = 0U;    /* enable INTIICA0 interrupt */

    /* Wait */
    while (0U == STD0)
    {
        if (0U == (wait--))
        {
            status = MD_ERROR2;
            break;
        }
    }

    /* Detect start condition */
    if (MD_OK == status)
    {
        g_iica0_tx_cnt = tx_num;
        gp_iica0_tx_address = tx_buf;
        g_iica0_master_status_flag = _00_IICA_MASTER_FLAG_CLEAR;
        adr &= (uint8_t)~0x01U;    /* set send mode */
        IICA0 = adr;    /* send address */
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_Config_IICA0_Master_Receive
* Description  : This function starts to receive data as master mode.
* Arguments    : adr -
*                    receive address
*                rx_buf -
*                    receive buffer pointer
*                rx_num -
*                    buffer size
*                wait -
*                    wait for start condition
* Return Value : status -
*                    MD_OK, MD_ERROR1 or MD_ERROR2
***********************************************************************************************************************/
MD_STATUS R_Config_IICA0_Master_Receive(uint8_t adr, uint8_t * const rx_buf, uint16_t rx_num, uint8_t wait)
{
    MD_STATUS status = MD_OK;

    STT0 = 1U;    /* send IICA0 start condition */
    IICAMK0 = 0U;    /* enable INTIICA0 interrupt */

    /* Wait */
    while (0U == STD0)
    {
        if (0U == (wait--))
        {
            status = MD_ERROR2;
            break;
        }
    }

    if (MD_OK == status)
    {
        /* Set parameter */
        g_iica0_rx_len = rx_num;
        g_iica0_rx_cnt = 0U;
        gp_iica0_rx_address = rx_buf;
        g_iica0_master_status_flag = _00_IICA_MASTER_FLAG_CLEAR;
        adr |= 0x01U;    /* set receive mode */
        IICA0 = adr;    /* receive address */
    }

    return (status);
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
