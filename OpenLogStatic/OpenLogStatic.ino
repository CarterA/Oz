/*
 12-3-09
 Nathan Seidle
 SparkFun Electronics 2012
 Modified by Carter Allen
 */

#include <SdFat.h> //We do not use the built-in SD.h file because it calls Serial.print
#include <SerialPort.h> //This is a new/beta library written by Bill Greiman. You rock Bill!
#include <EEPROM.h>

SerialPort<0, 600, 0> NewSerial;
//This is a very important buffer declaration. This sets the <port #, rx size, tx size>. We set
//the TX buffer to zero because we will be spending most of our time needing to buffer the incoming (RX) characters.
//900 works on minimal implementation, doesn't work with the full command prompt
//800 doesn't work with full command prompt
//700 works very well at 115200
//600 works well at 115200
//500

#include <avr/sleep.h>
#include <avr/power.h> //Needed for powering down perihperals such as the ADC/TWI and Timers

//Debug turns on (1) or off (0) a bunch of verbose debug statements. Normally use (0)
//#define DEBUG  1
#define DEBUG 0

//The bigger the receive buffer, the less likely we are to drop characters at high speed. However, the ATmega has a limited amount of
//RAM. This debug mode allows us to view available RAM at various stages of the program
//#define RAM_TESTING  1 //On
#define RAM_TESTING  0 //Off

//Internal EEPROM locations for the user settings
#define LOCATION_FILE_NUMBER_LSB	0x03
#define LOCATION_FILE_NUMBER_MSB	0x04

const uint32_t BAUD =    9600;
#define LOCAL_BUFF_SIZE  32
#define MAX_LOOPS        37 // BAUD/8/LOCAL_BUFF_SIZE

//STAT1 is a general LED and indicates serial traffic
#define STAT1  5 //On PORTD
#define STAT1_PORT  PORTD
#define STAT2  5 //On PORTB
#define STAT2_PORT  PORTB
const uint8_t statled1 = 5;  //This is the normal status LED
const uint8_t statled2 = 13; //This is the SPI LED, indicating SD traffic

//Blinking LED error codes
#define ERROR_SD_INIT	3

#define OFF   0x00
#define ON    0x01

Sd2Card card;
SdVolume volume;
SdFile currentDirectory;
SdFile file;

//Store error strings in flash to save RAM
void error(const char *str) {
  NewSerial.print(F("error: "));
  NewSerial.println(str);

  if (card.errorCode()) {
    NewSerial.print(F("SD error: "));
    NewSerial.print(card.errorCode(), HEX);
    NewSerial.print(',');
    NewSerial.println(card.errorData(), HEX);
  }

  while(1);
}

void setup(void)
{
  pinMode(statled1, OUTPUT);

  //Power down various bits of hardware to lower power usage  
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();

  //Shut off TWI, Timer2, Timer1, ADC
  ADCSRA &= ~(1<<ADEN); //Disable ADC
  ACSR = (1<<ACD); //Disable the analog comparator
  DIDR0 = 0x3F; //Disable digital input buffers on all ADC0-ADC5 pins
  DIDR1 = (1<<AIN1D)|(1<<AIN0D); //Disable digital input buffer on AIN1/0

  power_twi_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_adc_disable();

  //Setup UART
  NewSerial.begin(BAUD);
  NewSerial.print("1");

  //Setup SD & FAT
  if (!card.init(SPI_FULL_SPEED)) {
    NewSerial.print(F("error card.init")); 
    blink_error(ERROR_SD_INIT);
  }
  if (!volume.init(&card)) {
    NewSerial.print(F("error volume.init")); 
    blink_error(ERROR_SD_INIT);
  }
  if (!currentDirectory.openRoot(&volume)) {
    NewSerial.print(F("error openRoot")); 
    blink_error(ERROR_SD_INIT);
  }

  NewSerial.print("2");

}

void loop(void)
{
  newlog();
  while(1); //We should never get this far
}

