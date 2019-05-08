#ifndef SCCFLOWRPROTOCOL_H
#define SCCFLOWRPROTOCOL_H


#include <vector>
#include <list>
#include <unordered_map>
#include <queue>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <locale>

#define CMD_INVALID "Invalid"
#define CMD_CHECKSTATUS "CheckStatus"
#define CMD_ADDRESSSETTING "AddressSetting"
#define CMD_GETTAGDATA "GetTagData"

#define HOST_HEADER "HostHeader"
#define WGT_HEADER "WGTHeader"

#define ADDRESS_BYTE    'U'
#define ETX_BYTE        '\3'
#define NULL_CHAR       '\0'
#define TAB_CHAR        '\t'
#define SEPARATOR_CHAR  ','
#define ASSIGN_CHAR     ':'
#define START_BYTE      ASSIGN_CHAR
#define LF_CHAR         '\n'
#define CR_CHAR         '\r'

#define RTU_CMD_READ    0x03

#define MAX_WGT_BUFFER_SIZE 512

#define MAX_CHANNELS        1

#define MIN_WGT_DATA        8

#define MAX_VARS            4

#define MAX_REGISTERS       28

#define STATUS_FAILURE              0x4e
#define STATUS_SUCCESS              0x59
#define STATUS_RQT_ADDR_SETTING     0x01
#define STATUS_APPLY_ADDR_INFO      0x02
#define STATUS_ADDR_RCV_SUCCESS     0x03
#define STATUS_ADDR_DIST_SETTING    0x04
#define STATUS_ADDR_SET_SUCCEEDS    0x05
#define STATUS_NO_TAG_DATA          0x06
#define STATUS_TAG_READ_SUCCEEDS    0x07
#define STATUS_TAG_DATA_READY       0x08
#define STATUS_IDLE                 0x09
#define STATUS_NO_BATTERY           0x0a

#define VAR_BATTERY_ALARM           "Battery_Alarm"
#define VAR_FAIL_STATUS             "Fail_Status"
#define VAR_NOZZLE_ACTIVED          "Nozzle_Actived"
#define VAR_TAG_DETECTED            "Tag_Detected"



#define D_INSTANTFLOWRATE			0
#define D_FLUIDSPEED				2
#define D_MEASUREFLUIDSOUNDSPEED	3
#define L_POSACUMFLOWRATE			4
#define D_POSACUMFLOWRATEDECPART	5
#define L_NEGACUMFLOWRATE			6
#define D_NEGACUMFLOWRATEDECPART	7
#define L_NETACUMFLOWRATE			12
#define D_NETACUMFLOWRATEDECPART	13

#define VAR_INSTANTFLOWRATE			"InstantFlowRate"
#define VAR_FLUIDSPEED				"FluidSpeed"
#define VAR_MEASUREFLUIDSOUNDSPEED	"MeasureFluidSoundSpeed"
#define VAR_POSACUMFLOWRATE			"PosAcumFlowRate"
#define VAR_POSACUMFLOWRATEDECPART	"PosAcumFlowRateDecPart"
#define VAR_NEGACUMFLOWRATE			"NegAcumFlowRate"
#define VAR_NEGACUMFLOWRATEDECPART	"NegAcumFlowRateDecPart"
#define VAR_NETACUMFLOWRATE			"NetAcumFlowRate"
#define VAR_NETACUMFLOWRATEDECPART	"NetAcumFlowRateDecPart"


/*enum Host2WGTCommand
{
    Invalid = 0,
    StatusCheck,
    AddressSetting,
    GetTagData,
};

enum VariableName
{
    BatteryAlarm = 0,
    FailStatus,
    NozzleActived,
    TagDetected,
    VariableName_size
};

struct commandStruct
{
    char    command;
    char    addr;
    char    len;
    char    data[60];

    commandStruct(char cmd, char addr, char* resp, char len)
     : command(cmd), addr(addr), len(len)
    {
        memcpy(data, resp, len);
    }
    commandStruct()
     : command(0), addr(0), len(0)
    {}
    void set(char cmd, char chAddr, char* resp, char chLen)
    {
        command = cmd;
        addr    = chAddr;
        len     = chLen;
        memcpy(data, resp, len);
    }

};

struct ActionStruct
{
    std::string     strCmd;
    int             iTimeOut;
    bool            bNozzleActived;
    bool            bAlarm;
    bool            bFail;

    ActionStruct(const std::string& cmd, const int timeOut, bool nozzleActived, bool alarm, bool fail)
     : strCmd(cmd), iTimeOut(timeOut), bNozzleActived(nozzleActived), bAlarm(alarm), bFail(fail)
    {}
    ActionStruct()
     : strCmd(""), iTimeOut(0), bNozzleActived(false), bAlarm(false), bFail(false)
    {}
};

struct TagDataStruct
{
    char    chTagData[MAX_WGT_BUFFER_SIZE];
    char    chLenData;

    TagDataStruct(const char* buffer, const char len) : chLenData(len)
    {
        chTagData[0] = '\0';
        if (chLenData > 0)
            memcpy(chTagData, buffer, len);
    }
    TagDataStruct() : chLenData(0)
    {
        chTagData[0] = NULL_CHAR;
    }
};

struct VarStatus
{
    bool    bCurrentStatus;
    int     iThresHold;
    int     iChangesCount;
};*/

struct FlowRegisters
{
    double      m_dInstantFlowRate;
    double      m_dFluidSpeed;
    double      m_dMeasureFluidSoundSpeed;
    long        m_lPosAcumFlowRate;
    double      m_dPosAcumFlowRateDecPart;
    long        m_lNegAcumFlowRate;
    double      m_dNegAcumFlowRateDecPart;
    long        m_lNetAcumFlowRate;
    double      m_dNetAcumFlowRateDecPart;
};

