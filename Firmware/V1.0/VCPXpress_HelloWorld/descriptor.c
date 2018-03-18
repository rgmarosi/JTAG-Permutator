/**************************************************************************//**
 * @file
 * @brief   Hardware initialization functions.
 * @author  Silicon Laboratories
 * @version 1.0.0 (DM: July 14, 2014)
 *
 *******************************************************************************
 * @section License
 * (C) Copyright 2014 Silicon Labs Inc,
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *******************************************************************************
 *
 *
 * Descriptor information to pass to USB_Init()
 *
 *****************************************************************************/

#include "descriptor.h"

/*** [BEGIN] USB Descriptor Information [BEGIN] ***/

#define STRING1_LEN sizeof ("Robert") * 2
const uint8_t code USB_MfrStr[] = // Manufacturer String
{
   STRING1_LEN, 0x03,
   'R', 0,
   'o', 0,
   'b', 0,
   'e', 0,
   'r', 0,
   't', 0
};

#define STRING2_LEN sizeof("MyProductString") * 2
const uint8_t code USB_ProductStr[] = // Product Desc. String
{
   STRING2_LEN, 0x03,
   'M', 0,
   'y', 0,
   'P', 0,
   'r', 0,
   'o', 0,
   'd', 0,
   'u', 0,
   'c', 0,
   't', 0,
   'S', 0,
   't', 0,
   'r', 0,
   'i', 0,
   'n', 0,
   'g', 0
};

#define STRING3_LEN sizeof("0000") * 2
const uint8_t code USB_SerialStr[] = // Serial Number String
{
   STRING3_LEN, 0x03,
   '0', 0,
   '0', 0,
   '0', 0,
   '0', 0
};


const VCPXpress_Init_TypeDef InitStruct =
{
   0x10C4,                 // Vendor ID
   0xEA60,                 // Product ID
   USB_MfrStr,             // Pointer to Manufacturer String
   USB_ProductStr,         // Pointer to Product String
   USB_SerialStr,          // Pointer to Serial String
   32,                     // Max Power / 2
   0x80,                   // Power Attribute
   0x0100,                 // Device Release # (BCD format)
   false                    // Use USB FIFO space true
};
