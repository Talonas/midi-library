#include "midi.h"

static struct midi midi;

/*
 * The Triple Delay supports MIDI continuous controllers (CC's) for remotely
 * bypassing / enabling delay engine 1, 2 and 3:
 * CC# 12 - delay1
 * CC# 13 - delay2
 * CC# 14 - delay3
 */
static uint8_t triple_delay[3] = {12, 13, 14};
/* CC Value 0-63 turns them on */
static uint8_t triple_delay_value_on = 0;
/* CC Value 64-127 turns them off */
static uint8_t triple_delay_value_off = 64;
/*
 * The Flashback Triple Delay receives Midi continuous controller (CC's)
 * data on Midi channel 1 only.
 */
static uint8_t triple_delay_channel = 0;

/*
 * The Nova Drive MIDI controllable parameters:
 * CC# 21 - overdrive ON/OFF (0-63=OFF, 64-127=ON)
 * CC# 22 - distortion ON/OFF (0-63=OFF, 64-127=ON)
 */
static uint8_t nova_drive[2] = {21, 22};
/* CC Value 64-127 turns them on */
static uint8_t nova_drive_value_on = 64;
/* CC Value 0-63 turns them off */
static uint8_t nova_drive_value_off = 0;
/* MIDI channel can be configurable. In my configuration - MIDI channel is 2 */
static uint8_t nova_drive_channel = 1;

static uint8_t pedals_status = 0;

static void triple_delay_change_state(uint8_t status);
static void nova_drive_change_state(uint8_t status);


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

	/* init midi serial write */
	midi_init(&midi, send);
}

void
loop(void)
{
	triple_delay_change_state(pedals_status);
	nova_drive_change_state(pedals_status);

	pedals_status ^= 1;
}

/**
 * Toggles Triple Delay state.
 */
static void
triple_delay_change_state(uint8_t status)
{
	int i;


	if (status == 0)
	{
		status = triple_delay_value_on;
	}
	else
	{
		status = triple_delay_value_off;
	}

	for (i = 0; i < 3; i++)
	{
		midi_send_control_change(&midi, triple_delay_channel,
			triple_delay[i], status);
		delay(500);
	}
}

/**
 * Toggles Nova Drive state.
 */
static void
nova_drive_change_state(uint8_t status)
{
	int i;


	if (status == 0)
	{
		status = nova_drive_value_on;
	}
	else
	{
		status = nova_drive_value_off;
	}

	for (i = 0; i < 2; i++)
	{
		midi_send_control_change(&midi, nova_drive_channel,
			nova_drive[i], status);
		delay(500);
	}
}
