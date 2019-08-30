#pragma once
#include <stm32f4xx_hal.h>
#include "Shared.h"
//TODO: This may not be needed any more
#include <functional>

class Usart {
protected :
	static void transmit_byte(const UART_HandleTypeDef&, uint8_t);
};

//Used for MIDI input
class Usart_1 : public Usart {
	//For HAL
	static UART_HandleTypeDef huart1;
public:
	static void init();
	static void transmit_byte(uint8_t);	
	static uint32_t read_dr();
	static bool is_flag_set(const uint32_t flag);
	static void receive(uint8_t* buffer_ptr, uint16_t size); 
};


/* Usart2 is connected to the virtual com port
 * So this class is used to send messages that can be 
 * seen on a PC terminal*/

class Usart_2 : public Usart {
	static UART_HandleTypeDef huart2;
public:
	static void init();
	static void transmit_byte(uint8_t);	
	static uint32_t read_dr();
	static bool is_flag_set(const uint32_t flag);
};

//FSM for midi messages
//There are normally 3 bytes for each message, they need to be handled asyncrounously
//See https://www.songstuff.com/recording/article/midi_message_format/

enum Midi_state { wait_status_byte, wait_note_number, wait_velocity, wait_controller_number, wait_controller_data };

class Midi_in : public Usart_1 {
	//Current FSM state state
	static Midi_state state;
	//If a Note On midi byte is recieved then this struct is filled with the note data
	//It is sent to the approiate handler when filled
	static Note_on_struct note_on_struct;
	//As above but for a controller change
	static Controller_change_struct controller_change_struct;
	static Midi_state get_next_state_from_status_byte(uint8_t);
	static bool is_status_byte(uint8_t);
public:
	//Process byte from UART
	static void handle_midi_byte(uint8_t midi_byte, void(*note_on_handler)(Note_on_struct), void(*controller_change_handler)(Controller_change_struct));
};


