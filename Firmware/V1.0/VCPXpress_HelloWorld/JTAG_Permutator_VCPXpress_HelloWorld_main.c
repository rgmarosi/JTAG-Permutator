/**************************************************************************//**
 * JTAG_Permutator_VCPXpress_HelloWorld_main.c
 *
 * Main routine for Hello World test program
 *
 * Periodically prints "Hello World!" to a serial console on a host PC
 * install Silicon Lab's CP210x drivers on host PC to talk to the MCU
 * installation probably unnecessary if Simplicity Studio is already installed
 *
 *****************************************************************************/

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <SI_EFM8UB3_Register_Enums.h>          // SFR declarations
#include "VCPXpress.h"							// for virtual COM port
#include "descriptor.h"							// includes stdint.h as well

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
SI_SBIT (LED1, SFR_P2, 1);                // LED='1' means ON

#define BLINK_RATE         2              // Timer0 Interrupts per second
#define SYSCLK             6000000        // SYSCLK frequency (48MHz/8)
#define PRESCALE           48             // Timer0 prescaler
#define TIMER_RELOAD -(SYSCLK / PRESCALE / BLINK_RATE) // this must be < 2^16

#define TIMER_RELOAD_HIGH ((TIMER_RELOAD & 0xFF00)>>8)
#define TIMER_RELOAD_LOW (TIMER_RELOAD & 0x00FF)

#define PACKET_SIZE 64					  // bytes per USB packet

// xdata memory type is stored in RAM of 8051
uint16_t xdata InCount;                   // Holds size of received packet
uint16_t xdata OutCount;                  // Holds size of transmitted packet
uint8_t xdata RX_Packet[PACKET_SIZE];     // Packet received from host
uint8_t xdata TX_Packet[PACKET_SIZE];     // Packet to transmit to host

uint8_t xdata TX_Flag = 0;				  // set in Timer0 ISR and cleared in main loop
uint8_t xdata Is_Open_Flag = 0;			  // set in USB callback function. checked in main loop


//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void Sysclk_Init(void);
static void Port_Init(void);
void Timer0_Init();
void myAPICallback(void);


//-----------------------------------------------------------------------------
// SiLabs_Startup() Routine
// ----------------------------------------------------------------------------
// This function is called immediately after reset, before the initialization
// code is run in SILABS_STARTUP.A51 (which runs before main() ). This is a
// useful place to disable the watchdog timer, which is enable by default
// and may trigger before main() in some instances.
//-----------------------------------------------------------------------------
void SiLabs_Startup (void)
{
  // Disable the watchdog here
	WDTCN = 0xDE;							// turn off watchdog timer by entering
	WDTCN = 0xAD;							// both keys within 4 clock cycles
}

