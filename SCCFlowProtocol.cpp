#include "SCCFlowProtocol.h"
#include "../main_control/SCCDeviceNames.h"
#include "../main_control/SCCLog.h"

#include <iostream>

extern SCCLog glLog;

/*std::vector<std::string> stH2WCmdNameList =
{
    CMD_INVALID, CMD_CHECKSTATUS, CMD_ADDRESSSETTING, CMD_GETTAGDATA
};

std::unordered_map<std::string, char> stHeaderList =
{
    {HOST_HEADER,'H'},
    {WGT_HEADER,'W'},
};

std::unordered_map<std::string, char> stCmdList(
{
    {CMD_INVALID,' '},
    {CMD_CHECKSTATUS,'0'},
    {CMD_ADDRESSSETTING,'@'},
    {CMD_GETTAGDATA,'P'}
});

std::unordered_map<char, std::string> stStatusMap =
{
    {0x4e, "General Failure"},
    {0x59, "Success"},
    {0x01, "Request Address Setting"},
    {0x02, "Applying Address Information"},
    {0x03, "Address Receiving succeeds"},
    {0x04, "Address Distributed and in Setting"},
    {0x05, "Address Setting Succeeds"},
    {0x06, "No tag data o tag data reading fails"},
    {0x07, "Tag Reading succeeds"},
    {0x08, "Tag Data Ready"},
    {0x09, "Idle State"},
    {0x0a, "Battery no Power"},
};

std::unordered_map<char, ActionStruct> stActionMap =
{
    {0x4e, {CMD_CHECKSTATUS, 1000, false,  true,  true}},
    {0x59, {CMD_CHECKSTATUS,  100, false, false, false}},
    {0x01, {CMD_CHECKSTATUS,  100, false, false, false}},
    {0x02, {CMD_CHECKSTATUS,  100, false, false, false}},
    {0x03, {CMD_CHECKSTATUS,  100, false, false, false}},
    {0x04, {CMD_CHECKSTATUS,  100, false, false, false}},
    {0x05, {CMD_CHECKSTATUS,  100, false, false, false}},
    {0x06, {CMD_CHECKSTATUS,   50,  true, false, false}},
    {0x07, { CMD_GETTAGDATA,   50,  true, false, false}},
    {0x08, { CMD_GETTAGDATA,   50,  true, false, false}},
    {0x09, {CMD_CHECKSTATUS,   50, false, false, false}},
    {0x0a, {CMD_CHECKSTATUS, 1000, false,  true,  true}},
};

static std::unordered_map<std::string, int> stVariableIndexMap=
{
    {VAR_BATTERY_ALARM  , BatteryAlarm},
    {VAR_FAIL_STATUS    , FailStatus},
    {VAR_NOZZLE_ACTIVED , NozzleActived},
    {VAR_TAG_DETECTED   , TagDetected},
};

static std::unordered_map<int,int> stVariableThresHoldMap =
{
    {BatteryAlarm   , 2},
    {FailStatus     , 2},
    {NozzleActived  , 3},
    {TagDetected    , 2},
};*/


SCCFlowProtocol::SCCFlowProtocol() : m_pLast(m_chBufferIn), m_iBufferSize(0), m_iDataLen(0)
{
    //ctor
    /*memset(m_bAlarmVector,0, sizeof(bool)*MAX_CHANNELS);
    memset(m_bFailVector,0, sizeof(bool)*MAX_CHANNELS);
    memset(m_bNozzleActivedVector,0, sizeof(bool)*MAX_CHANNELS);
    memset(m_bTagDetected,0, sizeof(bool)*MAX_CHANNELS);

    memset(m_VarStatus, 0, sizeof(VarStatus) * MAX_CHANNELS*VariableName_size);
    for (int addr = 0; addr < MAX_CHANNELS; ++addr)
        for (int var = 0; var < VariableName_size; ++var)
            m_VarStatus[addr][var].iThresHold   = stVariableThresHoldMap[var];*/
}

SCCFlowProtocol::~SCCFlowProtocol()
{
    //dtor
}

