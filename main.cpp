#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <sstream>
#include <algorithm>

#include "SCCFlowProtocol.h"

#include "../commPort/SCCCommPort.h"
#include "../commPort/SCCRealTime.h"
#include "../commPort/SCCArgumentParser.h"

#include "../main_control/CSocket.h"
#include "../main_control/SCCDeviceNames.h"
#include "../main_control/SCCLog.h"
#include "../main_control/SCCDeviceParams.h"


using namespace std;

#define MAX_BUFFER_IN   2048
#define MY_DEVICE_NAME  DEVICE_OVALGEARFLOWM

static bool st_bSendMsgView = false;
static bool st_bRcvMsgView  = true;

SCCCommPort commPort;
CSocket sckComPort;
SCCLog globalLog(std::cout);
bool gl_bVerbose(true);

bool bConnected     = false;

std::string firstMessage()
{
    std::stringstream ss;

    ss << FRAME_START_MARK;
    ss << DEVICE_NAME << ":" << MY_DEVICE_NAME << ",";
    ss << SERVICE_PID << ":" << getpid();
    ss << PARAM_COM_PORT << ":" << commPort.getComPort();
    ss << FRAME_STOP_MARK;

    return std::string(ss.str());
}

void printMsg(const std::string& msg = "")
{
    if (sckComPort.isConnected())
    {
        if (bConnected == false)
        {
            bConnected = true;
            sckComPort.sendData(firstMessage());
            std::cout << "Socket connected." << std::endl;
        }
        sckComPort.sendData(msg);
    }
    else
    {
        std::cout << SCCRealTime::getTimeStamp() << ',' << msg << std::endl;
    }
}

