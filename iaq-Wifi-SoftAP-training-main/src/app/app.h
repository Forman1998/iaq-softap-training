#ifndef MAIN_H_
#define MAIN_H_

#include "r_smc_entry.h"
#include "display/driver/st7735s.h"
#include "display/text/text.h"

#define MAXIMUM_SYSTEM_STATES	(3)

/** @brief sets a hardware event flag*/
#define HW_SET_EVENT(flags,event)	{flags |= event;}
/** @brief clears a hardware event flag*/
#define HW_CLEAR_EVENT(flags,event)	{flags &= ~event;}
/** @brief Checks if hardware event flag is set*/
#define HW_EVENT_OCCURRED(flags,event)	( ((flags) & (event)) == event )

/** @brief Configuration macro for state text X location*/
#define TEXT_X_POSITION		(65U)
/** @brief Configuration macro for state text Y location*/
#define TEXT_Y_POSITION		(20U)

/** @brief macro used to prepare for disabling interrupts*/
#define PREPARE_CRITICAL_SECTION() uint8_t l_int_status = Interrupts_enabled()
/** @brief macro used to disable interrupts*/
#define ENTER_CRITICAL_SECTION()	DI()
/** @brief macro used to enable interrupts*/
#define EXIT_CRITICAL_SECTION()	if(l_int_status == 1U) { EI(); }

/** @brief Enumerated type for hardware event flags*/
typedef enum
{
	NO_EVENT				=	0x0000U,
	BUTTON_CLICK 			=	0x0001U,
	ROTARY_COUNT_UPDATED 	=	0x0002U,
	ALL_HARDWARE_EVENTS		=	0x0003U,
}hardware_event_t;

/** Colours*/
static const uint8_t COLOUR_BLACK[] = {0x00, 0x00};
static const uint8_t COLOUR_WHITE[] = {0xFF, 0xFF};

/* Display area data*/
extern ST7735S_display_area_info_t disp_info;
/* Array of titles for each state*/
extern const char * titles[];
/* System States*/
extern int16_t system_output_state;
extern int16_t prev_system_output_state;
extern volatile int16_t rotary_count;
extern volatile hardware_event_t hw_event_flags;

/** @brief turn on the backlight*/
static inline void Hw_enable_backlight(void)
{
	CCDE |= 0x40U;
	CCS6 = 0x02U; /* 5mA backlight*/
	P6_bit.no2 = 0U;
}
/* END OF FUNCTION*/

/** @brief turn on the backlight*/
static inline void Hw_disable_backlight(void)
{
	P6_bit.no2 = 1U;
	CCS6 = 0x00U; /* Hi-Z backlight*/
	CCDE &= ~0x40U;
}
/* END OF FUNCTION*/

/** @brief Inline assembly function to return interrupt enabled state
 * @return 1 = enabled, 0 otherwise.
 */
#pragma inline_asm Interrupts_enabled
static uint8_t Interrupts_enabled(void)
{
	MOV 	A, PSW
	SHR		A, 7
}
/* END OF FUNCTION*/

/** @brief return hw rotary count
 * @return hw rotary count*/
static inline int16_t Hw_get_rotary_count(void)
{
	int16_t l_rot_count = 0;
	PREPARE_CRITICAL_SECTION();

	ENTER_CRITICAL_SECTION();

	l_rot_count = rotary_count % MAXIMUM_SYSTEM_STATES; /* Apply maximum range on rotary count*/
	rotary_count = 0;

	EXIT_CRITICAL_SECTION();

	return l_rot_count;
}
/* END OF FUNCTION*/

/** @brief Copies data into RAMSD safe section, initialises the display & starts drivers*/
void App_init(void);

/** @brief function to update display*/
void App_update_display(void);

/** @brief checks for a button click event
 * @return true if event detected, false otherwise.
 */
bool App_button_click_event(void);

/** @brief checks for a rotary encoder event
 * @return true if event detected, false otherwise.
 */
bool App_rotary_event(void);

#endif
