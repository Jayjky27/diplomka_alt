/*
 *==============================================================================
 * Name: soc_def_smc.h
 * Author: Martin Stankus
 *==============================================================================
 */

#ifndef _SOC_DEF_SMC_H_
#define _SOC_DEF_SMC_H_

#define SMC_PMCTRL_RUNM_VAL_RUN			0u
#define SMC_PMCTRL_RUNM_VAL_VLPR		2u

#define SMC_PMCTRL_STOPM_VAL_STOP		0u
#define SMC_PMCTRL_STOPM_VAL_VLPS		2u
#define SMC_PMCTRL_STOPM_VAL_LLS		3u
#define SMC_PMCTRL_STOPM_VAL_VLLS		4u

#define SMC_PMSTAT_VAL_RUN				0x01u
#define SMC_PMSTAT_VAL_STOP				0x02u
#define SMC_PMSTAT_VAL_VLPR				0x04u
#define SMC_PMSTAT_VAL_VLPW				0x08u
#define SMC_PMSTAT_VAL_VLPS				0x10u
#define SMC_PMSTAT_VAL_LLS				0x20u
#define SMC_PMSTAT_VAL_VLLS				0x40u

#endif /* _SOC_DEF_SMC_H_ */
