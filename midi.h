#ifndef INCLUDE_MokReqNmEeDQA8RQLUhNTubCZPD3c87u
#define INCLUDE_MokReqNmEeDQA8RQLUhNTubCZPD3c87u 1

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * MIDI message includes a status byte and up to two data bytes.
 *
 * COMMAND  MEANING                DATA BYTES   DATA BYTE 1     DATA BYTE 2
 * 0x8      Note Off               2            key             velocity
 * 0x9      Note On                2            key             velocity
 * 0xA      Aftertouch             2            key             touch
 * 0xB      Control Change         2            controller #    controller value
 * 0xC      Program Change         2            program #       -
 * 0xD      Channel Pressure       1            pressure        -
 * 0xE      Pitch Bend             2            lsb (7bits)     msb (7bits)
 * 0xF      non musical cmds       -            -               -
 *
 * For more information take a look at:
 * https://ccrma.stanford.edu/~craig/articles/linuxmidi/misc/essenmidi.html
 * https://www.cs.cf.ac.uk/Dave/Multimedia/node158.html
 *
 * MIDI Channels
 *
 * Channel can be set through MIDI message command. Maximum channel number is
 * 0xF. Channels starts from 0x0.
 * Example:
 *     Command 0xE0 where E is command "Pitch Bend" and 0 is channel 1.
 *     Command 0xB8 where B is command "Control Change" and 8 is channel 9.
 * 
 */


struct midi
{
	uint8_t (*serial_write)(unsigned char *, uint8_t);
};

/* Standard MIDI commands values */
enum midi_commands
{
	MIDI_CMD_NOTE_OFF = 0x80,
	MIDI_CMD_NOTE_ON = 0x90,
	MIDI_CMD_AFTERTOUCH = 0xA0,
	MIDI_CMD_CONTROL_CHANGE = 0xB0,
	MIDI_CMD_PROGRAM_CHANGE = 0xC0,
	MIDI_CMD_CHANNEL_PRESSUER = 0xD0,
	MIDI_CMD_PITCH_BEND = 0xE0,
	MIDI_CMD_NON_MUSICAL = 0xF0,
};


/**
 * Inits MIDI serial write functionality.
 * @param[in,out] midi MIDI structure to be initialized. Not NULL
 * @param[in] serial_write Pointer to serial write function. Not NULL
 * @return 0 on success
 * @return -1 on failure
 */
int midi_init(struct midi *midi, uint8_t (*serial_write)(unsigned char *,
	uint8_t));

/**
 * Makes and sends MIDI message with given arguments.
 * @param[in] command MIDI command
 * @param[in] channel Number of channel starting from 0
 * @param[in] param1 Parameter 1. Optional, can be NULL
 * @param[in] param2 Parameter 2. Optional, can be NULL
 * @return 0 on success
 * @return -1 on failure
 */
int midi_send(struct midi *midi, uint8_t command, uint8_t channel,
	uint8_t *param1, uint8_t *param2);

/**
 * Sends single MIDI message. Message size must be from 1 to 3 bytes.
 * @param[in] msg MIDI message to be sent. Not NULL
 * @param[in] msg_size Midi message size of bytes. From 1 to 3
 * @return 0 on success
 * @return -1 on failure
 */
int midi_send_message(struct midi *midi, unsigned char *msg, uint8_t msg_size);

/**
 * Sends "Note Off" MIDI message.
 * @param[in] channel Number of channel starting from 0
 * @param[in] key Note key
 * @param[in] velocity Note velocity
 * @return 0 on success
 * @return -1 on failure
 */
int midi_send_note_off(struct midi *midi, uint8_t channel, uint8_t key,
	uint8_t velocity);

/**
 * Sends "Note On" MIDI message.
 * @param[in] channel Number of channel starting from 0
 * @param[in] key Note key
 * @param[in] velocity Note velocity
 * @return 0 on success
 * @return -1 on failure
 */
int midi_send_note_on(struct midi *midi, uint8_t channel, uint8_t key,
	uint8_t velocity);

/**
 * Sends "Message Aftertouch" MIDI message.
 * @param[in] channel Number of channel starting from 0
 * @param[in] key The key (note) number
 * @param[in] touch Pressure value
 * @return 0 on success
 * @return -1 on failure
 */
int midi_send_aftertouch(struct midi *midi, uint8_t channel, uint8_t key,
	uint8_t touch);

/**
 * Sends "Control Change" MIDI message.
 * @param[in] channel Number of channel starting from 0
 * @param[in] controller Controller number
 * @param[in] value Controller value
 * @return 0 on success
 * @return -1 on failure
 */
