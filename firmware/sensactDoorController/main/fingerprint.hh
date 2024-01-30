#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
namespace fingerprint
{
    constexpr uint32_t POWER_UP_DELAY_MS{50};
    constexpr size_t NOTEPAD_SIZE_BYTES{16 * 32};
    constexpr size_t FEATURE_BUFFER_MAX{6};
    constexpr uint32_t DEFAULT_ADDRESS{0xFFFFFFFF};
    constexpr uint32_t DEFAULT_BAUD_RATE{57600};
    constexpr size_t DEFAULT_TIMEOUT{1000}; //!< UART reading timeout in milliseconds
    enum class INSTRUCTION : uint8_t
    {
        GenImg = 0x01,         // Collect finger image
        Img2Tz = 0x02,         // To generate character file from image
        Match = 0x03,          // Carry out precise matching of two templates;
        Search = 0x04,         // Search the finger library
        RegModel = 0x05,       // To combine character files and generate template
        Store = 0x06,          // To store template
        LoadChar = 0x07,       // to read/load template
        UpChar = 0x08,         // to upload template
        DownChr = 0x09,        // to download template
        UpImage = 0x0A,        // To upload image
        DownImage = 0x0B,      // To download image
        DeleteChar = 0x0C,     // to delete templates
        Empty = 0x0d,          // to empty the library
        SetSysPara = 0x0e,     // To set system Parameter
        ReadSysPara = 0x0f,    // To read system Parameter
        SetPwd = 0x12,         // To set password
        VfyPwd = 0x13,         // To verify password
        GetRandomCode = 0x14,  // to get random code
        SetAddr = 0x15,        // To set device address
        ReadInfPage = 0x16,    // Read information page
        WriteNotepad = 0x18,   // Write Notepad on sensor
        ReadNotepad = 0x19,    // Read Notepad from sensor
        TempleteNum = 0x1D,    // To read finger template numbers
        ReadIndexTable = 0x1F, // Read-fingerprint template index table
        Cancel = 0x30,         // Cancel instruction
        HandShake = 0x40,      // HandShake
        GetAlgVer = 0x39,      // Get the algorithm library version
        GetFwVer = 0x3A,       // Get the firmware version
        AuraControl = 0x35,    // AuraLedConfig
        AutoEnroll = 0x31,     // Automatic registration template
        AutoIdentify = 0x32,   // Automatic fingerprint verification
    };

    enum class RET
    {
        OK = 0x00,                           //!< Command execution is complete
        PACKET_RECIEVE_ERR = 0x01,           //!< Error when receiving data package
        NO_FINGER_ON_SENSOR = 0x02,          //!< No finger on the sensor
        ENROLL_FAIL = 0x03,                  //!< Failed to enroll the finger
        GENERATE_CHARACTER_FILE_FAIL = 0x06, //!< Failed to generate character file due to overly disorderly fingerprint image
        FEATURE_FAIL = 0x07,                 //!< Failed to generate character file due to the lack of character point or small fingerprint image
        NO_MATCH = 0x08,                     //!< Finger doesn't match
        FINGER_NOT_FOUND = 0x09,             //!< Failed to find matching finger
        FAILTO_COMBINE_FINGER_FILES = 0x0A,  //!< Failed to combine the character files
        BAD_LOCATION = 0x0B,                 //!< Addressed PageID is beyond the finger library
        DB_RANGE_FAIL = 0x0C,                //!< Error when reading template from library or invalid template
        UPLOAD_TEMPLATE_FAIL = 0x0D,         //!< Error when uploading template
        PACKETRESPONSEFAIL = 0x0E,           //!< Module failed to receive the following data packages
        UPLOADFAIL = 0x0F,                   //!< Error when uploading image
        DELETEFAIL = 0x10,                   //!< Failed to delete the template
        DBCLEARFAIL = 0x11,                  //!< Failed to clear finger library
        WRONG_PASSWORD = 0x13,               //!< wrong password!
        INVALIDIMAGE = 0x15,                 //!< Failed to generate image because of lac of valid primary image
        FLASHERR = 0x18,                     //!< Error when writing flash
        NO_DEFINITION_ERROR = 0x19,
        INVALIDREG = 0x1A, //!< Invalid register number
        INCORRECT_CONFIGURATION = 0x1b,
        WRONG_NOTEPAD_PAGE_NUMBER = 0x1c,
        COMMUNICATION_PORT_FAILURE = 0x1d,
        FINGERPRINT_LIBRARY_IS_FULL = 0x1f,

        ADDRESS_CODE_INCORRECT = 0x20,
        PASSWORT_MUST_BE_VERIFIED = 0x21,     // password must be verified;
        FINGERPRINT_TEMPLATE_IS_EMPTY = 0x22, // fingerprint template is empty;
        FINGERPRINT_LIB_IS_EMPTY = 0x24,
        TIMEOUT = 0x26,
        FINGERPRINT_ALREADY_EXISTS = 0x27,
        SENSOR_HARDWARE_ERROR = 0x029,