/*std::string SCCFlowProtocol::getStrCmdStatusCheck(int addr,
                                               char* buffer,
                                               char& len)
{
    //return getStrCmd(CMD_CHECKSTATUS, addr, 0, buffer, len);
    return getCmdReadRegisters(addr, buffer, len, 0, 10);
}*/

/*std::string SCCFlowProtocol::getStrCmdSetAddr(int addr,
                                               int addr2,
                                               char* buffer,
                                               char& len)
{
    return getStrCmd(CMD_ADDRESSSETTING, addr, addr2, buffer, len);
}*/

/*std::string SCCFlowProtocol::getStrCmdGetTagId(int addr,
                                               char* buffer,
                                               char& len)
{
    return getStrCmd(CMD_GETTAGDATA, addr, 0, buffer, len);
}*/

/*std::string SCCFlowProtocol::getStrCmd(const std::string& cmd,
                                               int addr,
                                               int addr2,
                                               char* buffer,
                                               char& len)
{
    char* p = buffer;
    char* pBCC;

    *p++ = stHeaderList[HOST_HEADER];
    *p++ = 0x03;
    pBCC = buffer;
    *p++ = stCmdList[cmd];
    *p++ = ADDRESS_BYTE;
    *p++ = (char)addr;
    if (addr2 >0)
    {
        *p++ = ADDRESS_BYTE;
        *p++ = (char) addr2;
    }
    *p++ = ETX_BYTE;
    unsigned char chBCC = calcCRC((unsigned char*)pBCC, (unsigned char*)p);
    *p++ = (char)chBCC;
    *p = '\0';
    len = p - buffer;

    std::string msg((char*)buffer);

    return msg;
}*/

unsigned char SCCFlowProtocol::calcCRC(unsigned char* pFirst, unsigned char* pEnd)
{
    unsigned char ucBCC = '\0';

    for (unsigned char* p = pFirst; p != pEnd; ++p)
        ucBCC ^= *p;

    return ucBCC;
}

unsigned char SCCFlowProtocol::calcLRC(unsigned char* pFirst, unsigned char len)
{
    unsigned char ucLRC = 0;

    unsigned char* p = (unsigned char*)pFirst;

    for ( unsigned char i = 0; i < len ; i+=2)
    {
        unsigned char hi = *p++;
        ucLRC += asciiHexToDec(hi, *p++);
    }
    ucLRC ^= 0xff;
    ucLRC += 1;

    return ucLRC;
}

std::string SCCFlowProtocol::convChar2Hex(char* buffer, char& len)
{
    std::stringstream ss;

    for (int i = 0; i < len; ++i)
        ss << std::setfill('0') << std::setw(2) << std::hex << (int)(unsigned char)*buffer++; //<< " | ";
    //std::string msg;
    return std::string(ss.str());
}

