#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <memory>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#define TAG "FINGER"
#include <common.hh>
namespace fingerprint
{
    constexpr TickType_t POWER_UP_DELAY_TICKS{pdMS_TO_TICKS(50)};
    constexpr size_t NOTEPAD_SIZE_BYTES{16 * 32};
    constexpr size_t FEATURE_BUFFER_MAX{6};
    constexpr uint32_t DEFAULT_ADDRESS{0xFFFFFFFF};
    constexpr uint32_t DEFAULT_BAUD_RATE{57600};
    constexpr TickType_t DEFAULT_TIMEOUT_TICKS{pdMS_TO_TICKS(1000)}; //!< UART reading timeout in milliseconds
    constexpr const char* enrollStep2description[]{
        "",
        "Collect image for the first time",
        "Generate Feature for the first time",
        "Collect image for the second time",
        "Generate Feature for the second time",
        "Collect image for the third time",
        "Generate Feature for the third time",
        "Collect image for the fourth time",
        "Generate Feature for the fourth time",
        "Collect image for the fifth time",
        "Generate Feature for the fifth time",
        "Collect image for the sixth time",
        "Generate Feature for the sixth time",
        "Repeat fingerprint check",
        "Merge feature",
        "Storage template",
    };

    constexpr const char* identifyStep2description[]{
        "",
        "Collect Image",
        "Generate Feature",
        "Search",
    };

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