//-----------------------------------------------------------------------------
// main() Routine
// ----------------------------------------------------------------------------
// Sets up MCU and periodically sends "Hello World!" over USB
// Timer 0 ISR handles blinking of LED and triggers MCU->host transmissions
// MCU->host and host->MCU transmissions managed by USB ISR
//-----------------------------------------------------------------------------
int main (void)
{
	uint8_t i = 0 , res = 0;
	const uint8_t msg[] = "Hello World!\r\n";
	uint8_t msgLen = sizeof(msg);


	Sysclk_Init ();                            // Initialize system clock
	Port_Init ();                              // Initialize crossbar and GPIO
	Timer0_Init ();                            // Initialize Timer2

	// VCPXpress Initialization
	USB_Init(&InitStruct);

	// Enable VCPXpress API interrupts
	// point user defined callback to USB interrupt
	API_Callback_Enable(myAPICallback);

	IE_EA = 1;       // Enable global interrupts

	//fill TX buffer with message
	for(i = 0; i < msgLen; i++)
	{
		TX_Packet[i] = msg[i];
	}

	while(1)//spin while waiting for interrupts
	{
		if(Is_Open_Flag && TX_Flag)//attempt block_write to host
		{
			res = Block_Write(TX_Packet, msgLen, &OutCount);     // Start USB Write
			if(res == 0)//clear TX flag if successful, otherwise try again
			{
				TX_Flag = 0;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Sysclk_Init() Routine
// ----------------------------------------------------------------------------
// configures system clock to 6MHz
//-----------------------------------------------------------------------------
void Sysclk_Init(void)
{
	SFRPAGE = 0x10;					   // go to page 0x10 to write to HFOCN
	HFOCN  = HFOCN_HFO1EN__ENABLED;      // Enable 48Mhz oscillator
	// need to set SYSCLK to > 24 MHz before switching to HFOSC1

	// set clock to divide-by-1 until its ready
	// because it takes time for clock to propagate through divider
	CLKSEL = CLKSEL_CLKDIV__SYSCLK_DIV_1 | CLKSEL_CLKSL__HFOSC0;
	// wait for clock divider ready
	while ((CLKSEL & CLKSEL_DIVRDY__BMASK) == CLKSEL_DIVRDY__NOT_READY);
	// now switch to HFOSC1 with divider of HFOSC1/8 to get 6 MHz
	CLKSEL = CLKSEL_CLKDIV__SYSCLK_DIV_8 | CLKSEL_CLKSL__HFOSC1;
	//wait for clock to propagate through divider
	while ((CLKSEL & CLKSEL_DIVRDY__BMASK) == CLKSEL_DIVRDY__NOT_READY);
	SFRPAGE = 0x0;						//return to default SFR page
}

//-----------------------------------------------------------------------------
// Port_Init() Routine
// ----------------------------------------------------------------------------
// configures port IO for LEDs
//-----------------------------------------------------------------------------
static void Port_Init(void)//function is only visible to this file
{
   P2MDOUT = P2MDOUT_B1__OPEN_DRAIN;//configure heartbeat LED (2.1) as open-drain
   //P1SKIP = P1SKIP_B4__SKIPPED;
   XBR2 = XBR2_XBARE__ENABLED;      // Enable crossbar
}

//-----------------------------------------------------------------------------
// Timer0_Init() Routine
// ----------------------------------------------------------------------------
// configures timer 0 to interrupt every 0.5 seconds
// uses sysclk/48 as timebase. requires reloading in ISR
//-----------------------------------------------------------------------------
void Timer0_Init(void)
{
   TH0 = TIMER_RELOAD_HIGH;            // Init Timer0 High register
   TL0 = TIMER_RELOAD_LOW;             // Init Timer0 Low register
   TMOD = TMOD_T0M__MODE1;             // Timer0 in 16-bit mode
   CKCON0 = CKCON0_SCA__SYSCLK_DIV_48; // Timer0 uses a 1:48 prescaler
   IE_ET0 = 1;                         // Timer0 overflow interrupt enabled
   TCON = TCON_TR0__RUN;               // Timer0 ON
}

//-----------------------------------------------------------------------------
// VCPXpress_API_CALLBACK() Routine
// ----------------------------------------------------------------------------
// callback for USB interrupts
//-----------------------------------------------------------------------------
VCPXpress_API_CALLBACK(myAPICallback)
{
   uint32_t INTVAL = Get_Callback_Source();
   //uint8_t i;


   if (INTVAL & DEVICE_OPEN)
   {
      Block_Read(RX_Packet, PACKET_SIZE, &InCount);   // Start first USB Read
      Is_Open_Flag = 1;								  // allow block_write in main
   }

   if ((INTVAL & DEVICE_CLOSE) || (INTVAL & DEV_SUSPEND))
   {
	   Is_Open_Flag = 0;							  // disallow block_write
   }

   /*//do nothing on RX_Complete for now
   if (INTVAL & RX_COMPLETE)                          // USB Read complete
   {
      for (i=0; i<InCount;i++)                        // Copy received packet to output buffer
      {
         TX_Packet[i] = RX_Packet[i];
      }
      Block_Write(TX_Packet, InCount, &OutCount);     // Start USB Write
   }
	*/
   if (INTVAL & TX_COMPLETE)                          // USB Write complete
   {
      Block_Read(RX_Packet, PACKET_SIZE, &InCount);   // Start next USB Read if available
   }

}

//-----------------------------------------------------------------------------
// Timer0_ISR() Routine
// ----------------------------------------------------------------------------
// interrupt service routine for timer 0
// reloads timer0 and toggles LED
//-----------------------------------------------------------------------------
SI_INTERRUPT(Timer0_ISR, TIMER0_IRQn)
{
  TH0 = TIMER_RELOAD_HIGH;            // Reload Timer0 High register
  TL0 = TIMER_RELOAD_LOW;             // Reload Timer0 Low register

  LED1 = !LED1;                       // Change state of LED
  TX_Flag = 1;						  // set flag to send message to host
}
