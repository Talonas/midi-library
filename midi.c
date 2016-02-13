#include <stdint.h>
#include <string.h>

#include "midi.h"

static int validate_command(uint8_t command);


/**
 * Inits MIDI serial write functionality.
 * @param[in,out] midi MIDI structure to be initialized. Not NULL
 * @param[in] serial_write Pointer to serial write function. Not NULL
 * @return 0 on success
 * @return -1 on failure
 */
int
midi_init(struct midi *midi, uint8_t (*serial_write)(unsigned char *, uint8_t))
{
	int retval = -1;


	if (midi == NULL)
	{
		goto done;
	}

	if (serial_write == NULL)
	{
		goto done;
	}

	midi->serial_write = serial_write;

	retval = 0;
done:
	return retval;
}

/**
 * Makes and sends MIDI message with given arguments.
 * @param[in] command MIDI command
 * @param[in] channel Number of channel starting from 0
 * @param[in] param1 Parameter 1. Optional, can be NULL
 * @param[in] param2 Parameter 2. Optional, can be NULL
 * @return 0 on success
 * @return -1 on failure
 */
int
midi_send(struct midi *midi, uint8_t command, uint8_t channel,
	uint8_t *param1, uint8_t *param2)
{
	int retval = -1;
	int ret;
	uint8_t msg_size;
	unsigned char msg[3];


	if (midi == NULL)
	{
		goto done;
	}

	ret = midi_make_message(command, channel, param1, param2, msg, &msg_size);
	if (ret != 0)
	{
		goto done;
	}

	ret = midi_send_message(midi, msg, msg_size);
	if (ret != 0)
	{
		goto done;
	}

	retval = 0;
done:
	return retval;
}

/**
 * Sends single MIDI message. Message size must be from 1 to 3 bytes.
 * @param[in] msg MIDI message to be sent. Not NULL
 * @param[in] msg_size Midi message size of bytes. From 1 to 3
 * @return 0 on success
 * @return -1 on failure
 */
int
midi_send_message(struct midi *midi, unsigned char *msg, uint8_t msg_size)
{
	int retval = -1;
	int ret;


	if (midi == NULL)
	{
		goto done;
	}

	if (msg == NULL)
	{
		goto done;
	}

	if (msg_size == 0)
	{
		goto done;
	}

	if (msg_size > 3)
	{
		goto done;
	}

	if (midi->serial_write == NULL)
	{
		goto done;
	}

	ret = midi->serial_write(msg, msg_size);
	if (ret != (int)msg_size)
	{
		goto done;
	}

	retval = 0;
done:
	return retval;
}

/**
 * Sends "Note Off" MIDI message.
 * @param[in] channel Number of channel starting from 0
 * @param[in] key Note key
 * @param[in] velocity Note velocity
 * @return 0 on success
 * @return -1 on failure
 */
int
midi_send_note_off(struct midi *midi, uint8_t channel, uint8_t key,
	uint8_t velocity)
{
	return midi_send(midi, MIDI_CMD_NOTE_OFF, channel, &key, &velocity);
}

/**
 * Sends "Note On" MIDI message.
 * @param[in] channel Number of channel starting from 0
 * @param[in] key Note key
 * @param[in] velocity Note velocity
 * @return 0 on success
 * @return -1 on failure
 */
int
midi_send_note_on(struct midi *midi, uint8_t channel, uint8_t key,
	uint8_t velocity)
{
	return midi_send(midi, MIDI_CMD_NOTE_ON, channel, &key, &velocity);
}

/**
 * Sends "Message Aftertouch" MIDI message.
 * @param[in] channel Number of channel starting from 0
 * @param[in] key The key (note) number
 * @param[in] touch Pressure value
 * @return 0 on success
 * @return -1 on failure
 */
int
midi_send_aftertouch(struct midi *midi, uint8_t channel, uint8_t key,
	uint8_t touch)
{
	return midi_send(midi, MIDI_CMD_AFTERTOUCH, channel, &key, &touch);
}

/**
 * Sends "Control Change" MIDI message.
 * @param[in] channel Number of channel starting from 0
 * @param[in] controller Controller number
 * @param[in] value Controller value
 * @return 0 on success
 * @return -1 on failure
 */
int
midi_send_control_change(struct midi *midi, uint8_t channel,
	uint8_t controller, uint8_t value)
{
	return midi_send(midi, MIDI_CMD_CONTROL_CHANGE, channel, &controller,
		&value);
}

/**
 * Sends "Program Change" MIDI message.
 * @param[in] channel Number of channel starting from 0
 * @param[in] program_number Program number
 * @return 0 on success
 * @return -1 on failure
 */
int
midi_send_program_change(struct midi *midi, uint8_t channel,
	uint8_t program_number)
{
	return midi_send(midi, MIDI_CMD_PROGRAM_CHANGE, channel,
		&program_number, NULL);
}

