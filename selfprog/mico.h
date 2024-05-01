#ifndef MICO_H
#define MICO_H

#include "hardware/address_mapped.h"

#include "hardware/regs/intctrl.h"
#include "hardware/regs/m0plus.h"

#include "hardware/structs/dma.h"
#include "hardware/structs/nvic.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/pwm.h"
#include "hardware/structs/rosc.h"
#include "hardware/structs/scb.h"
#include "hardware/structs/systick.h"
#include "hardware/structs/timer.h"
#include "hardware/structs/watchdog.h"

//-----------------------------------------------------------------------------------------------------------------------
// atomic xor/set/clr aliases
//
// replaces hw_set_* hw_clear_* hw_xor_*
// source: src/rp2_common/hardware_base/include/hardware/address_mapped.h
// works for registers with 0x4000000 <= address < 0x60000000 (i.e. APB Peripherals, AHB-Lite Peripherals, DMA, USB)
// does not work for IOPORT (SIO)

#define MICO_ALIAS_RW(x) (x)
#define MICO_ALIAS_XOR(x) (*(typeof(&(x)))((1<<12) | (uintptr_t)&(x)))
#define MICO_ALIAS_SET(x) (*(typeof(&(x)))((2<<12) | (uintptr_t)&(x)))
#define MICO_ALIAS_CLR(x) (*(typeof(&(x)))((3<<12) | (uintptr_t)&(x)))

//-----------------------------------------------------------------------------------------------------------------------
// dma

typedef struct {
	uint32_t read_addr;
	uint32_t write_addr;
	uint32_t transfer_count;
	uint32_t ctrl_trig;
} mico_dma_block_0rwtc_t;

typedef struct {
	uint32_t ctrl_trig;
	uint32_t read_addr;
	uint32_t write_addr;
	uint32_t transfer_count;
} mico_dma_block_1crwt_t;

typedef struct {
	uint32_t ctrl_trig;
	uint32_t transfer_count;
	uint32_t read_addr;
	uint32_t write_addr;
} mico_dma_block_2ctrw_t;

typedef struct {
	uint32_t ctrl_trig;
	uint32_t write_addr;
	uint32_t transfer_count;
	uint32_t read_addr;
} mico_dma_block_3cwtr_t;

#define MICO_DMA_CTRL(en,hp,ds,ir,iw,rsize,rsel,ct,ts,iq,bswap,se)		\
	(/*en*/(en) << DMA_CH0_CTRL_TRIG_EN_LSB								\
	 | /*high_priority*/(hp) << DMA_CH0_CTRL_TRIG_HIGH_PRIORITY_LSB		\
	 | /*data_size*/(ds) << DMA_CH0_CTRL_TRIG_DATA_SIZE_LSB				\
	 | /*incr_read*/(ir) << DMA_CH0_CTRL_TRIG_INCR_READ_LSB				\
	 | /*incr_write*/(iw) << DMA_CH0_CTRL_TRIG_INCR_WRITE_LSB			\
	 | /*ring_size*/(rsize) << DMA_CH0_CTRL_TRIG_RING_SIZE_LSB			\
	 | /*ring_sel r0w1*/(rsel) << DMA_CH0_CTRL_TRIG_RING_SEL_LSB		\
	 | /*chain_to*/(ct) << DMA_CH0_CTRL_TRIG_CHAIN_TO_LSB				\
	 | /*treq_sel*/(ts) << DMA_CH0_CTRL_TRIG_TREQ_SEL_LSB				\
	 | /*irq_quiet*/(iq) << DMA_CH0_CTRL_TRIG_IRQ_QUIET_LSB				\
	 | /*bswap*/(bswap) << DMA_CH0_CTRL_TRIG_BSWAP_LSB					\
	 | /*sniff_en*/(se) << DMA_CH0_CTRL_TRIG_SNIFF_EN_LSB				\
	 )