        CANNOT_FIND_STARTCODE = 0x50,
        WRONG_MODULE_ADDRESS = 0x51,
        ACKNOWLEDGE_PACKET_EXPECTED=0x52,
        UNEXPECTED_LENGTH=0x53,
        CHECKSUM_ERROR=0x54,
    };

    enum class PARAM_INDEX : uint8_t
    {
        BAUD_RATE_CONTROL = 4, // N= [1/2/4/6/12]. Corresponding baud rate is 9600*N bps。, Default 6
        SECURITY_LEVEL = 5,
        DATA_PACKAGE_LENGTH = 6, // Its value is 0, 1, 2, 3, corresponding to 32 bytes, 64 bytes, 128 bytes, 256 bytes respectively.
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

    struct SystemParameter{
        uint16_t status;
        uint16_t librarySize;
        uint8_t securityLevel;
        uint32_t deviceAddress;
        uint8_t dataPacketSizeCode;
        uint8_t baudRateTimes9600;
        char* algVer;
        char* fwVer;
    };

    class iFingerprintHandler{
        public:
        virtual void HandleFingerprint(uint16_t finger, uint16_t score)=0;
    };


    class M
    {
    private:
        uart_port_t uart_num;
        gpio_num_t gpio_irq;
        SystemParameter params;
        bool previousIrqLineValue{true};
        iFingerprintHandler* handler;
        SemaphoreHandle_t mutex;

        static void static_task(void *args)
        {
            M *myself = static_cast<M *>(args);
            myself->task();
        }

        void task()
        {
            vTaskDelay(POWER_UP_DELAY_TICKS);
            while(true){
                vTaskDelay(pdMS_TO_TICKS(100));
                if(!xSemaphoreTake(mutex, pdMS_TO_TICKS(3000))){
                    ESP_LOGI(TAG, "There seems to be an enrollment. Fingerprint task pauses...");
                    continue;
                }
                bool newIrqValue=gpio_get_level(gpio_irq);
                if(previousIrqLineValue==true && newIrqValue==false){
                    //negative edge detected
                    ESP_LOGI(TAG, "Negative edge detected, trying to read fingerprint");
                    uint16_t fingerIndex;
                    uint16_t score;
                    RET ret= AutoIdentify(fingerIndex, score);
                    
                    if(ret==RET::OK){
                        ESP_LOGI(TAG, "Fingerprint detected successfully: fingerIndex=%d, score=%d", fingerIndex, score);
                        if(this->handler) handler->HandleFingerprint(fingerIndex, score);
                    }else{
                        ESP_LOGI(TAG, "AutoIdentify returns %d", (int)ret);
                    }
                }
                previousIrqLineValue=newIrqValue;
                xSemaphoreGive(mutex);
            }
        }

        void createAndSendDataPackage(PacketIdentifier pid, uint8_t* contents, size_t contentsLength, uint32_t targetAddress=DEFAULT_ADDRESS){
            contentsLength=std::min((size_t)64, contentsLength);
            uint16_t packageLength=contentsLength+2;//data and checksum
            uint16_t wireLength=2+4+1+2+contentsLength+2;
            uint8_t buffer[wireLength];
            WriteUInt16(STARTCODE, buffer, 0);
            WriteUInt32(targetAddress, buffer, 2);
            WriteU8((uint8_t)pid, buffer, 6);
            WriteUInt16(packageLength, buffer, 7);
            std::memcpy(buffer+9, contents, contentsLength);
            uint16_t sum{0};
            for(size_t i=6; i<wireLength-2;i++){ sum+=buffer[i]; }
            uart_write_bytes(this->uart_num, buffer, wireLength);

        }

        RET basicAckPackageCheck(uint32_t targetAddress, uint8_t* buf, size_t bufLen){
            int receivedBytes=uart_read_bytes(this->uart_num, buf, bufLen, DEFAULT_TIMEOUT_TICKS);
            if(receivedBytes!=bufLen) return RET::TIMEOUT;
            if(ParseUInt16(buf, 0)!=STARTCODE) return RET::CANNOT_FIND_STARTCODE;
            if(ParseUInt32(buf, 2)!=targetAddress) return RET::WRONG_MODULE_ADDRESS;
            if(buf[6]!=(uint8_t)PacketIdentifier::ACKPACKET) return RET::ACKNOWLEDGE_PACKET_EXPECTED;
            if(ParseUInt16(buf, 7)!=bufLen-9) return RET::UNEXPECTED_LENGTH;
            uint16_t sum{0};
            for(size_t i=6; i<bufLen-2;i++){ sum+=buf[i];}
            if(ParseUInt16(buf, bufLen-2)!=sum) return RET::CHECKSUM_ERROR;
            if(buf[9]!=0) return (RET)buf[9];
            return RET::OK;
        }

        #define BPC() while(0){RET ret=basicAckPackageCheck(targetAddress, buffer, wireLength); if(ret!=RET::OK) return ret;}

        RET SetSysPara(PARAM_INDEX param, uint8_t value, uint32_t targetAddress=DEFAULT_ADDRESS){
            uint8_t data[]{(uint8_t)INSTRUCTION::SetSysPara,(uint8_t)param,value};
            createAndSendDataPackage(PacketIdentifier::COMMANDPACKET, data, sizeof(data));
            size_t wireLength{12};
            uint8_t buffer[wireLength];
            BPC();
            return RET::OK;
        }

        RET ReadSysPara(SystemParameter& outParams, uint32_t targetAddress=DEFAULT_ADDRESS){
            uint8_t data[]{(uint8_t)INSTRUCTION::ReadSysPara};
            createAndSendDataPackage(PacketIdentifier::COMMANDPACKET, data, sizeof(data));
            const size_t wireLength{0x13+9};
            uint8_t buffer[wireLength];
            BPC();
            outParams.status=ParseUInt16(buffer, 10);
            outParams.librarySize=ParseUInt16(buffer, 14);
            outParams.securityLevel=ParseUInt16(buffer, 16);
            outParams.deviceAddress=ParseUInt32(buffer, 18);
            outParams.dataPacketSizeCode=ParseUInt16(buffer, 22);
            outParams.baudRateTimes9600=ParseUInt16(buffer, 24);
            return RET::OK;
        }


        RET Get32ByteString(INSTRUCTION instr, char** string, uint32_t targetAddress=DEFAULT_ADDRESS){
            *string = new char[33];
            *string[32]='\0';
            createAndSendDataPackage(PacketIdentifier::COMMANDPACKET, (uint8_t*)&instr, 1);
            const size_t wireLength{0x23+9};
            uint8_t buffer[wireLength];
            BPC();
            strncpy(*string, (char*)(buffer+10), 32);
            return RET::OK;
        }

        RET GetAlgVer(char** string, uint32_t targetAddress=DEFAULT_ADDRESS){
            return Get32ByteString(INSTRUCTION::GetAlgVer, string, targetAddress);
        }

        RET GetFwVer(char** string, uint32_t targetAddress=DEFAULT_ADDRESS){
            return Get32ByteString(INSTRUCTION::GetFwVer, string, targetAddress);
        }

        RET ReadAllSysPara(SystemParameter& outParams, uint32_t targetAddress=DEFAULT_ADDRESS){
            RET ret;
            ret= ReadSysPara(outParams, targetAddress);
            if(ret!=RET::OK) return ret;
            ret= GetAlgVer(&outParams.algVer, targetAddress);
            if(ret!=RET::OK) return ret;
            ret= GetFwVer(&outParams.fwVer, targetAddress);
            return ret;
        }

        RET DeleteChar(uint16_t index, uint32_t targetAddress=DEFAULT_ADDRESS){
            uint8_t data[5];
            data[0]=(uint8_t)INSTRUCTION::DeleteChar;
            WriteUInt16(index, data, 1);
            WriteUInt16(1, data, 3);
            createAndSendDataPackage(PacketIdentifier::COMMANDPACKET, data, sizeof(data));

            const size_t wireLength{0x3+9};
            uint8_t buffer[wireLength];
            BPC();
            return RET::OK;
        }

        RET EmptyLibrary(uint32_t targetAddress=DEFAULT_ADDRESS){
            uint8_t data[]{(uint8_t)INSTRUCTION::Empty};
            createAndSendDataPackage(PacketIdentifier::COMMANDPACKET, data, sizeof(data));
            const size_t wireLength{0x3+9};
            uint8_t buffer[wireLength];
            BPC();
            return RET::OK;
        }

        RET AutoEnroll(uint16_t& fingerIndexOr0xFFFF_inout, bool overwriteExisting, bool duplicateFingerAllowed, bool returnStatusDuringProcess, bool fingerHasToLeaveBetweenScans,  uint32_t targetAddress=DEFAULT_ADDRESS){
            uint8_t data[7];
            data[0]=(uint8_t)INSTRUCTION::AutoEnroll;
            WriteUInt16(fingerIndexOr0xFFFF_inout, data, 1);
            data[3]=overwriteExisting?1:0;
            data[4]=duplicateFingerAllowed?1:0;
            data[5]=returnStatusDuringProcess?1:0;
            data[6]=fingerHasToLeaveBetweenScans?1:0;
            createAndSendDataPackage(PacketIdentifier::COMMANDPACKET, data, sizeof(data));
            const size_t wireLength{0x6+9};
            uint8_t buffer[wireLength];
            while(true){
                BPC(); //returns this function, if anything goes wrong
                uint8_t step = buffer[10];
                fingerIndexOr0xFFFF_inout =ParseUInt16(buffer, 11);
                ESP_LOGI(TAG, "'%s', Finger is stored in index %d", enrollStep2description[step], fingerIndexOr0xFFFF_inout);
                if(step==0x0F) break;
            }
            return RET::OK;
        }

        RET AutoIdentify(uint16_t& fingerIndex_out, uint16_t& score_out,  PARAM_SECURITY securityLevel=PARAM_SECURITY::_3, bool returnStatusDuringProcess=true, uint8_t maxScanAttempts=1,  uint32_t targetAddress=DEFAULT_ADDRESS){
            uint8_t data[8];
            data[0]=(uint8_t)INSTRUCTION::AutoIdentify;
            data[1]=(uint8_t)securityLevel;
            WriteUInt16(0x0, data, 2);//search over all fingers
            WriteUInt16(0x05DC, data, 4);//search over all fingers
            data[6]=returnStatusDuringProcess?1:0;
            data[7]=maxScanAttempts;
            createAndSendDataPackage(PacketIdentifier::COMMANDPACKET, data, sizeof(data));
            const size_t wireLength{0x8+9};
            uint8_t buffer[wireLength];
            while(true){
                BPC(); //returns this function, if anything goes wrong
                uint8_t step = buffer[10];
                fingerIndex_out =ParseUInt16(buffer, 11);
                score_out = ParseUInt16(buffer, 13);
                ESP_LOGI(TAG, "'%s', Finger is stored in index %d", identifyStep2description[step], fingerIndex_out);
                if(step==3) break;
            }
            return RET::OK;
        }
        

    public:
        M(uart_port_t uart_num, gpio_num_t gpio_irq, iFingerprintHandler* handler) : uart_num(uart_num), gpio_irq(gpio_irq), handler(handler) {}
        esp_err_t begin(gpio_num_t txd, gpio_num_t rxd)
        {
            mutex = xSemaphoreCreateMutex();
            /* Install UART friver */
            uart_config_t c = {};
            c.baud_rate = 57600;
            c.data_bits = UART_DATA_8_BITS;
            c.parity = UART_PARITY_DISABLE;
            c.stop_bits = UART_STOP_BITS_1;
            c.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
            c.source_clk = UART_SCLK_DEFAULT;

            ESP_ERROR_CHECK(uart_driver_install(uart_num, 64, 0, 1, nullptr, 0));
            ESP_ERROR_CHECK(uart_param_config(uart_num, &c));
            ESP_ERROR_CHECK(uart_set_pin(uart_num, (int)txd, (int)rxd, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
            ESP_ERROR_CHECK(uart_flush(uart_num));

            gpio_set_pull_mode(gpio_irq, GPIO_PULLUP_ONLY);

            RET ret = ReadAllSysPara(this->params);
            if(ret!=RET::OK){
                ESP_LOGE(TAG, "Communication error with fingerprint reader. Error code %d", (int)ret);
            }

            xTaskCreate(static_task, "fingerprint", 4096, this, 10, nullptr);
            return ESP_OK;
        }

        bool TryEnrollAndStore(const char* name){
            if(!xSemaphoreTake(mutex, pdMS_TO_TICKS(3000))){
                ESP_LOGE(TAG, "Cannot take mutex in enrollment after 3000ms");
                return false;
            }
            uint16_t fingerIndex{0xFFFF};
            RET ret = AutoEnroll(fingerIndex, false, false, true, true);
            if(ret==RET::OK){
                //TODO store in flash
                ESP_LOGI(TAG, "Fingerprint has been successfully enrolled and stored with index %d and name %s", fingerIndex, name);
            }
            xSemaphoreGive(mutex);
            return ret==RET::OK;
        }

        bool TryRename(uint16_t index, const char* newName){
            //TODO: in flash
        }

        bool TryDelete(uint16_t index){
            //TODO: in sensor and in flash
        }

        bool TryDeleteAll(uint16_t index){
            //TODO : in sensor and in flash
        }
    };

}
#undef TAG