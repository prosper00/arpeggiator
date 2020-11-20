# Configure according to your device config. 
# Find the available 'DEVICE' FQBN by running 'arduino-cli board listall'
# find the settable (comma-seperated) DEVICE_OPTIONS available to be set by 'arduino-cli board details --fqbn <FQBN>'
# If you want to use an external programmer, specify PROGRAMMER=-P <programmer> (where eligible programmers are found from the board details command noted above
PORT=/dev/ttyACM0
DEVICE=LGT8fx\ Boards:avr:328
DEVICE_OPTIONS=:clock=32MHz,variant=modelP
PROGRAMMER=

# Specify the path in the CC= line if it's not in your $PATH
CC=arduino-cli
FQBN=$(DEVICE)$(DEVICE_OPTIONS)
CFLAGS=compile --fqbn $(FQBN) -v
FLASH=-p $(PORT) $(PROGRAMMER) -u

all:
	$(CC) $(CFLAGS)
	$(CC) board details $(FQBN)

flash:
	$(CC) $(CFLAGS) $(FLASH)
	$(CC) board details $(FQBN)

clean:
	rm -rf build
	rm -rf /tmp/arduino*
