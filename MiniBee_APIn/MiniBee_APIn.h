#ifndef MiniBee_APIn_h
#define MiniBee_APIn_h

// #include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <inttypes.h>
#include <WProgram.h>
// #include <EEPROM.h>
// #include <Wire.h>

#include <XBee.h>


// #define MINIBEE_REVISION 'D'
#define MINIBEE_LIBVERSION 4

/// all together: 3644 bytes
#define MINIBEE_ENABLE_TWI 1  /// TWI/I2C takes up 2064 bytes
#define MINIBEE_ENABLE_SHT 1  /// SHT takes up 1140 bytes
#define MINIBEE_ENABLE_PING 1 /// Ping takes up 440 bytes

#define MINIBEE_ENABLE_TWI_ADXL 1 /// 962 bytes - without: 18176
#define MINIBEE_ENABLE_TWI_LISDL 1 /// 614 bytes - without: 18524
#define MINIBEE_ENABLE_TWI_HMC 1 /// 1644 bytes - without: 17494 
#define MINIBEE_ENABLE_TWI_BMP 1 /// 4182 bytes - without: 14956
#define MINIBEE_ENABLE_TWI_TMP 1 /// 250 bytes - without: 18888

#if MINIBEE_ENABLE_TWI == 1
#include <Wire.h>

  #if MINIBEE_ENABLE_TWI_ADXL == 1
  #include <ADXL345.h>
  #endif

  #if MINIBEE_ENABLE_TWI_LISDL == 1
  #include <LIS302DL.h>
  #endif

  #if MINIBEE_ENABLE_TWI_TMP == 1
  #include <TMP102.h>
  #endif

  #if MINIBEE_ENABLE_TWI_BMP == 1
  #include <BMP085.h>
  #endif

  #if MINIBEE_ENABLE_TWI_HMC == 1
  #include <HMC5843.h>
  #endif

#endif

enum MiniBeePinConfig { 
  NotUsed,
  DigitalIn, DigitalOut,
  AnalogIn, AnalogOut, AnalogIn10bit, 
  SHTClock, SHTData, 
  TWIClock, TWIData,
  Ping,
  Custom = 100,
  MeID = 150,
  UnConfigured = 200,
};

enum TWIDeviceConfig { 
  TWI_ADXL345=10,
  TWI_LIS302DL=11,
  TWI_BMP085=20,
  TWI_TMP102=30,
  TWI_HMC58X3=40
};


class MiniBee_API{
  public:
    MiniBee_API();
    void setup( long, char );
    void loopStep( bool usedelay = true );
    
    void setID( uint8_t id );
    uint8_t getId(void);
    
    void sendData( void );
    void sendActive( void );
    void sendPaused( void );
    
    void readXBeeSerial();
    void sendXBeeSerial();
    
    uint8_t* sendAtCommand();
    void sendTx16( char type, uint8_t* data, uint8_t length );
    
    void (*customMsgFunc)( uint8_t *, uint8_t, uint16_t );// = NULL;
    void (*dataMsgFunc)( uint8_t *, uint8_t, uint16_t );// = NULL;

    
    void setCustomParser( void (*customFunc)(uint8_t *, uint8_t, uint16_t ) );

    void setRunning( uint8_t ); 
    void setLoopback( uint8_t ); 
    void setRemoteConfig( uint8_t ); 

    void setOutput();
    void setOutputValues( uint8_t * msg, uint8_t offset );
    
    bool checkConfMsg( uint8_t mid );

