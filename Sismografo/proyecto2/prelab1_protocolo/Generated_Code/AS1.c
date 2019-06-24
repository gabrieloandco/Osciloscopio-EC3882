/* ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename    : AS1.c
**     Project     : prelab1_protocolo
**     Processor   : MC9S08QE128CLK
**     Component   : AsynchroSerial
**     Version     : Component 02.611, Driver 01.33, CPU db: 3.00.067
**     Compiler    : CodeWarrior HCS08 C Compiler
**     Date/Time   : 2019-06-03, 14:08, # CodeGen: 42
**     Abstract    :
**         This component "AsynchroSerial" implements an asynchronous serial
**         communication. The component supports different settings of
**         parity, word width, stop-bit and communication speed,
**         user can select interrupt or polling handler.
**         Communication speed can be changed also in runtime.
**         The component requires one on-chip asynchronous serial channel.
**     Settings    :
**         Serial channel              : SCI1
**
**         Protocol
**             Init baud rate          : 115200baud
**             Width                   : 8 bits
**             Stop bits               : 1
**             Parity                  : none
**             Breaks                  : Disabled
**             Input buffer size       : 0
**             Output buffer size      : 0
**
**         Registers
**             Input buffer            : SCI1D     [$0027]
**             Output buffer           : SCI1D     [$0027]
**             Control register        : SCI1C1    [$0022]
**             Mode register           : SCI1C2    [$0023]
**             Baud setting reg.       : SCI1BD    [$0020]
**             Special register        : SCI1S1    [$0024]
**
**
**
**         Used pins:
**         ----------------------------------------------------------
**           Function | On package           |    Name
**         ----------------------------------------------------------
**            Input   |     42               |  PTB0_KBI1P4_RxD1_ADP4
**            Output  |     41               |  PTB1_KBI1P5_TxD1_ADP5
**         ----------------------------------------------------------
**
**
**
**     Contents    :
**         RecvChar        - byte AS1_RecvChar(AS1_TComData *Chr);
**         SendChar        - byte AS1_SendChar(AS1_TComData Chr);
**         GetCharsInRxBuf - word AS1_GetCharsInRxBuf(void);
**         GetCharsInTxBuf - word AS1_GetCharsInTxBuf(void);
**
**     Copyright : 1997 - 2014 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file AS1.c
** @version 01.33
** @brief
**         This component "AsynchroSerial" implements an asynchronous serial
**         communication. The component supports different settings of
**         parity, word width, stop-bit and communication speed,
**         user can select interrupt or polling handler.
**         Communication speed can be changed also in runtime.
**         The component requires one on-chip asynchronous serial channel.
*/         
/*!
**  @addtogroup AS1_module AS1 module documentation
**  @{
*/         


/* MODULE AS1. */

#pragma MESSAGE DISABLE C4002 /* WARNING C4002: Result not used is ignored */

#include "AS1.h"

#pragma CODE_SEG AS1_CODE



/*
** ===================================================================
**     Method      :  AS1_RecvChar (component AsynchroSerial)
**     Description :
**         If any data is received, this method returns one character,
**         otherwise it returns an error code (it does not wait for
**         data). This method is enabled only if the receiver property
**         is enabled.
**         [Note:] Because the preferred method to handle error and
**         break exception in the interrupt mode is to use events
**         <OnError> and <OnBreak> the return value ERR_RXEMPTY has
**         higher priority than other error codes. As a consequence the
**         information about an exception in interrupt mode is returned
**         only if there is a valid character ready to be read.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Chr             - Pointer to a received character
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_RXEMPTY - No data in receiver
**                           ERR_BREAK - Break character is detected
**                           (only when the <Interrupt service> property
**                           is disabled and the <Break signal> property
**                           is enabled)
**                           ERR_COMMON - common error occurred (the
**                           <GetError> method can be used for error
**                           specification)
** ===================================================================
*/
byte AS1_RecvChar(AS1_TComData *Chr)
{
  byte Result = ERR_OK;                /* Prepare default error code */
  byte StatReg = SCI1S1;               /* Read status register */

  if (StatReg & (SCI1S1_OR_MASK|SCI1S1_NF_MASK|SCI1S1_FE_MASK|SCI1S1_PF_MASK)) { /* Is any error set? */
    Result = ERR_COMMON;               /* If yes then set common error value */
  } else if ((StatReg & SCI1S1_RDRF_MASK) == 0U) { /* Is the reciver empty and no error is set? */
    return ERR_RXEMPTY;                /* If yes then error */
  } else {                             /* Intentionally left empty due to compatibility with MISRA rule 60 */
  }
  *Chr = SCI1D;                        /* Read data from the receiver */
  return Result;                       /* Return error code */
}

