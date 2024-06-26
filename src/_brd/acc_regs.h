/*
 *==============================================================================
 * Name: acc_regs.h
 * Author: Martin Stankus
 *==============================================================================
 */

#ifndef _ACC_REGS_H_
#define _ACC_REGS_H_

#define ACC_ADDR_SAMP_BLK				0x01u

#define ACC_ADDR_PROD_REV				0x12u
#define ACC_ADDR_WHOAMI					0x13u
#define ACC_ADDR_SMODE					0x14u
#define ACC_ADDR_CFG1					0x15u
#define ACC_ADDR_CFG2					0x16u
#define ACC_ADDR_CFG3					0x17u
#define ACC_ADDR_CFG4					0x18u
#define ACC_ADDR_CFG5					0x19u
#define ACC_ADDR_INTEN					0x20u
#define ACC_ADDR_INTPIN					0x21u

#define ACC_WHOAMI_FXLS8974CF			0x86u

#define ACC_SMODE_MODE_MASK				0x03u
#define ACC_SMODE_MODE_SHFT				0u
#define ACC_SMODE_MODE_ENC(val)			(((val) & ACC_SMODE_MODE_MASK) << ACC_SMODE_MODE_SHFT)
#define ACC_SMODE_MODE_DEC(val)			(((val) >> ACC_SMODE_MODE_SHFT) & ACC_SMODE_MODE_MASK)

#define ACC_CFG1_RST_MASK				0x01u
#define ACC_CFG1_RST_SHFT				7u
#define ACC_CFG1_RST_ENC(val)			(((val) & ACC_CFG1_RST_MASK) << ACC_CFG1_RST_SHFT)
#define ACC_CFG1_RST_DEC(val)			(((val) >> ACC_CFG1_RST_SHFT) & ACC_CFG1_RST_MASK)

#define ACC_CFG1_FSR_MASK				0x03u
#define ACC_CFG1_FSR_SHFT				1u
#define ACC_CFG1_FSR_ENC(val)			(((val) & ACC_CFG1_FSR_MASK) << ACC_CFG1_FSR_SHFT)
#define ACC_CFG1_FSR_DEC(val)			(((val) >> ACC_CFG1_FSR_SHFT) & ACC_CFG1_FSR_MASK)

#define ACC_CFG1_ACT_MASK				0x01u
#define ACC_CFG1_ACT_SHFT				0u
#define ACC_CFG1_ACT_ENC(val)			(((val) & ACC_CFG1_ACT_MASK) << ACC_CFG1_ACT_SHFT)
#define ACC_CFG1_ACT_DEC(val)			(((val) >> ACC_CFG1_ACT_SHFT) & ACC_CFG1_ACT_MASK)

#define ACC_CFG2_WAKEPM_MASK			0x03u
#define ACC_CFG2_WAKEPM_SHFT			6u
#define ACC_CFG2_WAKEPM_ENC(val)		(((val) & ACC_CFG2_WAKEPM_MASK) << ACC_CFG2_WAKEPM_SHFT)
#define ACC_CFG2_WAKEPM_DEC(val)		(((val) >> ACC_CFG2_WAKEPM_SHFT) & ACC_CFG2_WAKEPM_MASK)

#define ACC_CFG2_AINCTEMP_MASK			0x01u
#define ACC_CFG2_AINCTEMP_SHFT			1u
#define ACC_CFG2_AINCTEMP_ENC(val)		(((val) & ACC_CFG2_AINCTEMP_MASK) << ACC_CFG2_AINCTEMP_SHFT)
#define ACC_CFG2_AINCTEMP_DEC(val)		(((val) >> ACC_CFG2_AINCTEMP_SHFT) & ACC_CFG2_AINCTEMP_MASK)

#define ACC_CFG3_WAKEODRDEC_MASK		0x0Fu
#define ACC_CFG3_WAKEODRDEC_SHFT		4u
#define ACC_CFG3_WAKEODRDEC_ENC(val)	(((val) & ACC_CFG3_WAKEODRDEC_MASK) << ACC_CFG3_WAKEODRDEC_SHFT)
#define ACC_CFG3_WAKEODRDEC_DEC(val)	(((val) >> ACC_CFG3_WAKEODRDEC_SHFT) & ACC_CFG3_WAKEODRDEC_MASK)

#define ACC_CFG4_DRDYPUL_MASK			0x01u
#define ACC_CFG4_DRDYPUL_SHFT			3u
#define ACC_CFG4_DRDYPUL_ENC(val)		(((val) & ACC_CFG4_DRDYPUL_MASK) << ACC_CFG4_DRDYPUL_SHFT)
#define ACC_CFG4_DRDYPUL_DEC(val)		(((val) >> ACC_CFG4_DRDYPUL_SHFT) & ACC_CFG4_DRDYPUL_MASK)

