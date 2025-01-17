/*
 * Copyright (c) 2023 Nuvoton Technology Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NUVOTON_NPCM4XX_SOC_CLOCK_H_
#define _NUVOTON_NPCM4XX_SOC_CLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Common clock control device name for all NPCM4XX series */
#define NPCM4XX_CLK_CTRL_NAME DT_LABEL(DT_NODELABEL(pcc))

/**
 * @brief NPCM4XX clock configuration structure
 *
 * Used to indicate the device's clock bus type and corresponding PWDWN_CTL
 * register/bit to turn on/off its source clock.
 */
struct npcm4xx_clk_cfg {
	uint16_t bus:8;
	uint16_t ctrl:5;
	uint16_t bit:3;
};

/* Clock settings from pcc node */
/* Target OFMCLK freq */
#define OFMCLK DT_PROP(DT_NODELABEL(pcc), clock_frequency)
/* Core clock prescaler */
#define FPRED_VAL (DT_PROP(DT_NODELABEL(pcc), core_prescaler) - 1)
/* APB1 clock divider */
#define APB1DIV_VAL (DT_PROP(DT_NODELABEL(pcc), apb1_prescaler) - 1)
/* APB2 clock divider */
#define APB2DIV_VAL (DT_PROP(DT_NODELABEL(pcc), apb2_prescaler) - 1)
/* APB3 clock divider */
#define APB3DIV_VAL (DT_PROP(DT_NODELABEL(pcc), apb3_prescaler) - 1)
/* APB4 clock divider if supported */
#if DT_NODE_HAS_PROP(DT_NODELABEL(pcc), apb4_prescaler)
#define APB4DIV_VAL (DT_PROP(DT_NODELABEL(pcc), apb4_prescaler) - 1)
#endif

/*
 * NPCM4XX and later series clock tree macros:
 * (Please refer Figure 58. for more information.)
 *
 * Suggestion:
 * - OSC_CLK > 50MHz, XF_RANGE should be 1, else 0.
 * - CORE_CLK > 50MHz, AHB6DIV should be 1, else 0.
 * - CORE_CLK > 50MHz, FIUDIV should be 1, else 0.
 */

/* Core domain clock */
#define CORE_CLK (OFMCLK / DT_PROP(DT_NODELABEL(pcc), core_prescaler))
/* Low Frequency clock */
#define LFCLK     32768

/* FMUL clock */
#if (OFMCLK > 50000000)
#define FMCLK (OFMCLK / 2) /* FMUL clock = OFMCLK/2 if OFMCLK > 50MHz */
#else
#define FMCLK OFMCLK /* FMUL clock = OFMCLK */
#endif

/* APBs source clock */
#define APBSRC_CLK OFMCLK

/* AHB6 clock */
#if (CORE_CLK > 50000000)
#define AHB6DIV_VAL 1 /* AHB6_CLK = CORE_CLK/2 if CORE_CLK > 50MHz */
#else
#define AHB6DIV_VAL 0 /* AHB6_CLK = CORE_CLK */
#endif
/* FIU clock divider */
#if (CORE_CLK > 50000000)
#define FIUDIV_VAL 1 /* FIU_CLK = CORE_CLK/2 */
#else
#define FIUDIV_VAL 0 /* FIU_CLK = CORE_CLK */
#endif

/* Get APB clock freq */
#define NPCM4XX_APB_CLOCK(no) (APBSRC_CLK / (APB##no##DIV_VAL + 1))

/*
 * Frequency multiplier M/N value definitions according to the requested
 * OFMCLK (Unit:Hz).
 */
#if (OFMCLK > 50000000)
#define HFCGN_VAL    0x82 /* Set XF_RANGE as 1 if OFMCLK > 50MHz */
#else
#define HFCGN_VAL    0x02
#endif
#if   (OFMCLK == 100000000)
#define HFCGMH_VAL   0x0B
#define HFCGML_VAL   0xEC
#elif (OFMCLK == 96000000)
#define HFCGMH_VAL   0x0B
#define HFCGML_VAL   0x72
#elif (OFMCLK == 90000000)
#define HFCGMH_VAL   0x0A
#define HFCGML_VAL   0xBA
#elif (OFMCLK == 80000000)
#define HFCGMH_VAL   0x09
#define HFCGML_VAL   0x89
#elif (OFMCLK == 66000000)
#define HFCGMH_VAL   0x07
#define HFCGML_VAL   0xDE
#elif (OFMCLK == 50000000)
#define HFCGMH_VAL   0x0B
#define HFCGML_VAL   0xEC
#elif (OFMCLK == 48000000)
#define HFCGMH_VAL   0x0B
#define HFCGML_VAL   0x72
#elif (OFMCLK == 40000000)
#define HFCGMH_VAL   0x09
#define HFCGML_VAL   0x89
#elif (OFMCLK == 33000000)
#define HFCGMH_VAL   0x07
#define HFCGML_VAL   0xDE
#else
#error "Unsupported OFMCLK Frequency"
#endif

/**
 * @brief Function to notify clock driver that backup the counter value of
 *        low-frequency timer before ec entered deep idle state.
 */
void npcm4xx_clock_capture_low_freq_timer(void);

/**
 * @brief Function to notify clock driver that compensate the counter value of
 *        system timer by low-frequency timer after ec left deep idle state.
 *
 */
void npcm4xx_clock_compensate_system_timer(void);

/**
 * @brief Function to get time ticks in system sleep/deep sleep state. The unit
 *        is ticks.
 *
 */
uint64_t npcm4xx_clock_get_sleep_ticks(void);

/**
 * @brief Function to configure system sleep settings. After ec received "wfi"
 *        instruction, ec will enter sleep/deep sleep state for better power
 *        consumption.
 *
 * @param is_deep A boolean indicating ec enters deep sleep or sleep state
 * @param is_instant A boolean indicating 'Instant Wake-up' from deep idle is
 *                   enabled
 */
void npcm4xx_clock_control_turn_on_system_sleep(bool is_deep, bool is_instant);

/**
 * @brief Function to turn off system sleep mode.
 */
void npcm4xx_clock_control_turn_off_system_sleep(void);

#ifdef __cplusplus
}
#endif

#endif /* _NUVOTON_NPCM4XX_SOC_CLOCK_H_ */
