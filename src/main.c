//###########################################################################
// Description:
//! \addtogroup f2833x_example_list
//!  <h1>SCI Digital Loop Back with Interrupts (scia_loopback_interrupts)</h1>
//!
//! This program uses the internal loop back test mode of the peripheral.
//! Other then boot mode pin configuration, no other hardware configuration
//! is required. Both interrupts and the SCI FIFOs are used.
//
//###########################################################################
// $TI Release: F2833x/F2823x Header Files and Peripheral Examples V141 $
// $Release Date: November  6, 2015 $
// $Copyright: Copyright (C) 2007-2015 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "project.h"     // Device Headerfile and Examples Include File
#include "main.h"

#define             FLASH               0

#if FLASH
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
#endif

OLED *oled;
volatile float temp=10000;
volatile int  KeyRec=0;
unsigned char Forward[]={0x0004,0x0008,0x0010,0x0020};//正向旋转给定脉冲
unsigned char sForward[] = { 0x04, 0x04|0x08, 0x08, 0x08|0x10, 0x10, 0x10|0x20, 0x20, 0x20|0x04};
uint16_t speed;
uint16_t dir;
uint16_t mode;
uint8_t part_mode = S8_MODE;
Uint16 rdataA[8];
uint16_t refresh;


void main(void)
{
	uint16_t i;

	// Step 1. Initialize System Control:
	// PLL, WatchDog, enable Peripheral Clocks
	// This example function is found in the DSP2833x_SysCtrl.c file.
	InitSysCtrl();

#if  FLASH
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
	// Call Flash Initialization to setup flash waitstates
	// This function must reside in RAM
	InitFlash();
#endif
	// Step 2. Initialize GPIO:
	// This example function is found in the DSP2833x_Gpio.c file and
	// illustrates how to set the GPIO to it's default state.
	InitCpuTimers();                          //初始化定时器0
	InitGpio();
	GPIO_INIT();
	SCI_INIT();

	// Setup only the GP I/O only for SCI-A and SCI-B functionality
	// This function is found in DSP2833x_Sci.c

	// Step 3. Clear all interrupts and initialize PIE vector table:
	// Disable CPU interrupts
	DINT;
	oled = oled_new_dev();
	// Initialize PIE control registers to their default state.
	// The default state is all PIE interrupts disabled and flags
	// are cleared.
	// This function is found in the DSP2833x_PieCtrl.c file.
	InitPieCtrl();

	// Disable CPU interrupts and clear all CPU interrupt flags:
	IER = 0x0000;
	IFR = 0x0000;

	// Initialize the PIE vector table with pointers to the shell Interrupt
	// Service Routines (ISR).
	// This will populate the entire table, even if the interrupt
	// is not used in this example.  This is useful for debug purposes.
	// The shell ISR routines are found in DSP2833x_DefaultIsr.c.
	// This function is found in DSP2833x_PieVect.c.
	InitPieVectTable();

	// Interrupts that are used in this example are re-mapped to
	// ISR functions found within this file.
	EALLOW;  // This is needed to write to EALLOW protected registers
	PieVectTable.SCIRXINTA = &sciaRxFifoIsr;
	PieVectTable.SCIRXINTB = &scibRxFifoIsr;
	PieVectTable.SCIRXINTC = &scicRxFifoIsr;
	PieVectTable.TINT0 = &t0Isr;
	EDIS;   // This is needed to disable write to EALLOW protected registers
	// Step 5. User specific code, enable interrupts:
	// Enable interrupts required for this example
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
	PieCtrlRegs.PIEIER9.bit.INTx1=1;     // PIE Group 9, int1
	//PieCtrlRegs.PIEIER9.bit.INTx2=1;     // PIE Group 9, INT2
	PieCtrlRegs.PIEIER9.bit.INTx3=1;     // PIE Group 9, INT3
	//PieCtrlRegs.PIEIER9.bit.INTx4=1;     // PIE Group 9, INT4
	PieCtrlRegs.PIEIER9.bit.INTx5=1;     // PIE Group 9, INT3
	PieCtrlRegs.PIEIER1.bit.INTx7=1;          //使能PIE模块中的CPU定时器0的中断
	IER |= 0x100 | M_INT1; // Enable CPU INT
	EINT;
	// Step 6. IDLE loop. Just sit and loop forever (optional):
	ConfigCpuTimer(&CpuTimer0,150,PULSE_10MS);      //CPU定时器0的周期为10ms
	CpuTimer0Regs.TCR.bit.TSS = 0;
	display_all(85,0, S8_MODE);

	for(;;){
		if (refresh == 1) {
			refresh = 0;
			ConfigCpuTimer(&CpuTimer0,150,speed * 100);      //CPU定时器0的周期为10ms
			CpuTimer0Regs.TCR.bit.TSS = 0;
			part_mode = mode;
			display_all(speed, dir, mode);
		}
	}
}

void display_all(uint16_t speed, uint16_t dir, uint16_t mode )
{
	unsigned char s[50];
	sprintf(s, "speed: %d", speed);
	oled->showstring(oled, 0,0, s);

	if (dir == 0) {
		oled->showstring(oled, 0,2, "dir: +");
	}else{
		oled->showstring(oled, 0,2, "dir: -");
	}

	if (mode == S8_MODE) {
		oled->showstring(oled, 0,4, "mode: 08");
	}else{
		oled->showstring(oled, 0,4, "mode: 16");
	}
}

