#ifndef MICO_PIO_H
#define MICO_PIO_H

#include "mico.h"

#include <hardware/pio.h>

//-----------------------------------------------------------------------------------------------------------------------
// pio

#define MICO_PIO_CTRL(se,sr,cr)						\
	(/*sm_enable*/(se) << PIO_CTRL_SM_ENABLE_LSB				\
	 | /*sm_restart*/(sr) << PIO_CTRL_SM_RESTART_LSB			\
	 | /*clkdiv_restart*/(cr) << PIO_CTRL_CLKDIV_RESTART_LSB	\
	 )

#define MICO_PIO_SM_CLKDIV(frac,int_)			\
	(/*frac*/(frac) << PIO_SM0_CLKDIV_FRAC_LSB	\
	 | /*int*/(int_) << PIO_SM0_CLKDIV_INT_LSB	\
	 )

// wrap is from top to bottom. top to bottom. as in hi PC to lo PC. not how code looks on a screen. don't forget.
#define MICO_PIO_SM_EXECCTRL(sn,ss,wb,wt,os,ioe,oes,jp,sp,se)		\
	(/*status_n*/(sn) << PIO_SM0_EXECCTRL_STATUS_N_LSB				\
	 | /*status_sel*/(ss) << PIO_SM0_EXECCTRL_STATUS_SEL_LSB		\
	 | /*wrap_bottom*/(wb) << PIO_SM0_EXECCTRL_WRAP_BOTTOM_LSB		\
	 | /*wrap_top*/(wt) << PIO_SM0_EXECCTRL_WRAP_TOP_LSB			\
	 | /*out_sticky*/(os) << PIO_SM0_EXECCTRL_OUT_STICKY_LSB		\
	 | /*inline_out_en*/(ioe) << PIO_SM0_EXECCTRL_INLINE_OUT_EN_LSB	\
	 | /*out_en_sel*/(oes) << PIO_SM0_EXECCTRL_OUT_EN_SEL_LSB		\
	 | /*jmp_pin*/(jp) << PIO_SM0_EXECCTRL_JMP_PIN_LSB				\
	 | /*side_pindir*/(sp) << PIO_SM0_EXECCTRL_SIDE_PINDIR_LSB		\
	 | /*side_en*/(se) << PIO_SM0_EXECCTRL_SIDE_EN_LSB				\
	 )

#define MICO_PIO_SM_SHIFTCTRL(autopush,autopull,is,os,pusht,pullt,ft,fr) \
	(/*autopush*/(autopush) << PIO_SM0_SHIFTCTRL_AUTOPUSH_LSB			\
	 | /*autopull*/(autopull) << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB			\
	 | /*in_shiftdir l0r1*/ (is) << PIO_SM0_SHIFTCTRL_IN_SHIFTDIR_LSB	\
	 | /*out_shiftdir l0r1*/ (os) << PIO_SM0_SHIFTCTRL_OUT_SHIFTDIR_LSB	\
	 | /*push_thresh 0->32*/ (pusht) << PIO_SM0_SHIFTCTRL_PUSH_THRESH_LSB \
	 | /*pull_thresh 0->32*/ (pullt) << PIO_SM0_SHIFTCTRL_PULL_THRESH_LSB \
	 | /*fjoin_tx*/(ft) << PIO_SM0_SHIFTCTRL_FJOIN_TX_LSB				\
	 | /*fjoin_rx*/(fr) << PIO_SM0_SHIFTCTRL_FJOIN_RX_LSB				\
	 )

#define MICO_PIO_SM_PINCTRL(ob,sb,ssb,ib,oc,sc,ssc)					\
	(/*out_base*/(ob) << PIO_SM0_PINCTRL_OUT_BASE_LSB				\
	 | /*set_base*/(sb) << PIO_SM0_PINCTRL_SET_BASE_LSB				\
	 | /*sideset_base*/(ssb) << PIO_SM0_PINCTRL_SIDESET_BASE_LSB	\
	 | /*in_base*/(ib) << PIO_SM0_PINCTRL_IN_BASE_LSB				\
	 | /*out_count*/(oc) << PIO_SM0_PINCTRL_OUT_COUNT_LSB			\
	 | /*set_count*/(sc) << PIO_SM0_PINCTRL_SET_COUNT_LSB			\
	 | /*sideset_count*/(ssc) << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB	\
	 )

// pio instruction encoding