        UNSUPPORTED_COMMAND = 0xfc,
        HARDWARE_ERROR = 0xfd,
        COMMAND_EXECUTION_FAILURE = 0xfe,
    };

    enum class PARAM_INDEX : uint8_t
    {
        BAUD_RATE_CONTROL = 4, // N= [1/2/4/6/12]. Corresponding baud rate is 9600*N bps。, Default 6
        SECURITY_LEVEL = 5,
        DATA_PACKAGE_LENGTH = 6, // Its value is 0, 1, 2, 3, corresponding to 32 bytes, 64 bytes, 128 bytes, 256 bytes respectively.
    };

#define FINGERPRINT_LEDON 0x50  //!< Turn on the onboard LED
#define FINGERPRINT_LEDOFF 0x51 //!< Turn off the onboard LED

#define FINGERPRINT_LED_BREATHING 0x01   //!< Breathing light
#define FINGERPRINT_LED_FLASHING 0x02    //!< Flashing light
#define FINGERPRINT_LED_ON 0x03          //!< Always on
#define FINGERPRINT_LED_OFF 0x04         //!< Always off
#define FINGERPRINT_LED_GRADUAL_ON 0x05  //!< Gradually on
#define FINGERPRINT_LED_GRADUAL_OFF 0x06 //!< Gradually off
#define FINGERPRINT_LED_RED 0x01         //!< Red LED
#define FINGERPRINT_LED_BLUE 0x02        //!< Blue LED
#define FINGERPRINT_LED_PURPLE 0x03      //!< Purple LEDpassword

    enum class PARAM_LED : uint8_t
    {

    };

    enum class PARAM_BAUD : uint8_t
    {
        _9600 = 0x1,   //!< UART baud 9600
        _19200 = 0x2,  //!< UART baud 19200
        _28800 = 0x3,  //!< UART baud 28800
        _38400 = 0x4,  //!< UART baud 38400
        _48000 = 0x5,  //!< UART baud 48000
        _57600 = 0x6,  //!< UART baud 57600
        _67200 = 0x7,  //!< UART baud 67200
        _76800 = 0x8,  //!< UART baud 76800
        _86400 = 0x9,  //!< UART baud 86400
        _96000 = 0xA,  //!< UART baud 96000
        _105600 = 0xB, //!< UART baud 105600
        _115200 = 0xC, //!< UART baud 115200
    };

    enum class PARAM_SECURITY : uint8_t
    {
        //!< Security level register address
        // The safety level is 1 The highest rate of false recognition , The rejection
        // rate is the lowest . The safety level is 5 The lowest tate of false
        // recognition, The rejection rate is the highest .
        _1 = 0X1, //!< Security level 1
        _2 = 0X2, //!< Security level 2
        _3 = 0X3, //!< Security level 3
        _4 = 0X4, //!< Security level 4
        _5 = 0X5, //!< Security level 5
    };

    enum class PARAM_PACKETSIZE : uint8_t
    {

        _32 = 0X0,  //!< Packet size is 32 Byte
        _64 = 0X1,  //!< Packet size is 64 Byte
        _128 = 0X2, //!< Packet size is 128 Byte
        _256 = 0X3, //!< Packet size is 256 Byte
    };

    constexpr uint16_t STARTCODE{0xEF01}; //!< Fixed falue of EF01H; High byte transferred first
    
    enum class PacketIdentifier : uint8_t
    {
        COMMANDPACKET = 0x1, //!< Command packet
        DATAPACKET = 0x2,    //!< Data packet, must follow command packet or acknowledge packet
        ACKPACKET = 0x7,     //!< Acknowledge packet
        ENDDATAPACKET = 0x8, //!< End of data packet
    };

   
    ///! Helper class to craft UART packets
    struct FingerprintPacket
    {
        FingerprintPacket(INSTRUCTION type, uint16_t length, uint8_t *data)
        {
            this->start_code = STARTCODE;
            this->type = (uint8_t)type;
            this->length = length;
            address[0] = 0xFF;
            address[1] = 0xFF;
            address[2] = 0xFF;
            address[3] = 0xFF;
            if (length < 64)
                std::memcpy(this->data, data, length);
            else
                std::memcpy(this->data, data, 64);
        }
        uint16_t start_code; ///< "Wakeup" code for packet detection
        uint8_t address[4];  ///< 32-bit Fingerprint sensor address
        uint8_t type;        ///< Type of packet
        uint16_t length;     ///< Length of packet
        uint8_t data[64];    ///< The raw buffer for packet payload
    };

    
    class M
    {
    public:
        M(HardwareSerial *hs, uint32_t password = 0x0);
        Adafruit_Fingerprint(Stream *serial, uint32_t password = 0x0);

