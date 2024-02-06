/*
 *==============================================================================
 * Name: soc_def_lptmr.h
 * Author: Martin Stankus
 *==============================================================================
 */

#ifndef _SOC_DEF_LPTMR_H_
#define _SOC_DEF_LPTMR_H_

#define LPTMR_CSR_TPS_VAL_CMP0				0u
#define LPTMR_CSR_TPS_VAL_ALT1				1u
#define LPTMR_CSR_TPS_VAL_ALT2				2u
#define LPTMR_CSR_TPS_VAL_ALT3				3u

#define LPTMR_PSR_PRS_VAL_D2				0u
#define LPTMR_PSR_PRS_VAL_D4_F2				1u
#define LPTMR_PSR_PRS_VAL_D8_F4				2u
#define LPTMR_PSR_PRS_VAL_D16_F8			3u
#define LPTMR_PSR_PRS_VAL_D32_F16			4u
#define LPTMR_PSR_PRS_VAL_D64_F32			5u
#define LPTMR_PSR_PRS_VAL_D128_F64			6u
#define LPTMR_PSR_PRS_VAL_D256_F128			7u
#define LPTMR_PSR_PRS_VAL_D512_F256			8u
#define LPTMR_PSR_PRS_VAL_D1K_F512			9u
#define LPTMR_PSR_PRS_VAL_D2K_F1K			10u
#define LPTMR_PSR_PRS_VAL_D4K_F2K			11u
#define LPTMR_PSR_PRS_VAL_D8K_F4K			12u
#define LPTMR_PSR_PRS_VAL_D16K_F8K			13u
#define LPTMR_PSR_PRS_VAL_D32K_F16K			14u
#define LPTMR_PSR_PRS_VAL_D64K_F32K			15u

#define LPTMR_PSR_PCS_VAL_MCGIRCLK			0u
#define LPTMR_PSR_PCS_VAL_LPOCLK			1u
#define LPTMR_PSR_PCS_VAL_ERCLK32K			2u
#define LPTMR_PSR_PCS_VAL_OSCERCLK			3u

#endif /* _SOC_DEF_LPTMR_H_ */