int midi_send_control_change(struct midi *midi, uint8_t channel,
	uint8_t controller, uint8_t value);

/**
 * Sends "Program Change" MIDI message.
 * @param[in] channel Number of channel starting from 0
 * @param[in] program_number Program number
 * @return 0 on success
 * @return -1 on failure
 */
int midi_send_program_change(struct midi *midi, uint8_t channel,
	uint8_t program_number);

/**
 * Sends "Pitch Bend" MIDI message.
 * @param[in] channel Number of channel starting from 0
 * @param[in] lsb Least significant bit
 * @param[in] msb Most significant bit
 * @return 0 on success
 * @return -1 on failure
 */
int midi_send_pitch_bend(struct midi *midi, uint8_t channel, uint8_t lsb,
	uint8_t msb);

/**
 * Makes MIDI message with given command, channel and parameters.
 * @param[in] command MIDI command
 * @param[in] channel Number of channel starting from 0
 * @param[in] param1 Parameter 1. Optional, can be NULL
 * @param[in] param2 Parameter 2. Optional, can be NULL
 * @param[out] msg Generated MIDI message. Not NULL
 * @param[out] msg_size Bytes length of generated MIDI message. Not NULL
 * @return 0 on success
 * @return -1 on failure
 */
int midi_make_message(uint8_t command, uint8_t channel, uint8_t *param1,
	uint8_t *param2, unsigned char *msg, uint8_t *msg_size);

/**
 * Makes "Note Off" MIDI message with given arguments.
 * @param[in] channel Number of channel starting from 0
 * @param[in] key Note key
 * @param[in] velocity Note velocity
 * @param[out] msg Generated MIDI message. Not NULL
 * @param[out] msg_size Bytes length of generated MIDI message. Not NULL
 * @return 0 on success
 * @return -1 on failure
 */
int midi_make_message_note_off(uint8_t channel, uint8_t key, uint8_t velocity,
	unsigned char *msg, uint8_t *msg_size);

/**
 * Makes "Note On" MIDI message with given arguments.
 * @param[in] channel Number of channel starting from 0
 * @param[in] key Note key
 * @param[in] velocity Note velocity
 * @param[out] msg Generated MIDI message. Not NULL
 * @param[out] msg_size Bytes length of generated MIDI message. Not NULL
 * @return 0 on success
 * @return -1 on failure
 */
int midi_make_message_note_on(uint8_t channel, uint8_t key, uint8_t velocity,
	unsigned char *msg, uint8_t *msg_size);

/**
 * Makes "Message Aftertouch" MIDI message with given arguments.
 * @param[in] channel Number of channel starting from 0
 * @param[in] key The key (note) number
 * @param[in] touch Pressure value
 * @param[out] msg Generated MIDI message. Not NULL
 * @param[out] msg_size Bytes length of generated MIDI message. Not NULL
 * @return 0 on success
 * @return -1 on failure
 */
int midi_make_message_aftertouch(uint8_t channel, uint8_t key, uint8_t touch,
	unsigned char *msg, uint8_t *msg_size);

/**
 * Makes "Control Change" MIDI message with given arguments.
 * @param[in] channel Number of channel starting from 0
 * @param[in] controller Controller number
 * @param[in] value Controller value
 * @param[out] msg Generated MIDI message. Not NULL
 * @param[out] msg_size Bytes length of generated MIDI message. Not NULL
 * @return 0 on success
 * @return -1 on failure
 */
int midi_make_message_control_change(uint8_t channel, uint8_t controller,
	uint8_t value, unsigned char *msg, uint8_t *msg_size);

/**
 * Makes "Program Change" MIDI message with given arguments.
 * @param[in] channel Number of channel starting from 0
 * @param[in] program_number Program number
 * @param[out] msg Generated MIDI message. Not NULL
 * @param[out] msg_size Bytes length of generated MIDI message. Not NULL
 * @return 0 on success
 * @return -1 on failure
 */
int midi_make_message_program_change(uint8_t channel, uint8_t program_number,
	unsigned char *msg, uint8_t *msg_size);

/**
 * Makes "Pitch Bend" MIDI message with given arguments.
 * @param[in] channel Number of channel starting from 0
 * @param[in] lsb Least significant bit
 * @param[in] msb Most significant bit
 * @param[out] msg Generated MIDI message. Not NULL
 * @param[out] msg_size Bytes length of generated MIDI message. Not NULL
 * @return 0 on success
 * @return -1 on failure
 */
int midi_make_message_pitch_bend(uint8_t channel, uint8_t lsb, uint8_t msb,
	unsigned char *msg, uint8_t *msg_size);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_MokReqNmEeDQA8RQLUhNTubCZPD3c87u */
