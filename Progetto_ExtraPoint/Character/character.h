#define POS_FIRST_BATTERY 30
#define POS_SECOND_BATTERY 150
#define POS_BATTERY_Y 40
#define WIDTH_BATTERY 60
#define HEIGHT_BATTERY 35
#define OFFSET 9
#define WIDTH_CELL 7
#define HEIGHT_CELL 19
#define BLOCK_WIDTH 7
#define BLOCK_HEIGHT 7
#define BLOCK_WIDTH_SMALL 3
#define BLOCK_HEIGHT_SMALL 3
#define POS_BURGER_X 5
#define POS_BURGER_Y 225
#define POS_CANDY_X 195
#define POS_CANDY_Y 230
/*
	SHARED VARIABLES:
	- battery_cell_1 : current number of battery cell that happiness battery has.
	- battery_cell_2 : current number of battery cell that satiety battery has.
	- character_x : current x position of the character.
	- character_y : current y position of the character.
	- frame : current frame related to the animation of the character.
	- is_shifting : current status of the character (0 = is not shifting, 1 = is shifting) to avoid flipping while shifting.
	- is_flipping : current status of the character (0 = is not flipping, 1 = is flipping) to avoid shifting while flipping.
	- joystick_selection : current option selected with the joystick.
	- selection_disabled : current status of the selection (0 = selection enabled, 1 = selection disabled).
	- joystick_disabled : current value that disables the jostick input (1 = disabled, 0 = not disabled).
	- reset : current status of the tamagotchi (0 = is not in the reset phase, 1 = is in the reset phase).
	- select_pressed : to give priority to user action and register if the user pressed select while flipping, in order to execute the action soon after the flip.
	- time_elapsed : current age of the tamagotchi.
	- is_animating : variable to handle the animation (1 = animation in progress, 0 = no animation).
	- volume : variable to handle the volume.
*/
extern uint8_t battery_cell_1;
extern uint8_t battery_cell_2;
extern uint16_t character_x;
extern uint16_t character_y;
extern uint8_t frame;
extern uint8_t is_shifting;
extern uint8_t is_flipping;
extern uint8_t joystick_selection;
extern uint8_t selection_disabled;
extern uint8_t joystick_disabled;
extern uint8_t reset;
extern uint8_t select_pressed;
extern int time_elapsed;
extern uint8_t is_animating;
extern int volume;

/*
		function to shift to sx.
		shift the character to sx by 3 blocks two times
		frame: frame to consider in order to perform the shift (0 or 1)
		x_old,y_old: coordinate from which move to sx
*/
void shift_character_sx(uint8_t frame, uint8_t x_old, uint8_t y_old);

/*
		function to shift to dx.
		shift the character to dx by 3 blocks two times
		frame: frame to consider in order to perform the shift (0 or 1)
		x_old,y_old: coordinate from which move to dx
*/
void shift_character_dx(uint8_t frame, uint8_t x_old, uint8_t y_old);

/*
		function to charge the batteries.
		battery: battery to consider in order to charge it (0 or 1)
		num_cells: number of cells to add 
*/
void draw_battery_charge(uint8_t battery, uint8_t num_cells);

/*
		function to discharge the batteries.
		battery: battery to consider in order to discharge it (0 or 1)
		num_cells: number of cells to remove 
*/
void draw_battery_discharge(uint8_t battery, uint8_t num_cells);

/*
		function to draw the character
		x,y: coordinates from starting to draw it
		color: which color to draw it
*/
void draw_character(uint8_t x, uint8_t y, uint16_t color);

/*
		function to draw the candy
		x,y: coordinates from starting to draw it
		color: color to draw
*/
void draw_candy(uint8_t x, uint8_t y, uint16_t color);

/*
		function to draw the burger
		x,y: coordinates from starting to draw it
		color: color to draw
*/
void draw_burger(uint8_t x, uint8_t y, uint16_t color);
	
// 0 from first pose to change, 1 from change pose to first
/*
	function to animate the character
	direction: frame to consider when starting (0 from first pose to change, 1 from change pose to first)
	x,y: coordinates from starting to change it
*/
void change_character(uint8_t direction,uint8_t x, uint8_t y);

/*
	function to clear the pixel related to the second frame in case of reset
	x,y: coordinates from starting to clear it
*/
void clear_changing(uint8_t x, uint8_t y);

// Function that initializes the gui at the initial state 
void draw_structure_and_border(void);

/*
	function to update the current age of the tamagotchi
	hours: current age hours to update on the screen
	minutes: current age minutes to update on the screen
	seconds: current age seconds to update on the screen
*/
void update_age(uint8_t hours,uint8_t minutes, uint8_t seconds);

/*
	function that color the lines of the selected option 
	option: which option color
	color: which color use to do it
*/
void draw_options(int option, uint16_t color);

/*
	function to reset the tamagotchi in the original state when select button of joystick is pressed
*/
void reset_pressed(void);

/*
	function for eating the burger
*/
void eat_burger(void);

/*
	function for eating the candy
*/
void eat_candy(void);

/*
	function for drawing the heart
*/
void draw_heart(uint8_t x, uint8_t y, uint16_t color);

/*
	function for drawing the speaker
*/
void draw_speaker(uint8_t x, uint8_t y, uint16_t color);

/*
	function for drawing the sound
*/
void draw_sound(uint8_t x, uint8_t y, uint16_t color);

/*
	function for drawing the cross
*/
void draw_cross(uint8_t x, uint8_t y, uint16_t color);