#define MICO_DMA_CTRL_BLOCK4(chain_to)									\
	MICO_DMA_CTRL(/*en*/1,												\
				  /*high_priority*/0,									\
				  /*data_size*/DMA_CH0_CTRL_TRIG_DATA_SIZE_VALUE_SIZE_WORD, \
				  /*incr_read*/1,										\
				  /*incr_write*/1,										\
				  /*ring_size 1<<x bytes*/4,							\
				  /*ring_sel r0w1*/1,									\
				  /*chain_to*/(chain_to),								\
				  /*treq_sel*/0x3F,										\
				  /*irq_quiet*/1,										\
				  /*bswap*/0,											\
				  /*sniff_en*/0)

#define MICO_DMA_CTRL_RX(chain_to, treq_sel)							\
	MICO_DMA_CTRL(/*en*/1,												\
				  /*high_priority*/0,									\
				  /*data_size*/DMA_CH0_CTRL_TRIG_DATA_SIZE_VALUE_SIZE_WORD,	\
				  /*incr_read*/0,										\
				  /*incr_write*/1,										\
				  /*ring_size*/0,										\
				  /*ring_sel r0w1*/0,									\
				  /*chain_to*/(chain_to),								\
				  /*treq_sel*/(treq_sel),								\
				  /*irq_quiet*/1,										\
				  /*bswap*/0,											\
				  /*sniff_en*/0)

#define MICO_DMA_CTRL_TX(chain_to, treq_sel)							\
	MICO_DMA_CTRL(/*en*/1,												\
				  /*high_priority*/0,									\
				  /*data_size*/DMA_CH0_CTRL_TRIG_DATA_SIZE_VALUE_SIZE_WORD,	\
				  /*incr_read*/1,										\
				  /*incr_write*/0,										\
				  /*ring_size*/0,										\
				  /*ring_sel r0w1*/0,									\
				  /*chain_to*/(chain_to),								\
				  /*treq_sel*/(treq_sel),								\
				  /*irq_quiet*/1,										\
				  /*bswap*/0,											\
				  /*sniff_en*/0)

//-----------------------------------------------------------------------------------------------------------------------
// io

#define MICO_IO_BANK0_GPIO_CTRL(funcsel,outover,oeover,inover,irqover) \
	(/*funcsel*/(funcsel) << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB			\
	 | /*outover*/(outover) << IO_BANK0_GPIO0_CTRL_OUTOVER_LSB			\
	 | /*oeover*/(oeover) << IO_BANK0_GPIO0_CTRL_OEOVER_LSB				\
	 | /*inover*/(inover) << IO_BANK0_GPIO0_CTRL_INOVER_LSB				\
	 | /*irqover*/(irqover) << IO_BANK0_GPIO0_CTRL_IRQOVER_LSB			\
	 )

#define MICO_IO_BANK0_GPIO_CTRL_FUNCSEL(funcsel)						\
	MICO_IO_BANK0_GPIO_CTRL((funcsel), /*outover*/0, /*oeover*/0, /*inover*/0, /*irqover*/0)

//-----------------------------------------------------------------------------------------------------------------------
// irq isr

typedef void mico_isr_t(void);

inline static void mico_irq_isr(uint irq, mico_isr_t *isr) {
    ((mico_isr_t**)scb_hw->vtor)[VTABLE_FIRST_IRQ + irq] = isr;
}

// hardware_irq/irq.c:irq_set_enabled(num, enabled):
inline static void mico_irq_clrpend(uint32_t mask) { *(io_rw_32*)(PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET) = mask; }
inline static void mico_irq_enable(uint32_t mask) {	*(io_rw_32*)(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET) = mask; }
inline static void mico_irq_clr(uint32_t mask) { *(io_rw_32*)(PPB_BASE + M0PLUS_NVIC_ICER_OFFSET) = mask; }
inline static void mico_irq_clrpend_enable(uint32_t mask) { mico_irq_clrpend(mask); mico_irq_enable(mask); }

//-----------------------------------------------------------------------------------------------------------------------
// pads