    uint8_t *config; //array of pointers for all the config bytes
//     void writeConfig(uint8_t *, uint8_t); // eeprom write
//     void readConfig(void); // eeprom read
    void readConfigMsg(uint8_t *, uint8_t); // read config message as it arrives
    void parseConfig(void); // parse the config

#if MINIBEE_ENABLE_TWI == 1
    bool getFlagTWI();	//returns twi flag state
    int readTWIdevices(int dboff);
    void setupTWIdevices(void);
#endif

#if MINIBEE_ENABLE_SHT == 1
	//sht
	int ioSHT;
	int ackSHT;
	int valSHT; 
	bool getFlagSHT();	//returns sht flag state
	uint8_t *getPinSHT();	//returns the pins used for SHT

// 		void setupSHT(int*);	//setup function for SHT
	void setupSHT();	//setup function for SHT
	void startSHT(void);
	void resetSHT(void);
	void softResetSHT(void);
	void waitSHT(void);
	void measureSHT(int cmd);
	void writeByteSHT(void);
	void readByteSHT(void);
	int getStatusSHT(void);
	int shiftInSHT(void);
#endif

#if MINIBEE_ENABLE_PING == 1
	//ping
	bool getFlagPing();	//returns ping flag state
	uint8_t getPinPing();	//returns the pins used for Ping
// 		void setupPing(int*);	//setup function for Ping
	int readPing(void);
#endif


  private:
    void setMeLed( uint8_t );
    uint8_t me_status;
    
    	// collecting sensor data:
    void dataFromInt( unsigned int output, int offset );
    void dataFromLong24( unsigned long output, int offset );

    uint8_t readSensors( uint8_t db );
    void readXBeePacket();
    void routeMsg(uint8_t type, uint8_t *msg, uint8_t size, uint16_t source );
    bool checkIDMsg( uint8_t );
    bool checkNotNodeMsg( uint16_t );
    
    bool isAnalogPin( uint8_t );
    bool isIOPin( uint8_t );
    
    void flashLed(int pin, int times, int wait);
    
    char msg_type;
    uint8_t status;
    uint8_t actcount;

//     char *outMessage;
    uint8_t outMessage[2];

    uint8_t *outData;
    int datacount;
    int datasize;
    
    uint8_t node_id; // TODO: should this be a uint16_t instead?
    uint8_t config_id;
    uint8_t configInfo[2];
    
    uint8_t prev_id_msg;
    
    uint8_t msg_id_send;
    int msgInterval;
    uint8_t samplesPerMsg;
    int smpInterval;
    uint8_t curSample;
    
    bool loopback;
    uint8_t remoteConfig;

    bool hasInput; // = false;
    bool hasOutput; // = false;
    bool hasCustom; // = false;
    
    char board_revision;
    
    uint8_t nrpins;

    #define PINOFFSET 3
    
    bool custom_pin[19]; // sets whether custom pin is configured
    bool digital_in[19]; 
    bool analog_in[8];
    uint8_t custom_size[19]; // sets size of custom pin data
    uint8_t customDataSize;
    uint8_t customInputs;
    
    bool analog_precision[8]; 
    
    bool pwm_on[6]; // sets whether pwm pin is on or not
    static uint8_t pwm_pins[]; // = { 3,5,6, 8,9,10 };
    uint8_t pwm_values[6]; // = {0,0,0, 0,0,0};

    bool digital_out[19]; // sets whether digital out on
    uint8_t digital_values[19];


#if MINIBEE_ENABLE_SHT == 1
	// SHT sensor - See Sensirion Data sheet
	#define  SHT_T_CMD  0x03                
	#define  SHT_H_CMD  0x05
	#define  SHT_R_STAT 0x07
	#define  SHT_W_STAT 0x06
	#define  SHT_RST_CMD 0x1E

    uint8_t shtMask;
    bool shtOn;
    uint8_t sht_pins[2];	//scl, sda  clock, data
#endif

#if MINIBEE_ENABLE_PING == 1
    bool pingOn;
    uint8_t ping_pin;	//ping pins (these could be more, but not right now)
#endif

    
#if MINIBEE_ENABLE_TWI == 1
    bool twiOn;
    uint8_t * twi_devices;
    uint8_t nr_twi_devices;
    #if MINIBEE_ENABLE_TWI_ADXL == 1
      ADXL345 * accelADXL;
    #endif
    #if MINIBEE_ENABLE_TWI_LISDL == 1
      LIS302DL * accelLIS;
    #endif
    #if MINIBEE_ENABLE_TWI_TMP == 1
      TMP102 * temp102;
    #endif
    #if MINIBEE_ENABLE_TWI_BMP == 1
      BMP085 * bmp085;
    #endif
    #if MINIBEE_ENABLE_TWI_HMC == 1
      HMC5843 * hmc58x3;
    #endif
#endif


};

#endif	
