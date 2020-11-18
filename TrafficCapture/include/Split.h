/***********************************************************
 * Author: Wen Li
 * Date  : 11/10/2020
 * Describe: traffic flow split
 * History:
   <1> 11/10/2020 , create
************************************************************/
#ifndef _SPLIT_H_
#define _SPLIT_H_
#include <BasicType.h>
#include <pcap.h>
#include <Packet.h>
#include <Context.h>

using namespace std;

#define CAP_LEN  (2048)

class Split
{
protected:
    string m_PcapFile;
    DWORD m_IsCsv;
    

public:
    
    static FlowManage m_Flm;
    Split (string FileName, DWORD IsCsv=0)
    {
        m_IsCsv = IsCsv;
        m_PcapFile = FileName;
        LoadPcap (FileName.c_str ());
    }

    VOID SplitFlows();

private:
    VOID LoadPcap (const char *PcapFile);
    VOID Dump (Flow *F);    
};



#endif 