//Log to a new file everytime the system boots
//Checks the spots in EEPROM for the next available LOG# file name
//Updates EEPROM and then appends to the new log file.
//Limited to 65535 files but this should not always be the case.
void newlog(void)
{
  uint8_t msb, lsb;
  uint16_t new_file_number;

  //Combine two 8-bit EEPROM spots into one 16-bit number
  lsb = EEPROM.read(LOCATION_FILE_NUMBER_LSB);
  msb = EEPROM.read(LOCATION_FILE_NUMBER_MSB);

  new_file_number = msb;
  new_file_number = new_file_number << 8;
  new_file_number |= lsb;

  //If both EEPROM spots are 255 (0xFF), that means they are un-initialized (first time OpenLog has been turned on)
  //Let's init them both to 0
  if((lsb == 255) && (msb == 255))
  {
    new_file_number = 0; //By default, unit will start at file number zero
    EEPROM.write(LOCATION_FILE_NUMBER_LSB, 0x00);
    EEPROM.write(LOCATION_FILE_NUMBER_MSB, 0x00);
  }

  //The above code looks like it will forever loop if we ever create 65535 logs
  //Let's quit if we ever get to 65534
  //65534 logs is quite possible if you have a system with lots of power on/off cycles
  if(new_file_number == 65534)
  {
    //Gracefully drop out to command prompt with some error
    NewSerial.print(F("!Too many logs:1!"));
    return; //Bail!
  }

  //If we made it this far, everything looks good - let's start testing to see if our file number is the next available

  //Search for next available log spot
  char new_file_name[] = "LOG00000.TXT";
  while(1)
  {
    new_file_number++;
    if(new_file_number > 65533) //There is a max of 65534 logs
    {
      NewSerial.print(F("!Too many logs:2!"));
      return;
    }

    sprintf(new_file_name, "LOG%05d.TXT", new_file_number); //Splice the new file number into this file name

    //Try to open file, if fail (file doesn't exist), then break
    if (file.open(&currentDirectory, new_file_name, O_CREAT | O_EXCL | O_WRITE)) break;
  }
  file.close(); //Close this new file we just opened
  //file.writeError = false; // clear any write errors

  //Record new_file number to EEPROM
  lsb = (uint8_t)(new_file_number & 0x00FF);
  msb = (uint8_t)((new_file_number & 0xFF00) >> 8);

  EEPROM.write(LOCATION_FILE_NUMBER_LSB, lsb); // LSB

  if (EEPROM.read(LOCATION_FILE_NUMBER_MSB) != msb)
    EEPROM.write(LOCATION_FILE_NUMBER_MSB, msb); // MSB

#if DEBUG
  NewSerial.print(F("\nCreated new file: "));
  NewSerial.println(new_file_name);
#endif

  //Begin writing to file
  append_file(new_file_name);
}

//This is the most important function of the device. These loops have been tweaked as much as possible.
//Modifying this loop may negatively affect how well the device can record at high baud rates.
//Appends a stream of serial data to a given file
//Assumes the currentDirectory variable has been set before entering the routine
//Does not exit until Ctrl+z (ASCII 26) is received
//Returns 0 on error
//Returns 1 on success
uint8_t append_file(char* file_name)
{
  // O_CREAT - create the file if it does not exist
  // O_APPEND - seek to the end of the file prior to each write
  // O_WRITE - open for write
  if (!file.open(&currentDirectory, file_name, O_CREAT | O_APPEND | O_WRITE)) error("open1");
  if (file.fileSize() == 0) {
    //This is a trick to make sure first cluster is allocated - found in Bill's example/beta code
    //file.write((uint8_t)0); //Leaves a NUL at the beginning of a file
    file.rewind();
    file.sync();
  }  

  NewSerial.print('<'); //give a different prompt to indicate no echoing
  digitalWrite(statled1, HIGH); //Turn on indicator LED

#if RAM_TESTING
  NewSerial.print("Free RAM receive ready: ");
  NewSerial.println(memoryTest());
#endif

  uint8_t localBuffer[LOCAL_BUFF_SIZE];

  uint16_t idleTime = 0;
  const uint16_t MAX_IDLE_TIME_MSEC = 100; //The number of milliseconds before unit goes to sleep

  uint16_t timeSinceLastRecord = 0; //Sync the file every maxLoop # of bytes

  //Start recording incoming characters
  while(1) { //Infinite loop

    uint8_t n = NewSerial.read(localBuffer, sizeof(localBuffer)); //Read characters from global buffer into the local buffer
    if (n > 0) {
      //Scan the local buffer for esacape characters
      //In the light version of OpenLog, we don't check for escape characters

      file.write(localBuffer, n); //Record the buffer to the card

      STAT1_PORT ^= (1<<STAT1); //Toggle the STAT1 LED each time we record the buffer

      idleTime = 0; //We have characters so reset the idleTime

      if(timeSinceLastRecord++ > MAX_LOOPS) { //This will force a sync approximately every second
        timeSinceLastRecord = 0;
        file.sync();
      }
    }
    else {
      idleTime++;
      delay(1); //Burn 1ms waiting for new characters coming in
    }
  }

  return(1); //Success!
}

//The following are system functions needed for basic operation
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Blinks the status LEDs to indicate a type of error
void blink_error(uint8_t ERROR_TYPE) {
  while(1) {
    for(int x = 0 ; x < ERROR_TYPE ; x++) {
      digitalWrite(statled1, HIGH);
      delay(200);
      digitalWrite(statled1, LOW);
      delay(200);
    }

    delay(2000);
  }
}

//End core system functions
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
