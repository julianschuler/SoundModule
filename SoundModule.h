#ifndef __SoundModule__
#define __SoundModule__

#include <util/delay_basic.h>

#define BAUD_RATE				9600
#define CYCLE_OVERHEAD			3
#define TX_DELAY				(((F_CPU)/(BAUD_RATE)/4) - CYCLE_OVERHEAD)


class SoundModule {
private:
	uint8_t txBitMask;
	volatile uint8_t* txPortRegister;
	
	
	void sendCommand(uint8_t cmd, uint8_t paraml=0x00, uint8_t paramh = 0x00) {
		uint16_t checksum = 0xFEFB - cmd - paramh - paraml;
		uint8_t buf[] = {0x7E, 0xFF, 0x06, cmd, 0x00, paramh, paraml, highByte(checksum), lowByte(checksum), 0xEF};
		
		volatile uint8_t *reg = txPortRegister;
		uint8_t reg_mask = txBitMask;
		uint8_t inv_mask = ~txBitMask;
		uint8_t oldSREG	= SREG;
		
		for (uint8_t i = 0; i < 10; i++) {
			uint8_t b = buf[i];
			cli();
			*reg &= inv_mask;
			_delay_loop_2(TX_DELAY);
			for (uint8_t j = 0; j < 8; j++) {
				if (b & 1) {
					*reg |= reg_mask;
				}
				else {
					*reg &= inv_mask;
				}
				_delay_loop_2(TX_DELAY);
				b >>= 1;
			}
			*reg |= reg_mask;
			SREG = oldSREG;
			_delay_loop_2(TX_DELAY);
		}
	}
	
	
public:
	SoundModule(uint8_t txPin) {
		digitalWrite(txPin, HIGH);
		pinMode(txPin, OUTPUT);
		txBitMask = digitalPinToBitMask(txPin);
		txPortRegister = portOutputRegister(digitalPinToPort(txPin));
	}
	
	
	void playFile(uint8_t file) {
		sendCommand(0x03, file);
	}
	
	
	void loopFile(uint8_t file) {
		sendCommand(0x08, file);
	}
	
	
	void pause() {
		sendCommand(0x0E);
	}
	
	
	void setVolume(uint8_t volume) {
		sendCommand(0x06, volume);
	}
};


#undef BAUD_RATE
#undef CYCLE_OVERHEAD
#undef TX_DELAY

#endif