int main(int argc, char* argv[])
{
    std::string         nPort;
    int baudRate        = 9600;
    float fTimeFactor   = 1.0;
    int remotePort      = 0;
    int startReg        = 0;
    int numRegs         = MAX_REGISTERS;
    char chBufferIn[MAX_BUFFER_IN];
    size_t posBuf       = 0;
    bool bOneTime       = false;

    if (argc > 2)
    {
        nPort = argv[1];
        baudRate = std::stoi(argv[2]);
        if (argc > 3)
            remotePort = std::stoi(argv[3]);
        if (argc > 4)
        {
            std::string strArg(argv[4]);
            if (std::all_of(strArg.begin(), strArg.end(), ::isdigit))
                fTimeFactor = std::stof(argv[4]);
            else
                if (strArg == "ViewSend")
                    st_bSendMsgView = true;
        }
        if ( argc > 6)
        {
            startReg    = std::stoi(argv[5]);
            numRegs     = std::stoi(argv[6]);
        }
        if (argc > 7)
        {
            std::string strArg(argv[7]);
            if (strArg == "true")
                bOneTime = true;
        }
    }

    if (remotePort)
    {
        sckComPort.connect("127.0.0.1", remotePort);
        bConnected  = false;
    }

    commPort.setArgs(argc,&argv[0]);
    commPort.setDeviceName(MY_DEVICE_NAME);
    SCCFlowProtocol flowProtocol;
    SCCRealTime clock;

    //commPort.openPort(nPort, baudRate);

    char bufferOut[255];
    char bufferIn[250];
    //char len;
    char chLen = 0;
    int iAddr = 1;
    std::string msg;

    //msg = flowProtocol.getStrCmdStatusCheck(iAddr, bufferOut, len);
    msg = flowProtocol.getCmdReadRegisters(iAddr, bufferOut, chLen, startReg, numRegs);

    //flowProtocol.getCmdReadRegisters(iAddr, bufferOut, len);

    msg = flowProtocol.convChar2Hex(bufferOut, chLen);

    if (st_bSendMsgView)
        std::cout << "Message: " << bufferOut << " sent." << std::endl;

    //std::queue<int> comPortQueue;
    commPort.setBaudRate(baudRate);
    commPort.getComPortList(nPort);

    /*commPort.sendData(bufferOut, len);
    commPort.sleepDuringTxRx(len+numRegs*4+11);*/

    if (st_bSendMsgView)
        cout << "Waiting for response" << std::endl;
    msg = "";

    int iTimeOut;
    bool bNextAddr;
    //char chLenLast = 0;
    int iNoRxCounter = 0;
    do
    {
        /*if (!bConnected && sckComPort.isConnected())
            printMsg();*/
        bNextAddr = true;
        iTimeOut = 250;
        if (iNoRxCounter >= 5)
        {
            iNoRxCounter = 0;
            //flowProtocol.getStrCmdStatusCheck(iAddr, bufferOut, chLen);
            flowProtocol.getCmdReadRegisters(iAddr, bufferOut, chLen, startReg, numRegs);
        }
        if (chLen > 0)
        {
            if (!commPort.isDeviceConnected() && !commPort.searchNextPort())
            {
                if (!commPort.isDeviceConnected())
                    globalLog << "Device No Connected. Quit Program" << std::endl;
                else
                    globalLog << "Response Time out. Quit Program" << std::endl;
                break;
            }
            /*while(!comPortQueue.empty())
            {
                int nPort = comPortQueue.front();
                commPort.closePort();
                bool bOpened = commPort.openPort(nPort, baudRate);
                comPortQueue.pop();
                if (bOpened)
                    break;
            }*/
            if (st_bSendMsgView)
            {
                cout << commPort.printCounter() << std::endl;
                msg = flowProtocol.convChar2Hex(bufferOut, chLen);
                //msg = flowProtocol.con(bufferOut, chLen);
                cout << SCCRealTime::getTimeStamp() << ',' << "Sending Message: " << bufferOut << std::endl;
            }
            commPort.sendData(bufferOut, chLen);
            commPort.sleepDuringTxRx(chLen+numRegs*4+11);
            //chLenLast = chLen;
            chLen = 0;
            //iTimeOut = 20;
            bNextAddr = false;
        }
        if (commPort.isRxEvent() == true)
        {
            iNoRxCounter = 0;
            bNextAddr =false;
            int iLen;
            bool ret = commPort.getData(bufferIn, iLen);
            if (st_bSendMsgView)
            {
                //cout << " bufferIn.len(): " << iLen << ". bufferIn(char): [" << bufferIn << "]" << std::endl;
                char len = (char) iLen;
                cout << ". bufferIn(char): [" << flowProtocol.convChar2Hex(bufferIn, len) << "]" << std::endl;
            }
            if (ret == true)
            {
                if (posBuf + iLen < MAX_BUFFER_IN)
                {
                    memcpy(&chBufferIn[posBuf], bufferIn, iLen);
                    posBuf += iLen;
                }
                //len = (char)iLen;
                /*if (st_bRcvMsgView)
                {
                    msg = flowProtocol.convChar2Hex(bufferIn, len);
                    cout << ++nCount << " Buffer In(Hex): [" << msg << "]. Buffer In(char): [" << bufferIn << "]" << std::endl;
                }*/
                std::string strCmd;
                //char resp[256];
                //int addr = iAddr;
                //char respLen = 0;
                bool bIsValidResponse = flowProtocol.getFlowMeterResponse(iAddr, chBufferIn, posBuf);
                //bool bNextAction = false;
                if (bIsValidResponse == true)
                {
                    /*while(!comPortQueue.empty())
                    {
                        comPortQueue.pop();
                    }*/
                    commPort.setDeviceConnected();
                    commPort.stopSearchPort();
                    posBuf = 0;
                    //iTimeOut = 50;
                    if (st_bRcvMsgView)
                    {
                        //cout << ++nCount << " " << commPort.printCounter() << clock.getTimeStamp() << " Valid WGT Response" << std::endl;
                        /*if (strCmd == CMD_CHECKSTATUS)
                            cout << ++nCount << " WGT Status: " << flowProtocol.getStrStatus(resp[0]) << endl;*/
                    }
                    /*bNextAction = flowProtocol.nextAction(iAddr, bufferOut, chLen, iTimeOut);
                    if (bNextAction == true)*/
                        if (st_bRcvMsgView)
                        {
                            //std::stringstream ss;
                            //ss << ++nCount << " " << commPort.printCounter() << flowProtocol.printStatus(iAddr) << std::endl;
                            printMsg(flowProtocol.printStatus(iAddr));
                        }
                    if (st_bSendMsgView)
                        flowProtocol.printData();
                    if (bOneTime)
                        break;
                }
                else
                {
                    iTimeOut = 0;
                    commPort.sleepDuringTxRx(numRegs*4+11-posBuf);
                }
            }
        }
        if (iTimeOut > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(iTimeOut*fTimeFactor)));
        if (bNextAddr == true)
        {
            //++iAddr;
        }
        if (/*bConnecting ==true || */bConnected == true)
        {
            if (sckComPort.getState() == sckError)
            {
                if (remotePort)
                {
                    sckComPort.connect("127.0.0.1", remotePort);
                    //bConnecting = true;
                    bConnected  = false;
                    //iSckCounter = 0;
                }
            }
            /*if (sckComPort.isConnected())
            {
                //bConnecting = false;
                bConnected  = true;
            }*/
        }
        ++iNoRxCounter;
        if (bConnected == true && !sckComPort.isConnected())
            break;
    }
    while (commPort.isOpened());

    sckComPort.disconnect();
    commPort.closePort();
    exit(0);

    return 0;
}