union RawData
{
    float       fRegister[MAX_REGISTERS/2];
    int32_t     lRegister[MAX_REGISTERS/2];
};

class SCCFlowProtocol
{
    public:
        SCCFlowProtocol();
        virtual ~SCCFlowProtocol();

        std::string convChar2Hex(char* buffer, char& len);
        /*std::string getStrCmdStatusCheck(int addr, char* buffer, char& len);
        std::string getStrCmdSetAddr(int addr, int newAddr, char* buffer, char& len);
        std::string getStrCmdGetTagId(int addr, char* buffer, char& len);*/

        //bool getWGTResponse(char* buffer, char len, std::string& cmd, int& addr, char* resp, char& respLen);
        bool getFlowMeterResponse(char addr, char* buffer, char len);

        std::string getStrStatus(char status);

        bool nextAction(int addr, char* buffer, char& len, int& timeout);

        bool isAlarm(char addr);
        bool isFail(char addr);
        bool isNozzleActived(char addr);
        bool isTagDetected(char addr);

        std::string printStatus(char addr, bool addStrData = false);

        bool getTagId(char addr, char* tagBuffer, char& len);
        char getStatus(char addr);

        std::string getCmdReadRegisters(char addr, char* buffer, char& len, unsigned int startRegister, unsigned int numRegisters);

        void printData();

    protected:

        unsigned char calcCRC(unsigned char* pFirst, unsigned char* pEnd);
        unsigned char calcLRC(unsigned char* pFirst,unsigned char len);
        //std::string getStrCmd(const std::string& cmd, int addr, int addr2, char* buffer, char& len);
        void moveBufferToLeft(char* pos, char offset);
        /*std::string getWGTCommand(char cmd);
        bool getWGTResponse(std::string& cmd, int& addr, char* resp, char& respLen);
        void addCommandToDvcMap(char cmd, char addr, char* resp, char len);

        bool nextActionFromStatus(commandStruct& cmdSt, int addr, char* buffer, char& len, int& timeout);
        bool nextActionFromAddressSetting(commandStruct& cmdSt, int addr, char* buffer, char& len, int& timeout);
        bool nextActionFromGetTagData(commandStruct& cmdSt, int addr, char* buffer, char& len, int& timeout);

        void addStatusToVector(char addr, commandStruct& cmdSt);

        void addTagDataToMap(commandStruct& cmdSt, char addr);

        void getCommandFromAction(ActionStruct& actionSt, char addr, char* buffer, char& len);

        ActionStruct getActionFromStatus(char status);

        void setAlarm(char addr);
        void setNozzleActivated(char addr);
        void setFail(char addr);
        void setTagDetected(char addr);

        void clearAlarm(char addr);
        void clearNozzleActivated(char addr);
        void clearFail(char addr);
        void clearTagDetected(char addr);

        void setVector(char addr, bool* vect);
        void clearVector(char addr, bool* vect);
        bool isVector(char addr, bool* vect);*/

        std::string boolToString(bool b, const std::string& valTrue = "", const std::string& valFalse = "");

        void setVar(int addr, int var);
        bool clearVar(int addr, int var);
        bool isSetVar(int addr, int var);

        template <class T>
        std::string numToAscii(const T& d, char length)
        {
            std::stringstream ss;

            ss << std::setfill('0') << std::setw(length) << std::hex << (int)d;

            std::string str(ss.str());

            std::string strUpper;

            std::locale loc;

            for (std::string::size_type i = 0; i < str.length(); ++i)
                strUpper += std::toupper(str[i], loc);

            return strUpper;
        }
        template <class T>
        void numToAscii(const T& d, char* buffer, char length)
        {
            std::string s = numToAscii(d, length);
            memcpy(buffer, s.c_str(), length);
        }
        unsigned char asciiHexToDec(const char hexHi, const char hexLo);
        unsigned char asciiHexToDec(const char hex);
        unsigned char asciiHexToDec(const char* hex);

        bool checkAddress(char addr, char* frame);
        bool checkCommand(char cmd, char* frame);
        bool checkLRC(char* pFirst, size_t len);

        bool compareValueToBuffer(unsigned char val, char* frame, size_t len);
        void readRTUData(char addr, char* pFirst, size_t len);

        void asciiToReal4(char* p, double& val, char num);
        void asciiHexToFloat(unsigned char* pDst, char* pSrc, size_t bytes);
        void putData(char* p, char num);

    private:

        int m_iAddress;
        char m_chBufferIn[MAX_WGT_BUFFER_SIZE];
        char* m_pLast;
        int m_iBufferSize;

        //commandStruct m_DeviceVector[MAX_CHANNELS];
        //commandStruct* m_pCommandSt;

        //std::unordered_map <char, TagDataStruct> m_TagDataMap;

        /*char m_chStatusVector[MAX_CHANNELS];
        bool m_bAlarmVector[MAX_CHANNELS];
        bool m_bFailVector[MAX_CHANNELS];
        bool m_bNozzleActivedVector[MAX_CHANNELS];
        bool m_bTagDetected[MAX_CHANNELS];
        VarStatus m_VarStatus[MAX_CHANNELS][MAX_VARS];*/
        FlowRegisters m_Register[MAX_CHANNELS];
        RawData         m_RawData[MAX_CHANNELS];

        std::string     m_strData;
        int             m_iDataLen;
};

#endif // SCCFLOWRPROTOCOL_H