#define MICO_PADS_BANK0_GPIO(od,ie,drive,pue,pde,schmitt,slewfast)	\
	(/*od*/(od) << PADS_BANK0_GPIO0_OD_LSB							\
	 | /*ie*/(ie) << PADS_BANK0_GPIO0_IE_LSB						\
	 | /*drive*/(drive) << PADS_BANK0_GPIO0_DRIVE_LSB				\
	 | /*pue*/(pue) << PADS_BANK0_GPIO0_PUE_LSB						\
	 | /*pde*/(pde) << PADS_BANK0_GPIO0_PDE_LSB						\
	 | /*schmitt*/(schmitt) << PADS_BANK0_GPIO0_SCHMITT_LSB			\
	 | /*slewfast*/(slewfast) << PADS_BANK0_GPIO0_SLEWFAST_LSB		\
	 )

#define MICO_PADS_BANK0_GPIO_DRIVE(drive) \
	MICO_PADS_BANK0_GPIO(/*od*/0, /*ie*/1, /*drive*/drive, /*pue*/0, /*pde*/0, /*schmitt*/1, /*slewfast*/0);

#define MICO_PADS_BANK0_GPIO_DRIVE_PUE(drive) \
	MICO_PADS_BANK0_GPIO(/*od*/0, /*ie*/1, /*drive*/drive, /*pue*/1, /*pde*/0, /*schmitt*/1, /*slewfast*/0);

#define MICO_PADS_BANK0_GPIO_DRIVE_PDE(drive) \
	MICO_PADS_BANK0_GPIO(/*od*/0, /*ie*/1, /*drive*/drive, /*pue*/0, /*pde*/1, /*schmitt*/1, /*slewfast*/0);

#define MICO_PADS_BANK0_GPIO_OD() \
	MICO_PADS_BANK0_GPIO(/*od*/1, /*ie*/1, /*drive*/0, /*pue*/0, /*pde*/0, /*schmitt*/1, /*slewfast*/0);

#define MICO_PADS_BANK0_GPIO_OD_PUE(drive)								\
	MICO_PADS_BANK0_GPIO(/*od*/1, /*ie*/1, /*drive*/0, /*pue*/1, /*pde*/0, /*schmitt*/1, /*slewfast*/0);

#define MICO_PADS_BANK0_GPIO_OD_PDE(drive) \
	MICO_PADS_BANK0_GPIO(/*od*/1, /*ie*/1, /*drive*/0, /*pue*/0, /*pde*/1, /*schmitt*/1, /*slewfast*/0);

//-----------------------------------------------------------------------------------------------------------------------
// pwm

#define MICO_PWM_CSR(en,pc,ai,bi,divmode,pr,pa)			\
	(/*en*/(en) << PWM_CH0_CSR_EN_LSB					\
	 | /*ph_correct*/(pc) << PWM_CH0_CSR_PH_CORRECT_LSB	\
	 | /*a_inv*/(ai) << PWM_CH0_CSR_A_INV_LSB			\
	 | /*b_inv*/(bi) << PWM_CH0_CSR_B_INV_LSB			\
	 | /*divmode*/(divmode) << PWM_CH0_CSR_DIVMODE_LSB	\
	 | /*ph_ret*/(pr) << PWM_CH0_CSR_PH_RET_LSB			\
	 | /*ph_adv*/(pa) << PWM_CH0_CSR_PH_ADV_LSB)

#define MICO_PWM_DIV(frac,int_)					\
	(/*frac*/(frac) << PWM_CH0_DIV_FRAC_LSB		\
	 | /*int*/(int_) << PWM_CH0_DIV_INT_LSB		\
	 )

#define MICO_PWM_CC(a,b)						\
	(/*a*/(a) << PWM_CH0_CC_A_LSB				\
	 | /*b*/(b) << PWM_CH0_CC_B_LSB				\
	 )

//-----------------------------------------------------------------------------------------------------------------------
// rando

inline static uint32_t mico_rando(uint len) {
	uint32_t acc;
	for(uint i = 0; i < len; i++)
		acc = (acc << 1) ^ rosc_hw->randombit;
	return acc;
}

//-----------------------------------------------------------------------------------------------------------------------
// reset_block
//
// hardware/resets.h:reset_block(bits): MICO_ALIAS_SET(resets_hw)->reset = bits
// hardware/resets.h:unreset_block(bits): MICO_ALIAS_CLR(resets_hw)->reset = bits

#endif
