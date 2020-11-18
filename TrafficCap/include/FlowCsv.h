/***********************************************************
 * Author: Wen Li
 * Date  : 11/10/2020
 * Describe: traffic flow split
 * History:
   <1> 11/10/2020 , create
************************************************************/
#ifndef _FLOWCSV_H_
#define _FLOWCSV_H_
#include <BasicType.h>
#include <Context.h>
#include <iostream> 
#include <fstream>
#include "FlowTable.h"


using namespace std;

#define BUF_LEN (4096)


class FlowCsv
{
protected:
    FlowManage *m_Flm;
    FlowTable *m_Ft;


public:

    FlowCsv (FlowManage *Flm, FlowTable *Ft)
    {
        m_Flm = Flm;
        m_Ft  = Ft;
    }

    VOID Dump ()
    {
        DWORD DumpNum = 0;
        BYTE* Buffer = (BYTE* )malloc (16 * 1024);
        assert (Buffer != NULL);

        FILE* CsvFile;
        string FileName = "CSVs/" + to_string (time(NULL)) + ".csv";
	    CsvFile = fopen (FileName.c_str(), "w");
        assert (CsvFile != NULL);
        
        for (auto It = m_Flm->begin (); It != m_Flm->end (); It++)
        {
            Flow *F = *It;
            if (F->m_Fin != 0)
            {
                continue;
            }
       
            if (F->m_SduNum < CAPTURE_NUM)
            {
                continue;
            }
            //printf ("F->m_SduNum = %u, Age = %u, Packet Num = %u \r\n", F->m_SduNum, Age, F->m_PakcetData.size());

            /* flow name: src_srcport_dst_dst_port_protocol */
            snprintf ((char *)Buffer, BUF_LEN, "%.2X_%.2X_%.2X_%.2X_%.2X", 
                      F->m_SrcIp, F->m_SrcPort, F->m_DstIp, F->m_DstPort, F->m_ProtoType);
            fprintf (CsvFile, "%s,", Buffer);

            string ProName = "None";
            if (m_Ft != NULL)
            {
                ProName = m_Ft->GetFlowProc (F->m_SrcIp, F->m_DstIp, F->m_SrcPort, F->m_DstPort, F->m_ProtoType);
            }
            fprintf (CsvFile, "%s,", ProName.c_str());

            /* write packet data */
            for (auto pIt = F->m_PakcetData.begin (); pIt != F->m_PakcetData.end (); pIt++)
            {
                IpPacket *Packet = *pIt;

                memset (Buffer, 0, BUF_LEN);
                DWORD offset = 0;
                while (offset < Packet->m_PayloadLen)
                {
                    CHAR Value[8];
                    snprintf (Value, sizeof (Value), "%.2X ", Packet->m_Payload[offset]);
                    strcat ((char*)Buffer, Value);
                    offset++;
                    if (offset >= FORMAT_LEN)
                    {
                        break;
                    }
                }

                while (offset < FORMAT_LEN)
                {
                    CHAR Value[8];
                    snprintf (Value, sizeof (Value), "%.2X ", 0);
                    strcat ((char*)Buffer, Value);
                    offset++;
                }

                fprintf (CsvFile, "%s,", Buffer);
            }

            fprintf (CsvFile, "\r\n");

            F->m_Fin = 1;
            F->DelPackets ();
            DumpNum++;
        }

        delete Buffer;
        if (DumpNum == 0)
        {
            remove (FileName.c_str());
        }
        else
        {
            cout<<"Dump "<<DumpNum <<" flows into "<<FileName<<"\r\n";
        }

        fclose(CsvFile);    
        return;
    }
};



#endif 