/**
 * Sends "Pitch Bend" MIDI message.
 * @param[in] channel Number of channel starting from 0
 * @param[in] lsb Least significant bit
 * @param[in] msb Most significant bit
 * @return 0 on success
 * @return -1 on failure
 */
int
midi_send_pitch_bend(struct midi *midi, uint8_t channel, uint8_t lsb,
	uint8_t msb)
{
	return midi_send(midi, MIDI_CMD_PITCH_BEND, channel, &lsb, &msb);
}

/**
 * Makes MIDI message with given command, channel and parameters.
 * @param[in] command MIDI command
 * @param[in] channel Number of channel starting from 0
 * @param[in] param1 Parameter 1. Optional, can be NULL
 * @param[in] param2 Parameter 2. Optional, can be NULL
 * @param[out] msg_size Bytes length of generated MIDI message. Not NULL
 * @return 0 on success
 * @return -1 on failure
 */
int
midi_make_message(uint8_t command, uint8_t channel, uint8_t *param1,
	uint8_t *param2, unsigned char *msg, uint8_t *msg_size)
{
	int retval = -1;
	int ret;
	uint8_t bytes_count = 1;
	uint8_t first_byte;


	if (msg == NULL)
	{
		goto done;
	}

	if (msg_size == NULL)
	{
		goto done;
	}

	if (channel > 0xF)
	{
		goto done;
	}

	ret = validate_command(command);
	if (ret != 0)
	{
		goto done;
	}

	/* first command + channel byte */
	first_byte = command + channel;
	memcpy(msg, &first_byte, sizeof(first_byte));

	if (param1 != NULL)
	{
		/* param 1 byte */
		memcpy(msg + 1, param1, sizeof(*param1));
		bytes_count++;
	}

	if (param2 != NULL)
	{
		/* param 2 byte */
		memcpy(msg + 2, param2, sizeof(*param2));
		bytes_count++;
	}

	*msg_size = bytes_count;

	retval = 0;
done:
	return retval;
}

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
int
midi_make_message_note_off(uint8_t channel, uint8_t key, uint8_t velocity,
	unsigned char *msg, uint8_t *msg_size)
{
	return midi_make_message(MIDI_CMD_NOTE_OFF, channel, &key, &velocity,
		msg, msg_size);
}

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
int
midi_make_message_note_on(uint8_t channel, uint8_t key, uint8_t velocity,
	unsigned char *msg, uint8_t *msg_size)
{
	return midi_make_message(MIDI_CMD_NOTE_ON, channel, &key, &velocity,
		msg, msg_size);
}

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
int
midi_make_message_aftertouch(uint8_t channel, uint8_t key, uint8_t touch,
	unsigned char *msg, uint8_t *msg_size)
{
	return midi_make_message(MIDI_CMD_AFTERTOUCH, channel, &key, &touch,
		msg, msg_size);
}

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
int
midi_make_message_control_change(uint8_t channel, uint8_t controller,
	uint8_t value, unsigned char *msg, uint8_t *msg_size)
{
	return midi_make_message(MIDI_CMD_CONTROL_CHANGE, channel,
		&controller, &value, msg, msg_size);
}

/**
 * Makes "Program Change" MIDI message with given arguments.
 * @param[in] channel Number of channel starting from 0
 * @param[in] program_number Program number
 * @param[out] msg Generated MIDI message. Not NULL
 * @param[out] msg_size Bytes length of generated MIDI message. Not NULL
 * @return 0 on success
 * @return -1 on failure
 */
int
midi_make_message_program_change(uint8_t channel, uint8_t program_number,
	unsigned char *msg, uint8_t *msg_size)
{
	return midi_make_message(MIDI_CMD_PROGRAM_CHANGE, channel,
		&program_number, NULL, msg, msg_size);
}

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
int
midi_make_message_pitch_bend(uint8_t channel, uint8_t lsb, uint8_t msb,
	unsigned char *msg, uint8_t *msg_size)
{
	return midi_make_message(MIDI_CMD_PITCH_BEND, channel, &lsb, &msb,
		msg, msg_size);
}

/**
 * Checks if command value is valid MIDI command.
 * @param[in] command Command to be validate
 * @return 0 if valid
 * @return -1 if not valid
 */
static int
validate_command(uint8_t command)
{
	int retval = -1;


	switch (command)
	{
	case MIDI_CMD_NOTE_OFF:
	case MIDI_CMD_NOTE_ON:
	case MIDI_CMD_AFTERTOUCH:
	case MIDI_CMD_CONTROL_CHANGE:
	case MIDI_CMD_PROGRAM_CHANGE:
	case MIDI_CMD_CHANNEL_PRESSUER:
	case MIDI_CMD_PITCH_BEND:
	case MIDI_CMD_NON_MUSICAL:
		break;
	default:
		goto done;
	}

	retval = 0;
done:
	return retval;
}