typedef enum mico_pio_src_dst {
    MICO_PIO_PINS = 0,
    MICO_PIO_X = 1,
    MICO_PIO_Y = 2,
    MICO_PIO_NULL = 3,
    MICO_PIO_PINDIRS = 4,
    MICO_PIO_EXEC_MOV = 4,
    MICO_PIO_STATUS = 5,
    MICO_PIO_PC = 5,
    MICO_PIO_ISR = 6,
    MICO_PIO_OSR = 7,
    MICO_PIO_EXEC_OUT = 7,
} mico_pio_src_dst_t;

#define MICO_PIO_DELAY(value)                ((value) << 8)
#define MICO_PIO_SIDESET(count, value)       ((value) << (13 - (count)))
#define MICO_PIO_JMP(addr)                   (0x0000 | 0<<5 | (addr))
#define MICO_PIO_JMP_NOT_X(addr)             (0x0000 | 1<<5 | (addr))
#define MICO_PIO_JMP_X_DEC(addr)             (0x0000 | 2<<5 | (addr))
#define MICO_PIO_JMP_NOT_Y(addr)             (0x0000 | 3<<5 | (addr))
#define MICO_PIO_JMP_Y_DEC(addr)             (0x0000 | 4<<5 | (addr))
#define MICO_PIO_JMP_X_NE_Y(addr)            (0x0000 | 5<<5 | (addr))
#define MICO_PIO_JMP_PIN(addr)               (0x0000 | 6<<5 | (addr))
#define MICO_PIO_JMP_NOT_OSRE(addr)          (0x0000 | 7<<5 | (addr))
#define MICO_PIO_WAIT_GPIO(polarity, gpio)   (0x2000 | (polarity)<<7 | 0<<5 | (gpio))
#define MICO_PIO_WAIT_PIN(polarity, pin)     (0x2000 | (polarity)<<7 | 1<<5 | (pin))
#define MICO_PIO_WAIT_IRQ(polarity, irq)     (0x2000 | (polarity)<<7 | 2<<5 | 0<<4 | (irq))
#define MICO_PIO_WAIT_IRQ_REL(polarity, irq) (0x2000 | (polarity)<<7 | 2<<5 | 1<<4 | (irq))
#define MICO_PIO_IN(src, count)              (0x4000 | (src)<<5 | ((count) & 0x1F))
#define MICO_PIO_OUT(dst, count)             (0x6000 | (dst)<<5 | ((count) & 0x1F))
#define MICO_PIO_PUSH(iffull, block)         (0x8000 | (iffull)<<6 | (block)<<5)
#define MICO_PIO_PULL(ifempty, block)        (0x8080 | (ifempty)<<6 | (block)<<5)
#define MICO_PIO_MOV(dst, src)               (0xa000 | (dst)<<5 | 0<<3 | (src))
#define MICO_PIO_MOV_INVERT(dst, src)        (0xa000 | (dst)<<5 | 1<<3 | (src))
#define MICO_PIO_MOV_REVERSE(dst, src)       (0xa000 | (dst)<<5 | 2<<3 | (src))
#define MICO_PIO_IRQ(clr, wait, index)       (0xc000 | (clr)<<6 | (wait)<<5 | 0<<4 | (index))
#define MICO_PIO_IRQ_REL(clr, wait, index)   (0xc000 | (clr)<<6 | (wait)<<5 | 1<<4 | (index))
#define MICO_PIO_SET(dst, data)              (0xe000 | (dst)<<5 | ((data) & 0x1F))

#define MICO_PIO_NOP() MICO_PIO_MOV(MICO_PIO_Y, MICO_PIO_Y)

#define MICO_PIO_IS_JMP(instr) (0 == ((instr) & 0xE000))

// pio_load
//
// No DMA because
// (1) src is 16b instructions, dst is 32b registers
// (2) relocation modifications to non-zero dst_off (jmp instrs must be incremented by dst_off)

inline static uint mico_pio_load(io_wo_32 *dst_base, uint dst_off, const uint16_t *src, uint count) {
	dst_base += dst_off;
	for(uint i = 0; i < count; i++) {
        uint16_t instr = src[i];
		if(MICO_PIO_IS_JMP(instr))
			instr += dst_off;
		dst_base[i] = instr;
    }
	return dst_off;
}

inline static uint mico_pio_load_program(io_wo_32 *dst_base, uint dst_off, const pio_program_t *program) {
	return mico_pio_load(dst_base, dst_off, program->instructions, program->length);
}

#endif