/*
** ===================================================================
**     Method      :  AS1_SendChar (component AsynchroSerial)
**     Description :
**         Sends one character to the channel. If the component is
**         temporarily disabled (Disable method) SendChar method only
**         stores data into an output buffer. In case of a zero output
**         buffer size, only one character can be stored. Enabling the
**         component (Enable method) starts the transmission of the
**         stored data. This method is available only if the
**         transmitter property is enabled.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Chr             - Character to send
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_TXFULL - Transmitter is full
** ===================================================================
*/
byte AS1_SendChar(AS1_TComData Chr)
{
  if (SCI1S1_TDRE == 0U) {             /* Is the transmitter full? */
    return ERR_TXFULL;                 /* If yes then error */
  }
  SCI1D = (byte)Chr;                   /* Store char to the transmitter register */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  AS1_GetCharsInRxBuf (component AsynchroSerial)
**     Description :
**         Returns the number of characters in the input buffer. This
**         method is available only if the receiver property is enabled.
**     Parameters  : None
**     Returns     :
**         ---             - The number of characters in the input
**                           buffer.
** ===================================================================
*/
/*
word AS1_GetCharsInRxBuf(void)

**      This method is implemented as a macro. See header module. **
*/

/*
** ===================================================================
**     Method      :  AS1_GetCharsInTxBuf (component AsynchroSerial)
**     Description :
**         Returns the number of characters in the output buffer. This
**         method is available only if the transmitter property is
**         enabled.
**     Parameters  : None
**     Returns     :
**         ---             - The number of characters in the output
**                           buffer.
** ===================================================================
*/
/*
word AS1_GetCharsInTxBuf(void)

**      This method is implemented as a macro. See header module. **
*/

/*
** ===================================================================
**     Method      :  AS1_Init (component AsynchroSerial)
**
**     Description :
**         Initializes the associated peripheral(s) and the component's 
**         internal variables. The method is called automatically as a 
**         part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void AS1_Init(void)
{
  /* SCI1C1: LOOPS=0,SCISWAI=0,RSRC=0,M=0,WAKE=0,ILT=0,PE=0,PT=0 */
  setReg8(SCI1C1, 0x00U);              /* Configure the SCI */ 
  /* SCI1C3: R8=0,T8=0,TXDIR=0,TXINV=0,ORIE=0,NEIE=0,FEIE=0,PEIE=0 */
  setReg8(SCI1C3, 0x00U);              /* Disable error interrupts */ 
  /* SCI1C2: TIE=0,TCIE=0,RIE=0,ILIE=0,TE=0,RE=0,RWU=0,SBK=0 */
  setReg8(SCI1C2, 0x00U);              /* Disable all interrupts */ 
  /* SCI1S2: LBKDIF=0,RXEDGIF=0,??=0,RXINV=0,RWUID=0,BRK13=0,LBKDE=0,RAF=0 */
  setReg8(SCI1S2, 0x00U);               
  SCI1BDH = 0x00U;                     /* Set high divisor register (enable device) */
  SCI1BDL = 0x10U;                     /* Set low divisor register (enable device) */
  SCI1C2_TE = 0x01U;                    /* Enable transmitter */
  SCI1C2_RE = 0x01U;                    /* Enable receiver */
}



/* END AS1. */


/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale HCS08 series of microcontrollers.
**
** ###################################################################
*/