#define ACC_CFG4_INTPOL_MASK			0x01u
#define ACC_CFG4_INTPOL_SHFT			0u
#define ACC_CFG4_INTPOL_ENC(val)		(((val) & ACC_CFG4_INTPOL_MASK) << ACC_CFG4_INTPOL_SHFT)
#define ACC_CFG4_INTPOL_DEC(val)		(((val) >> ACC_CFG4_INTPOL_SHFT) & ACC_CFG4_INTPOL_MASK)

#define ACC_CFG5_VECMEN_MASK			0x01u
#define ACC_CFG5_VECMEN_SHFT			4u
#define ACC_CFG5_VECMEN_ENC(val)		(((val) & ACC_CFG5_VECMEN_MASK) << ACC_CFG5_VECMEN_SHFT)
#define ACC_CFG5_VECMEN_DEC(val)		(((val) >> ACC_CFG5_VECMEN_SHFT) & ACC_CFG5_VECMEN_MASK)

#define ACC_INTEN_DRDYEN_MASK			0x01u
#define ACC_INTEN_DRDYEN_SHFT			7u
#define ACC_INTEN_DRDYEN_ENC(val)		(((val) & ACC_INTEN_DRDYEN_MASK) << ACC_INTEN_DRDYEN_SHFT)
#define ACC_INTEN_DRDYEN_DEC(val)		(((val) >> ACC_INTEN_DRDYEN_SHFT) & ACC_INTEN_DRDYEN_MASK)

#define ACC_INTPIN_DRDYINT2_MASK		0x01u
#define ACC_INTPIN_DRDYINT2_SHFT		7u
#define ACC_INTPIN_DRDYINT2_ENC(val)	(((val) & ACC_INTPIN_DRDYINT2_MASK) << ACC_INTPIN_DRDYINT2_SHFT)
#define ACC_INTPIN_DRDYINT2_DEC(val)	(((val) >> ACC_INTPIN_DRDYINT2_SHFT) & ACC_INTPIN_DRDYINT2_MASK)

#define ACC_SMODE_MODE_STDB				0u
#define ACC_SMODE_MODE_WAKE				1u
//#define ACC_SMODE_MODE_SLEEP			2u
//#define ACC_SMODE_MODE_EXTRG			3u

#define ACC_CFG1_FSR_2G					0u
#define ACC_CFG1_FSR_4G					1u
#define ACC_CFG1_FSR_8G					2u
#define ACC_CFG1_FSR_16G				3u

#define ACC_CFG2_WAKEPM_LP				0u
#define ACC_CFG2_WAKEPM_HP				1u
//#define ACC_CFG2_WAKEPM_FP			2u

#define ACC_CFG3_WAKEODR_3200HZ			0u
#define ACC_CFG3_WAKEODR_1600HZ			1u
#define ACC_CFG3_WAKEODR_800HZ			2u
#define ACC_CFG3_WAKEODR_400HZ			3u
#define ACC_CFG3_WAKEODR_200HZ			4u
#define ACC_CFG3_WAKEODR_100HZ			5u
#define ACC_CFG3_WAKEODR_50HZ			6u
#define ACC_CFG3_WAKEODR_25HZ			7u
#define ACC_CFG3_WAKEODR_12_5HZ			8u
#define ACC_CFG3_WAKEODR_6_25HZ			9u
#define ACC_CFG3_WAKEODR_3_125HZ		10u
#define ACC_CFG3_WAKEODR_1_563HZ		11u
#define ACC_CFG3_WAKEODR_0_781HZ		12u

/*
#define ACC_CFG3_WAKEDEC_1SMP			0u
#define ACC_CFG3_WAKEDEC_2SMP			1u
#define ACC_CFG3_WAKEDEC_4SMP			2u
#define ACC_CFG3_WAKEDEC_8SMP			3u
#define ACC_CFG3_WAKEDEC_16SMP			4u
#define ACC_CFG3_WAKEDEC_32SMP			5u
#define ACC_CFG3_WAKEDEC_64SMP			6u
#define ACC_CFG3_WAKEDEC_128SMP			7u
#define ACC_CFG3_WAKEDEC_256SMP			8u
#define ACC_CFG3_WAKEDEC_512SMP			9u
#define ACC_CFG3_WAKEDEC_1024SMP		10u
#define ACC_CFG3_WAKEDEC_2048SMP		11u
#define ACC_CFG3_WAKEDEC_4096SMP		12u
*/

#endif /* _ACC_REGS_H_ */