/*bool SCCFlowProtocol::getWGTResponse(std::string& cmd,
                                        int& addr,
                                        char* resp,
                                        char& respLen)
{
    bool bCmd = false;

    do
    {
        char chHeader = stHeaderList[WGT_HEADER];
        auto itHeader = m_chBufferIn;
        for (; itHeader != m_pLast && *itHeader != chHeader; ++itHeader);

        if (itHeader == m_pLast)
        {
            m_pLast = m_chBufferIn;
            m_iBufferSize = 0;
            return false;
        }
        if (itHeader !=m_chBufferIn)
        {
            char offset = itHeader - m_chBufferIn;
            moveBufferToLeft(itHeader, offset);
        }

        if (m_iBufferSize < MIN_WGT_DATA)
            return false;

        char chCmd, chAddr, chLen, *buf;

        char* p = m_chBufferIn + 1;
        respLen = chAddr = *p++;
        if (respLen + 4 > m_iBufferSize)
        {
            moveBufferToLeft(m_chBufferIn+1,1);
            continue;
        }

        cmd = getWGTCommand((chCmd = *p++));
        if (*p != ADDRESS_BYTE)
        {
            moveBufferToLeft(m_chBufferIn+1,1);
            continue;
        }
        p++;

        addr = chAddr = *p++;

        buf = p;
        memcpy(resp, p, respLen - 3);
        chLen = respLen - 3;
        p += (respLen-3);
        if (*p != ETX_BYTE)
        {
            moveBufferToLeft(m_chBufferIn+1,1);
            continue;
        }
        ++p;
        unsigned char bcc = calcCRC((unsigned char*)m_chBufferIn, (unsigned char*)p);
        if ((unsigned char)*p != bcc)
        {
            moveBufferToLeft(m_chBufferIn+1,1);
            continue;
        }
        ++p;
        moveBufferToLeft(p, respLen + 4);
        addCommandToDvcMap(chCmd, chAddr, buf, chLen);
        bCmd = true;
    }
    while (bCmd == false && m_iBufferSize > 0);

    return bCmd;
}

bool SCCFlowProtocol::getWGTResponse(char* buffer,
                                        char len,
                                        std::string& cmd,
                                        int& addr,
                                        char* resp,
                                        char& respLen)
{
    if (len <= 0)
        return false;

    int newLen = len;
    if (m_iBufferSize + len > MAX_WGT_BUFFER_SIZE)
        newLen = MAX_WGT_BUFFER_SIZE - m_iBufferSize;

    if (newLen <= 0)
    {
        m_pLast = m_chBufferIn;
        m_iBufferSize = 0;
        newLen = len;
    }

    memcpy(m_pLast, buffer, len);
    m_iBufferSize += len;
    m_pLast += len;

    return getWGTResponse(cmd, addr, resp, respLen);
}

std::string SCCFlowProtocol::getWGTCommand(char cmd)
{
    for (auto it : stCmdList)
    {
        if (it.second == cmd)
            return it.first;
    }
    return CMD_INVALID;
}*/

void SCCFlowProtocol::moveBufferToLeft(char* pos, char len)
{
    if (len == 0 || pos == m_chBufferIn)
        return;

    if (len >= m_iBufferSize || pos == m_pLast)
    {
        m_iBufferSize = 0;
        m_pLast = m_chBufferIn;
        return;
    }

    memcpy(m_chBufferIn, pos, m_iBufferSize - len);
    m_iBufferSize -= len;
    m_pLast -= len;
}

/*std::string SCCFlowProtocol::getStrStatus(char status)
{
    return stStatusMap[status];
}*/

