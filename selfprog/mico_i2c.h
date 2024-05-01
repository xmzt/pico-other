#ifndef MICO_I2C_H
#define MICO_I2C_H

#include "mico.h"

#include "hardware/structs/i2c.h"

//-----------------------------------------------------------------------------------------------------------------------
// i2c

#define MICO_I2C_DATA_WRITE(restart,stop,data)		   \
	(/*restart*/(restart) << I2C_IC_DATA_CMD_RESTART_LSB	\
	 | /*stop*/(stop) << I2C_IC_DATA_CMD_STOP_LSB			\
	 | /*w0r1*/ 0 << I2C_IC_DATA_CMD_CMD_LSB		   \
	 | /*data*/(data))

#define MICO_I2C_DATA_READ(restart,stop)		\
	(/*restart*/(restart) << I2C_IC_DATA_CMD_RESTART_LSB	\
	 | /*stop*/(stop) << I2C_IC_DATA_CMD_STOP_LSB			\
	 | /*w0r1*/ 1 << I2C_IC_DATA_CMD_CMD_LSB)

#define MICO_I2C_CON(mm,speed,tenas,tenam,re,sd,sdi,tec,rffhc)			\
	(/*master_mode*/(mm) << I2C_IC_CON_MASTER_MODE_LSB					\
	 | /*speed*/(speed) << I2C_IC_CON_SPEED_LSB							\
	 | /*ic_10bitaddr_slave*/(tenas) << I2C_IC_CON_IC_10BITADDR_SLAVE_LSB \
	 | /*ic_10bitaddr_master*/(tenam) << I2C_IC_CON_IC_10BITADDR_MASTER_LSB \
	 | /*ic_restart_en*/(re) << I2C_IC_CON_IC_RESTART_EN_LSB			\
	 | /*ic_slave_disable*/(sd) << I2C_IC_CON_IC_SLAVE_DISABLE_LSB		\
	 | /*stop_det_ifaddressed*/(sdi) << I2C_IC_CON_STOP_DET_IFADDRESSED_LSB \
	 | /*tx_empty_ctrl*/(tec) << I2C_IC_CON_TX_EMPTY_CTRL_LSB			\
	 | /*rx_fifo_full_hld_ctrl*/(rffhc) << I2C_IC_CON_RX_FIFO_FULL_HLD_CTRL_LSB \
	 )

void mico_i2c_set_baudrate_lt_1e6(i2c_hw_t *hw, uint baudrate);

#endif
