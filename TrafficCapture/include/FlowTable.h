/***********************************************************
 * Author: Wen Li
 * Date  : 11/17/2020
 * Describe: static flow table
 * History:
   <1> 11/10/2020 , create
************************************************************/
#ifndef _FLOWTABLE_H_
#define _FLOWTABLE_H_
#include <BasicType.h>
#include <Context.h>
#include <iostream> 
#include <fstream>

using namespace std;

struct ProcInfo
{
    DWORD m_ID;
    string m_Name;

    set<DWORD> Socket;

    ProcInfo (DWORD ID, string Name)
    {
        m_ID = ID;
        m_Name = Name;
    }
};

class FlowTable
{
protected:
    FlowManage m_Flm;
    map<DWORD, ProcInfo*> m_PID2Proc;


public:

    FlowTable ( )
    {

    }

    VOID LoadTcpInfo(map<DWORD, Flow*>& Inode2Flow);
    VOID GetProcessId();
    VOID GetProSockets();

    DWORD UpdateFlowTable();

    string GetFlowProc (DWORD SrcIp, DWORD DstIp, WORD SrcPort, WORD DstPort, DWORD ProtoType);

};



#endif 