/*void SCCFlowProtocol::addCommandToDvcMap(char cmd, char addr, char* resp, char len)
{
    if (MAX_CHANNELS < addr)
        return;

    commandStruct *pCmdSt = &m_DeviceVector[addr-1]; // (cmd, addr, resp, len);
    pCmdSt->set(cmd, addr, resp, len);
}

bool SCCFlowProtocol::nextAction(int addr, char* buffer, char& len, int& timeout)
{
    if (MAX_CHANNELS < addr)
        return false;

    commandStruct CmdSt = m_DeviceVector[addr-1];

    bool res = false;

    if (CmdSt.command == stCmdList[CMD_CHECKSTATUS])
    {
        res = nextActionFromStatus(CmdSt, addr, buffer, len, timeout);
    }
    else if (CmdSt.command == stCmdList[CMD_ADDRESSSETTING])
    {
        res = nextActionFromAddressSetting(CmdSt, addr, buffer, len, timeout);
    }
    else if (CmdSt.command == stCmdList[CMD_GETTAGDATA])
    {
        res = nextActionFromGetTagData(CmdSt, addr, buffer, len, timeout);
    }

    return res;
}

bool SCCFlowProtocol::nextActionFromStatus(commandStruct& cmdSt, int addr, char* buffer, char& len, int& timeout)
{
    if (cmdSt.len <1)
        return false;

    addStatusToVector(addr, cmdSt);
    ActionStruct actionSt = getActionFromStatus(m_chStatusVector[addr-1]);
    getCommandFromAction(actionSt, addr, buffer, len);

    timeout = actionSt.iTimeOut;

    if (actionSt.bAlarm)
        setAlarm(addr);
    else
        clearAlarm(addr);

    if (actionSt.bFail)
        setFail(addr);
    else
        clearFail(addr);

    if (actionSt.bNozzleActived)
        setNozzleActivated(addr);
    else
    {
        clearNozzleActivated(addr);
        //clearTagDetected(addr);
    }

    if (getStatus(addr) != STATUS_TAG_READ_SUCCEEDS && getStatus(addr) != STATUS_TAG_DATA_READY)
        clearTagDetected(addr);
    else
        setTagDetected(addr);

    return true;
}

char SCCFlowProtocol::getStatus(char addr)
{
    if (addr > MAX_CHANNELS)
        return STATUS_FAILURE;
    return m_chStatusVector[addr-1];
}

bool SCCFlowProtocol::nextActionFromAddressSetting(commandStruct& cmdSt, int addr, char* buffer, char& len, int& timeout)
{
    ActionStruct actionSt(CMD_CHECKSTATUS, 1000, false, false, false);

    getCommandFromAction(actionSt, addr, buffer, len);
    timeout = actionSt.iTimeOut;
    return true;
}

bool SCCFlowProtocol::nextActionFromGetTagData(commandStruct& cmdSt, int addr, char* buffer, char& len, int& timeout)
{
    addTagDataToMap(cmdSt, addr);
    setTagDetected(addr);

    ActionStruct actionSt(CMD_CHECKSTATUS, 1000, false, false, false);

    getCommandFromAction(actionSt, addr, buffer, len);
    timeout = actionSt.iTimeOut;

    return true;
}

void SCCFlowProtocol::addStatusToVector(char addr, commandStruct& cmdSt)
{
    if (addr <1 || cmdSt.len < 1)
        return;

    if (MAX_CHANNELS < addr)
        return;

    m_chStatusVector[addr - 1] = cmdSt.data[0];
}

void SCCFlowProtocol::addTagDataToMap(commandStruct& cmdSt, char addr)
{
    auto it = m_TagDataMap.find(addr);
    if (it ==m_TagDataMap.end())
    {
        TagDataStruct tagDataSt(cmdSt.data, cmdSt.len);
        m_TagDataMap.insert(std::make_pair(addr, tagDataSt));
    }
    else
    {
        TagDataStruct tagDataSt(cmdSt.data, cmdSt.len);
        it->second = tagDataSt;
    }
}

void SCCFlowProtocol::getCommandFromAction(ActionStruct& actionSt,char addr, char* buffer, char& len)
{
    if (actionSt.strCmd == CMD_CHECKSTATUS)
        getStrCmdStatusCheck(addr, buffer, len);
    else if (actionSt.strCmd == CMD_ADDRESSSETTING)
        getStrCmdSetAddr(addr, addr, buffer, len);
    else if (actionSt.strCmd == CMD_GETTAGDATA)
        getStrCmdGetTagId(addr, buffer, len);
    else
    {
        *buffer = NULL_CHAR;
        len = 0;
    }
}*/

