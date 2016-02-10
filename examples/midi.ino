/*
 * NOTE: if you are using Arduino IDE - put keep this file in the same
 * directory with midi.c midi.h
 */
#include "midi.h"

static struct midi midi;

/**
 * Arduino Serial.write() wrapper for midi lib.
 * @param[in] data MIDI message
 * @param[in] size Size in bytes of MIDI message
 */
uint8_t
send(unsigned char *data, uint8_t size)
{
	return Serial.write(data, size);
}

void
setup(void)
{
	/* MIDI communicates at 31250 baud */
	Serial.begin(31250);

	/* initialize MIDI */
	midi_init(&midi, send);
}

void
loop(void)
{
	uint8_t channel = 2;
	uint8_t key = 0;
	uint8_t velocity = 1;

	/* send Note On, channel 3, key 0, velocity 1 */
	midi_send(&midi, MIDI_CMD_NOTE_ON, channel, &key, &velocity);

	/* send Control Change, channel 1, controller 12, value 64 */
	midi_send_control_change(&midi, 0, 12, 64);
	delay(100);

	delay(1000);
}