void GPIO_INIT(void)
{

	EALLOW;
	// STOP MOTOR;
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;
	GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;
	GpioCtrlRegs.GPAPUD.bit.GPIO4 = 1;
	GpioCtrlRegs.GPAPUD.bit.GPIO5 = 1;
	GpioCtrlRegs.GPAPUD.bit.GPIO6 = 1;


	EDIS;
}

// INT1.7
interrupt void  t0Isr(void)      // CPU-Timer 0
{
	// Insert ISR Code here

	// To receive more interrupts from this PIE group, acknowledge this interrupt
	// PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

	// Next two lines for debug only to halt the processor here
	// Remove after inserting ISR Code

	CpuTimer0.InterruptCount ++;
	if (dir == 0) {
		if(CpuTimer0.InterruptCount%8==2 || CpuTimer0.InterruptCount%8==1)
		{
			if (part_mode == S8_MODE)
				GpioDataRegs.GPADAT.all=Forward[0];
			else
				GpioDataRegs.GPADAT.all=sForward[0];
		}
		if(CpuTimer0.InterruptCount%8==3 || CpuTimer0.InterruptCount%8==2)
		{
			if (part_mode == S8_MODE)
				GpioDataRegs.GPADAT.all=Forward[1];
			else
				GpioDataRegs.GPADAT.all=sForward[1];
		}
		if(CpuTimer0.InterruptCount%8==4 || CpuTimer0.InterruptCount%8==3)
		{
			if (part_mode == S8_MODE)
				GpioDataRegs.GPADAT.all=Forward[2];
			else
				GpioDataRegs.GPADAT.all=sForward[2];
		}
		if(CpuTimer0.InterruptCount%8==5 || CpuTimer0.InterruptCount%8==4)
		{
			if (part_mode == S8_MODE)
				GpioDataRegs.GPADAT.all=Forward[3];
			else
				GpioDataRegs.GPADAT.all=sForward[3];
		}
		if(CpuTimer0.InterruptCount%8==6 || CpuTimer0.InterruptCount%8==5)
		{
			if (part_mode == S8_MODE)
				GpioDataRegs.GPADAT.all=Forward[0];
			else
				GpioDataRegs.GPADAT.all=sForward[4];
		}

		if(CpuTimer0.InterruptCount%8==7 || CpuTimer0.InterruptCount%8==6)
		{
			if (part_mode == S8_MODE)
				GpioDataRegs.GPADAT.all=Forward[1];
			else
				GpioDataRegs.GPADAT.all=sForward[5];
		}
		if(CpuTimer0.InterruptCount%8==7 || CpuTimer0.InterruptCount%8==0)
		{
			if (part_mode == S8_MODE)
				GpioDataRegs.GPADAT.all=Forward[2];
			else
				GpioDataRegs.GPADAT.all=sForward[6];
		}
		if(CpuTimer0.InterruptCount%8==0 || CpuTimer0.InterruptCount%8==1)
		{
			if (part_mode == S8_MODE)
				GpioDataRegs.GPADAT.all=Forward[3];
			else
				GpioDataRegs.GPADAT.all=sForward[7];
		}
	}else{
		if(CpuTimer0.InterruptCount%8==2 || CpuTimer0.InterruptCount%8==1)
		{
			if (part_mode == S8_MODE)
				GpioDataRegs.GPADAT.all=Forward[3];
			else
				GpioDataRegs.GPADAT.all=sForward[7];
		}
		if(CpuTimer0.InterruptCount%8==3 || CpuTimer0.InterruptCount%8==2)
		{
			if (part_mode == S8_MODE)
				GpioDataRegs.GPADAT.all=Forward[2];
			else
				GpioDataRegs.GPADAT.all=sForward[6];
		}
		if(CpuTimer0.InterruptCount%8==4 || CpuTimer0.InterruptCount%8==3)
		{
			if (part_mode == S8_MODE)
				GpioDataRegs.GPADAT.all=Forward[1];
			else
				GpioDataRegs.GPADAT.all=sForward[5];
		}
		if(CpuTimer0.InterruptCount%8==5 || CpuTimer0.InterruptCount%8==4)
		{
			if (part_mode == S8_MODE)
				GpioDataRegs.GPADAT.all=Forward[0];
			else
				GpioDataRegs.GPADAT.all=sForward[4];
		}
		if(CpuTimer0.InterruptCount%8==6 || CpuTimer0.InterruptCount%8==5)
		{
			if (part_mode == S8_MODE)
				GpioDataRegs.GPADAT.all=Forward[3];
			else
				GpioDataRegs.GPADAT.all=sForward[3];
		}

		if(CpuTimer0.InterruptCount%8==7 || CpuTimer0.InterruptCount%8==6)
		{
			if (part_mode == S8_MODE)
				GpioDataRegs.GPADAT.all=Forward[2];
			else
				GpioDataRegs.GPADAT.all=sForward[2];
		}
		if(CpuTimer0.InterruptCount%8==7 || CpuTimer0.InterruptCount%8==0)
		{
			if (part_mode == S8_MODE)
				GpioDataRegs.GPADAT.all=Forward[1];
			else
				GpioDataRegs.GPADAT.all=sForward[1];
		}
		if(CpuTimer0.InterruptCount%8==0 || CpuTimer0.InterruptCount%8==1)
		{
			if (part_mode == S8_MODE)
				GpioDataRegs.GPADAT.all=Forward[0];
			else
				GpioDataRegs.GPADAT.all=sForward[0];
		}
	}

	CpuTimer0Regs.TCR.bit.TIF=1;
	PieCtrlRegs.PIEACK.bit.ACK1=1;
	EINT;
}