/*void SCCFlowProtocol::setVar(int addr, int var)
{
    VarStatus& v = m_VarStatus[addr][var];

    v.bCurrentStatus = true;
    v.iChangesCount = 0;
}

bool SCCFlowProtocol::clearVar(int addr, int var)
{
    VarStatus& v = m_VarStatus[addr][var];

    if (v.bCurrentStatus == true)
    {
        ++v.iChangesCount;
        if (v.iChangesCount >= v.iThresHold)
        {
            v.bCurrentStatus = false;
            return true;
        }
    }
    return false;
}

bool SCCFlowProtocol::isSetVar(int addr, int var)
{
    VarStatus& v = m_VarStatus[addr][var];

    return v.bCurrentStatus;
}


void SCCFlowProtocol::setAlarm(char addr)
{
    setVector(addr, m_bAlarmVector);
    setVar(addr, BatteryAlarm);
}

void SCCFlowProtocol::setNozzleActivated(char addr)
{
    setVector(addr, m_bNozzleActivedVector);
    setVar(addr, NozzleActived);
}

void SCCFlowProtocol::setFail(char addr)
{
    setVector(addr, m_bFailVector);
    setVar(addr, FailStatus);
}

void SCCFlowProtocol::setTagDetected(char addr)
{
    setVector(addr, m_bTagDetected);
    setVar(addr, TagDetected);
}


void SCCFlowProtocol::clearAlarm(char addr)
{
    if (clearVar(addr, BatteryAlarm))
        clearVector(addr, m_bAlarmVector);
}

void SCCFlowProtocol::clearNozzleActivated(char addr)
{
    if (clearVar(addr, NozzleActived))
        clearVector(addr, m_bNozzleActivedVector);
}

void SCCFlowProtocol::clearFail(char addr)
{
    if (clearVar(addr, FailStatus))
        clearVector(addr, m_bFailVector);
}

void SCCFlowProtocol::clearTagDetected(char addr)
{
    if (clearVar(addr, TagDetected))
        clearVector(addr, m_bTagDetected);
}

void SCCFlowProtocol::setVector(char addr, bool* vect)
{
    if (MAX_CHANNELS < addr)
        return;
    vect[addr-1] = true;
}

void SCCFlowProtocol::clearVector(char addr, bool* vect)
{
    if (MAX_CHANNELS < addr)
        return;
    vect[addr-1] = false;
}

bool SCCFlowProtocol::isVector(char addr, bool* vect)
{
    if (MAX_CHANNELS < addr)
        return false;

    return vect[addr-1];
}

ActionStruct SCCFlowProtocol::getActionFromStatus(char status)
{
    ActionStruct actionSt;
    switch(status)
    {
    case 0x4e:
        actionSt = {CMD_CHECKSTATUS, 1000, false,  true,  true};
        break;
    case 0x59:
        actionSt = {CMD_CHECKSTATUS, 1000, false, false, false};
        break;
    case 0x01:
        actionSt = {CMD_CHECKSTATUS, 1000, false, false, false};
        break;
    case 0x02:
        actionSt = {CMD_CHECKSTATUS, 1000, false, false, false};
        break;
    case 0x03:
        actionSt = {CMD_CHECKSTATUS, 1000, false, false, false};
        break;
    case 0x04:
        actionSt = {CMD_CHECKSTATUS, 1000, false, false, false};
        break;
    case 0x05:
        actionSt = {CMD_CHECKSTATUS, 1000, false, false, false};
        break;
    case 0x06:
        actionSt = {CMD_CHECKSTATUS, 1000,  true, false, false};
        break;
    case 0x07:
        actionSt = { CMD_GETTAGDATA, 1000,  true, false, false};
        break;
    case 0x08:
        actionSt = { CMD_GETTAGDATA, 1000,  true, false, false};
        break;
    case 0x09:
        actionSt = {CMD_CHECKSTATUS, 1000, false, false, false};
        break;
    case 0x0a:
        actionSt = {CMD_CHECKSTATUS, 1000, false,  true,  true};
        break;
    default:
        actionSt = {CMD_CHECKSTATUS, 1000, false, false, false};
        break;
    }
    return actionSt;
}*/

