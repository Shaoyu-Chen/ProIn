#include <stdint.h>
#include "main.h"
#include "peripheral.h"

extern uint32_t _empty_sp_;

/*
 *  Vector Table Definiiton
 */
void DFLT_Handler()
{
    while(1);
}

void Reset_Handler()		__attribute__ ((weak, alias ("DFLT_Handler")));
void NMI_Handler()		__attribute__ ((weak, alias ("DFLT_Handler")));
void HardFault_Handler()	__attribute__ ((weak, alias ("DFLT_Handler")));
void MemManage_Handler()	__attribute__ ((weak, alias ("DFLT_Handler")));
void BusFault_Handler()		__attribute__ ((weak, alias ("DFLT_Handler")));
void UsageFault_Handler()	__attribute__ ((weak, alias ("DFLT_Handler")));
void SVC_Handler()		__attribute__ ((weak, alias ("DFLT_Handler")));
void DebugMon_Handler()		__attribute__ ((weak, alias ("DFLT_Handler")));
void PendSV_Handler()		__attribute__ ((weak, alias ("DFLT_Handler")));
void SysTick_Handler()		__attribute__ ((weak, alias ("DFLT_Handler")));

uint32_t *vector_table[]
__attribute__ ((section(".vector_table"))) = {
    [0] = (uint32_t *) &_empty_sp_,		// Initial SP
    [1] = (uint32_t *) Reset_Handler,           // Entry
    [2] = (uint32_t *) NMI_Handler,             // NMI
    [3] = (uint32_t *) HardFault_Handler,		// Hard Fault
    [4] = (uint32_t *) MemManage_Handler,       // MPU Fault
    [5] = (uint32_t *) BusFault_Handler,        // Bus Fault
    [6] = (uint32_t *) UsageFault_Handler,      // Usage Fault
    [7 ... 10] = NULL,
    [11] = (uint32_t *) SVC_Handler,		// SuperVisor Call
    [12] = (uint32_t *) DebugMon_Handler,	// Debug Monitor
    [13] = NULL,
    [14] = (uint32_t *) PendSV_Handler,		// Pendable Service Call
    [15] = (uint32_t *) SysTick_Handler		// System Tick Timer
};

