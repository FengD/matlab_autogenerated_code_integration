/* $Revision: 1.1.6.13 $ */
/**
* @file:       can_message.h
*
* Purpose:     Declare custom and register CAN datatype.
*
* $Authors:    Shankar Subramanian $
*
* $Copyright: 2008 The MathWorks, Inc.$
*
* $Date: 2011/05/13 18:05:25 $
*/
#ifndef __CANMESSAGE__HEADER__
#define __CANMESSAGE__HEADER__

//#include "Std_Types.h"
typedef unsigned char         boolean; /*                 TRUE .. FALSE                */
typedef signed char           sint8;   /*                 -128 .. +127                 */
typedef unsigned char         uint8;   /*                    0 .. 255                  */
typedef signed short          sint16;  /*               -32768 .. +32767               */
typedef unsigned short        uint16;  /*                    0 .. 65535                */
typedef signed int            sint32;  /*          -2147483648 .. +2147483647          */
typedef unsigned int          uint32;  /*                    0 .. 4294967295           */
typedef signed long long      sint64;  /* -9223372036854775808 .. 9223372036854775807  */
typedef unsigned long long    uint64;  /*                    0 .. 18446744073709551615 */

typedef signed int      sint8_least;  /* At least 7 bit + 1 bit sign    */
typedef unsigned int    uint8_least;  /* At least 8 bit                 */
typedef signed int      sint16_least; /* At least 15 bit + 1 bit sign   */
typedef unsigned int    uint16_least; /* At least 16 bit                */
typedef signed int      sint32_least; /* At least 31 bit + 1 bit sign   */
typedef unsigned int    uint32_least; /* At least 32 bit                */

typedef float  float32;
typedef double float64;

typedef unsigned char StatusType; /* OSEK compliance */
typedef uint8 Std_ReturnType;



/* @DO NOT REMOVE: The following typedef is required for targets (FM5, C166 etc.) to actually
use it for their code generation TLC file. Since their datatype is being
discarded, this is being defined in our header which they will be referencing.*/
typedef enum {CAN_MESSAGE_STANDARD, CAN_MESSAGE_EXTENDED} CanFrameType;

/* Define invalid CAN Identifier value. This can be used to specify invalid CAN Message
   This represents a uint32_T value */
#define INVALID_CAN_ID 0xFFFFFFFF

/*
The CAN_DATATYPE structure has been structured so that it is tightly packed.
Size of structure (without timestamp field) :16 bytes
Size of structure (with timestamp field)    :24 bytes
*/
typedef struct {

    /* Is Extended frame */
    uint8 Extended;

    /* Length */
    uint8 Length;

    /* RTR */
    uint8 Remote;

    /* Error */
    uint8 Error;

    /* CAN ID */
    uint32 ID;

    /*
    TIMESTAMP_NOT_REQUIRED is a macro that will be defined by Target teams
    PIL, C166, FM5, xPC if they do not require the timestamp field during code
    generation. By default, timestamp is defined. If the targets do not require
    the timestamp field, they should define the macro TIMESTAMP_NOT_REQUIRED before
    including this header file for code generation.
    */
    #ifndef TIMESTAMP_NOT_REQUIRED
        /* Timestamp */
        double Timestamp;
    #endif

    /* Data field */
    uint8 Data[8];

}  CAN_MESSAGE;

typedef CAN_MESSAGE CAN_DATATYPE;


#endif