std::string SCCFlowProtocol::printStatus(char addr, bool addStrData)
{
    if (addr > MAX_CHANNELS)
        return "";

    //FlowRegisters& reg  = m_Register[(unsigned char)(addr-1)];

    std::stringstream ss;

    ss << FRAME_START_MARK ;

    ss << MSG_HEADER_TYPE << ASSIGN_CHAR << DEVICE_OVALGEARFLOWM;

    /*ss << SEPARATOR_CHAR << VAR_INSTANTFLOWRATE 		<< ASSIGN_CHAR << reg.m_dInstantFlowRate;
    ss << SEPARATOR_CHAR << VAR_FLUIDSPEED 				<< ASSIGN_CHAR << reg.m_dFluidSpeed;
    ss << SEPARATOR_CHAR << VAR_MEASUREFLUIDSOUNDSPEED 	<< ASSIGN_CHAR << reg.m_dMeasureFluidSoundSpeed;
    ss << SEPARATOR_CHAR << VAR_POSACUMFLOWRATE 		<< ASSIGN_CHAR << reg.m_lPosAcumFlowRate;
    ss << SEPARATOR_CHAR << VAR_POSACUMFLOWRATEDECPART 	<< ASSIGN_CHAR << reg.m_dPosAcumFlowRateDecPart;
    ss << SEPARATOR_CHAR << VAR_NEGACUMFLOWRATE 		<< ASSIGN_CHAR << reg.m_lNegAcumFlowRate;
    ss << SEPARATOR_CHAR << VAR_NEGACUMFLOWRATEDECPART 	<< ASSIGN_CHAR << reg.m_dNegAcumFlowRateDecPart;
    ss << SEPARATOR_CHAR << VAR_NETACUMFLOWRATE 		<< ASSIGN_CHAR << reg.m_lNetAcumFlowRate;
    ss << SEPARATOR_CHAR << VAR_NETACUMFLOWRATEDECPART 	<< ASSIGN_CHAR << reg.m_dNetAcumFlowRateDecPart;*/

    ss << SEPARATOR_CHAR << VAR_INSTANTFLOWRATE 		<< ASSIGN_CHAR << m_EH6400ARegister.InstantValue.i32Value;
    ss << SEPARATOR_CHAR << VAR_POSACUMFLOWRATE 		<< ASSIGN_CHAR << m_EH6400ARegister.TotalCumulativeHigh.i32Value;
    ss << SEPARATOR_CHAR << VAR_POSACUMFLOWRATEDECPART 	<< ASSIGN_CHAR << m_EH6400ARegister.TotalCumulativeLow;
    ss << SEPARATOR_CHAR << VAR_NETACUMFLOWRATE 		<< ASSIGN_CHAR << m_EH6400ARegister.OneTimeHigh.i32Value;
    ss << SEPARATOR_CHAR << VAR_NETACUMFLOWRATEDECPART 	<< ASSIGN_CHAR << m_EH6400ARegister.OneTimeLow;

    if (addStrData)
    {
        ss << SEPARATOR_CHAR << "strData" << ASSIGN_CHAR << m_strData;
        ss << SEPARATOR_CHAR << "data_len" << ASSIGN_CHAR << m_iDataLen;
    }

    ss << FRAME_STOP_MARK;

    return std::string(ss.str());
}

/*bool SCCFlowProtocol::isAlarm(char addr)
{
    return isVector(addr, m_bAlarmVector);
}

bool SCCFlowProtocol::isFail(char addr)
{
    return isVector(addr, m_bFailVector);
}

bool SCCFlowProtocol::isNozzleActived(char addr)
{
    return isVector(addr, m_bNozzleActivedVector);
}

bool SCCFlowProtocol::isTagDetected(char addr)
{
    return isVector(addr, m_bTagDetected);
}*/

std::string SCCFlowProtocol::boolToString(bool b, const std::string& valTrue, const std::string& valFalse)
{
    if (b ==true)
    {
        if (valTrue != "")
            return valTrue;
        return "true";
    }
    if (valFalse != "")
        return  valFalse;
    return "false";
}

/*bool SCCFlowProtocol::getTagId(char addr, char* tagBuffer, char& len)
{
    auto it = m_TagDataMap.find(addr);
    if (it ==m_TagDataMap.end())
    {
        len = 0;
        tagBuffer[0] = NULL_CHAR;
        return false;
    }
    else
    {
        TagDataStruct& tagDataSt = it->second;
        len = tagDataSt.chLenData;
        memcpy(tagBuffer, tagDataSt.chTagData, tagDataSt.chLenData);
    }
    return true;
}*/

std::string SCCFlowProtocol::getCmdReadRegisters(char addr,
                                    char* buffer,
                                    char& len,
                                    unsigned int startRegister,
                                    unsigned int numRegisters)
{
    std::string msg; //(":01030000000AF2");
    //msg += START_BYTE;
    //
    //(": | 01 | 03 | 0000 | 000A | F2");

    msg += numToAscii(addr, 2);
    msg += numToAscii(RTU_CMD_READ, 2);
    msg += numToAscii(startRegister, 4);
    msg += numToAscii(0, 2);
    msg += numToAscii(numRegisters, 2);

    msg += "C5CD"; //numToAscii(calcLRC(buf, msg.length() - 1), 2);
    //unsigned char buf[msg.length()+1];
    //memcpy(buf, msg.c_str(), msg.length());
    //msg += numToAscii(calcCRC(buf, buf + msg.length()), 2);

    //unsigned char buf[msg.length()];
    //memcpy(buf, msg.substr(1).c_str(), msg.length());

    //msg += CR_CHAR;
    //msg += LF_CHAR;

    memcpy(buffer, msg.c_str(), msg.length());
    buffer[msg.length()] = NULL_CHAR;

    len = msg.length();

    unsigned char* p = (unsigned char*)buffer;

    *p++ = addr;
    *p++ = RTU_CMD_READ;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x0a;
    *p++ = 0xC5;
    *p++ = 0xCD;
    *p++ = 0x00;
    len = msg.length() / 2;
    return msg;
}