        void begin(uint32_t baud);

        bool verifyPassword(void);
        uint8_t getParameters(void);

        uint8_t getImage(void);
        uint8_t image2Tz(uint8_t slot = 1);
        uint8_t createModel(void);

        uint8_t emptyDatabase(void);
        uint8_t storeModel(uint16_t id);
        uint8_t loadModel(uint16_t id);
        uint8_t getModel(void);
        uint8_t deleteModel(uint16_t id);
        uint8_t fingerFastSearch(void);
        uint8_t fingerSearch(uint8_t slot = 1);
        uint8_t getTemplateCount(void);
        uint8_t setPassword(uint32_t password);
        uint8_t LEDcontrol(bool on);
        uint8_t LEDcontrol(uint8_t control, uint8_t speed, uint8_t coloridx,
                           uint8_t count = 0);

        uint8_t setBaudRate(uint8_t baudrate);
        uint8_t setSecurityLevel(uint8_t level);
        uint8_t setPacketSize(uint8_t size);

        void writeStructuredPacket(const Adafruit_Fingerprint_Packet &p);
        uint8_t getStructuredPacket(Adafruit_Fingerprint_Packet *p,
                                    uint16_t timeout = DEFAULTTIMEOUT);

        /// The matching location that is set by fingerFastSearch()
        uint16_t fingerID;
        /// The confidence of the fingerFastSearch() match, higher numbers are more
        /// confidents
        uint16_t confidence;
        /// The number of stored templates in the sensor, set by getTemplateCount()
        uint16_t templateCount;

        uint16_t status_reg = 0x0;   ///< The status register (set by getParameters)
        uint16_t system_id = 0x0;    ///< The system identifier (set by getParameters)
        uint16_t capacity = 64;      ///< The fingerprint capacity (set by getParameters)
        uint16_t security_level = 0; ///< The security level (set by getParameters)
        uint32_t device_addr =
            0xFFFFFFFF;             ///< The device address (set by getParameters)
        uint16_t packet_len = 64;   ///< The max packet length (set by getParameters)
        uint16_t baud_rate = 57600; ///< The UART baud rate (set by getParameters)

    private:
        uint8_t checkPassword(void);
        uint8_t writeRegister(uint8_t regAdd, uint8_t value);
        uint32_t thePassword;
        uint32_t theAddress;
        uint8_t recvPacket[20];

        Stream *mySerial;
#if defined(__AVR__) || defined(ESP8266) || defined(FREEDOM_E300_HIFIVE1)
        SoftwareSerial *swSerial;
#endif
        HardwareSerial *hwSerial;
    };
}

/*
  By using the touch ring as an additional input to the image sensor the sensitivity is much higher for door bell ring events. Unfortunately
  we cannot differ between touches on the ring by fingers or rain drops, so rain on the ring will cause false alarms.
*/
const int touchRingPin = 5; // touch/wakeup pin connected to fingerprint sensor

enum class ScanResult
{
    noFinger,
    matchFound,
    noMatchFound,
    error
};
enum class EnrollResult
{
    ok,
    error
};

struct Match
{
    ScanResult scanResult = ScanResult::noFinger;
    uint16_t matchId = 0;
    String matchName = "unknown";
    uint16_t matchConfidence = 0;
    uint8_t returnCode = 0;
};

struct NewFinger
{
    EnrollResult enrollResult = EnrollResult::error;
    uint8_t returnCode = 0;
};

class FingerprintManager
{
private:
    Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
    bool lastTouchState = false;
    String fingerList[201];
    int fingerCountOnSensor = 0;
    bool ignoreTouchRing = false; // set to true when the sensor is usually exposed to rain to avoid false ring events. Can also be set conditional by a rain sensor over MQTT
    bool lastIgnoreTouchRing = false;

    void updateTouchState(bool touched);
    bool isRingTouched();
    void loadFingerListFromPrefs();
    void disconnect();
    uint8_t writeNotepad(uint8_t pageNumber, const char *text, uint8_t length);
    uint8_t readNotepad(uint8_t pageNumber, char *text, uint8_t length);

public:
    bool connected;
    bool connect();
    Match scanFingerprint();
    NewFinger enrollFinger(int id, String name);
    void deleteFinger(int id);
    void renameFinger(int id, String newName);
    String getFingerListAsHtmlOptionList();
    void setIgnoreTouchRing(bool state);
    bool isFingerOnSensor();
    void setLedRingError();
    void setLedRingWifiConfig();
    void setLedRingReady();
    String getPairingCode();
    bool setPairingCode(String pairingCode);

    bool deleteAll();

    // functions for sensor replacement
    void exportSensorDB();
    void importSensorDB();
};