unsigned char SCCFlowProtocol::asciiHexToDec(const char hexHi, const char hexLo)
{
    unsigned char hi, lo;

    hi = asciiHexToDec(hexHi);
    lo = asciiHexToDec(hexLo);

    lo += (hi << 4);

    return lo;
}

unsigned char SCCFlowProtocol::asciiHexToDec(const char hex)
{
    unsigned char d;

    d = hex - 48;

    if (d > 9)
        d -= 7;

    return d;
}

unsigned char SCCFlowProtocol::asciiHexToDec(const char* hex)
{
    return asciiHexToDec(*hex, *(hex+1));
}

bool SCCFlowProtocol::getFlowMeterResponse(char addr, char* buffer, char len)
{
    return readRTUDataEH6400A(addr, buffer, len);
    /*char* p = buffer;

    if (checkLRC(p, len-5))
        return false;
    if (checkAddress(addr, p++))
        return false;
    ++p;
    if (checkCommand(RTU_CMD_READ, p++))
        return false;
    ++p;
    char count = asciiHexToDec(p++);
    ++p;
    if (count != len - 11)
        return false;
    readRTUData(addr, p, count);
    return true;*/
}

bool SCCFlowProtocol::checkAddress(char addr, char* frame)
{
    return compareValueToBuffer(addr, frame, 2);
}

bool SCCFlowProtocol::checkCommand(char cmd, char* frame)
{
    return compareValueToBuffer(cmd, frame, 2);
}

bool SCCFlowProtocol::compareValueToBuffer(unsigned char val, char* frame, size_t len)
{
    char buf[len*2];
    numToAscii(val, buf, len*2);
    return (memcmp(buf, frame, len*2) == 0);
}

bool SCCFlowProtocol::checkLRC(char* pFirst, size_t len)
{
    unsigned char lrc = calcLRC((unsigned char*)pFirst, len);
    return compareValueToBuffer(lrc, pFirst+len, 2);
}

void SCCFlowProtocol::readRTUData(char addr, char* pFirst, size_t len)
{
    putData(pFirst, len);
    char* pSrc = pFirst;
    unsigned char* pDst = (unsigned char*)&m_RawData[(unsigned char)(addr-1)].fRegister[0];
    const size_t sizeRawData = sizeof(m_RawData[(unsigned char)(addr-1)].fRegister[0]);

    for (size_t i = 0; i < MAX_REGISTERS/2 ; ++i)
    {
        asciiHexToFloat(pDst, pSrc, sizeRawData);
        pDst += sizeRawData;
        pSrc += sizeRawData*2;
    }

    RawData& rawDat     = m_RawData[(unsigned char)(addr-1)];
    FlowRegisters& reg  = m_Register[(unsigned char)(addr-1)];
    //size_t i = 0;

    //rawToReal4(p, reg.m_dInstantFlowRate, 2);
    reg.m_dInstantFlowRate          = rawDat.fRegister[D_INSTANTFLOWRATE];
    reg.m_dFluidSpeed               = rawDat.fRegister[D_FLUIDSPEED];
    reg.m_dMeasureFluidSoundSpeed   = rawDat.fRegister[D_MEASUREFLUIDSOUNDSPEED];
    reg.m_lPosAcumFlowRate          = rawDat.lRegister[L_POSACUMFLOWRATE];
    reg.m_dPosAcumFlowRateDecPart   = rawDat.fRegister[D_POSACUMFLOWRATEDECPART];
    reg.m_lNegAcumFlowRate          = rawDat.lRegister[L_NEGACUMFLOWRATE];
    reg.m_dNegAcumFlowRateDecPart   = rawDat.fRegister[D_NEGACUMFLOWRATEDECPART];
    reg.m_lNetAcumFlowRate          = rawDat.lRegister[L_NETACUMFLOWRATE];
    reg.m_dNetAcumFlowRateDecPart   = rawDat.fRegister[D_NETACUMFLOWRATEDECPART];

}

bool SCCFlowProtocol::readRTUDataEH6400A(char addr, char* pFirst, size_t len)
{
    //putData(pFirst, len);
    char* pSrc = pFirst;

    HeaderRegEH6400A* pHeader = (HeaderRegEH6400A*) pFirst;
    ++pSrc;
    ++pSrc;
    ++pSrc;

    FlowRegEH6400A* pData = (FlowRegEH6400A*) (pFirst+3);
    //FlowRegEH6400A flowVal = *pData;
    //int16_t* CalibratioCode1 = (int16_t*) (pFirst+23);
    CalibrationCodeEH6400A* caliCode = (CalibrationCodeEH6400A*) (pFirst+23);
    //flowVal.swap_order();

    if (pHeader->address == 0x01 &&
        pHeader->functionCode == 0x03 &&
        pHeader->length == 0x14)
    {
        m_EH6400ARegister   = *pData;
        m_EH6400ARegister.swap_order();
        /*FlowRegEH6400A flowData;
        binData* pD = (binData*)pSrc;
        flowData.OneTimeHigh = pD->intData;
        ++pD;
        flowData.OneTimeLow = pD->floatData;
        ++pD;
        flowData.TotalCumulativeHigh = pD->intData;
        ++pD;
        flowData.TotalCumulativeLow = pD->floatData;
        ++pD;
        flowData.InstantValue = pD->intData;
        ++pD;
        int sz = sizeof(*pD);*/
    }
    if ( caliCode->code1 == 0xbb && caliCode->code2 == 0xd4)
           return true;
    return true;
}

void SCCFlowProtocol::asciiHexToFloat(unsigned char* pDst, char* pSrc, size_t bytes)
{
    pSrc += ((bytes-1)*2);
    for (size_t i = 0; i < bytes; ++i)
    {
        *pDst++ = asciiHexToDec(pSrc);
        pSrc -= 2;
    }
}

void SCCFlowProtocol::asciiToReal4(char* p, double& val, char num)
{
}

void SCCFlowProtocol::putData(char* p, char num)
{
    //std::cout << "Data: ";
    m_iDataLen = num;
    *(p+num) = NULL_CHAR;

    m_strData = p;
    return;
}

void SCCFlowProtocol::printData()
{
    std::cout << "Data: " << m_strData << std::endl;

    const size_t sizeRawData = sizeof(float);
    size_t sz = m_iDataLen / (sizeRawData*2);

    if (sz)
    {
        float       fReg[sz];
        int32_t     lReg[sz];

        char* pSrc = (char*)m_strData.c_str();
        unsigned char* pDst1 = (unsigned char*)&fReg;
        unsigned char* pDst2 = (unsigned char*)&lReg;

        for (size_t i = 0; i < sz ; ++i)
        {
            asciiHexToFloat(pDst1, pSrc, sizeRawData);
            asciiHexToFloat(pDst2, pSrc, sizeRawData);
            pDst1 += sizeRawData;
            pDst2 += sizeRawData;
            pSrc += sizeRawData*2;
        }

        for (size_t i = 0; i < sz ; ++i)
        {
            std::cout << "float: " << fReg[i];
            std::cout << ", Long: " << lReg[i];
            std::cout << std::endl;
        }
    }
    else
    {
        const size_t sizeData = sizeof(int16_t);
        sz = m_iDataLen / (sizeData*2);

        if (sz)
        {
            int16_t     lReg[sz];

            char* pSrc = (char*)m_strData.c_str();
            unsigned char* pDst2 = (unsigned char*)&lReg;

            for (size_t i = 0; i < sz ; ++i)
            {
                asciiHexToFloat(pDst2, pSrc, sizeData);
                pDst2 += sizeData;
                pSrc += sizeData*2;
            }

            for (size_t i = 0; i < sz ; ++i)
            {
                std::cout << "Long: " << lReg[i];
                std::cout << std::endl;
            }
        }
    }
}

