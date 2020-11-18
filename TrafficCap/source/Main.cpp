/***********************************************************
 * Author: Wen Li
 * Date  : 4/14/2020
 * Describe: traffic capture
 * History:
   <1> 11/10/2020 , create
************************************************************/
#include "Capture.h"
#include "FlowCsv.h"
#include "Split.h"
#include "FlowTable.h"

FlowTable FT;
FlowManage m_Flmange;
T_IPSet *IpSet;

VOID Help ()
{
    printf("************************************************\r\n");
    printf("*                help information              *\r\n");
    printf("************************************************\r\n");
    printf("-d <device name> \r\n");
    printf("************************************************\r\n\r\n");

    return;
}

static inline T_IPSet* GetUserIpSet ()
{
    char IPaddr[128];
    static T_IPSet UserIp;
    
    FILE *F = fopen (USER_IP_PROPERTITY, "r");
    if (F == NULL)
    {
        UserIp.insert (ntohl(inet_addr("192.168.159.130")));
    }
    else
    {
        while (!feof(F))
        {
            char *Ret = fgets (IPaddr, sizeof (IPaddr), F);
            if (Ret == NULL)
            {
                break;
            }
            
            UserIp.insert (ntohl(inet_addr(IPaddr)));
        }

        fclose (F);
    }

    return &UserIp;
}



void *F2CsvThread (void* Arg)
{
    FlowCsv FC (&m_Flmange, &FT);
    while (1)
    {
        sleep (60);
        
        FC.Dump ();
    }
    
    return NULL;
}

void *UpdateFlowTable (void* Arg)
{
    while (1)
    {
        sleep (10);
        
        FT.UpdateFlowTable ();
    }
    
    return NULL;
}



int main(int argc, char *argv[])
{
    char ch;
    pthread_t Tid;
    string Device = "";
    string File = "";
    DWORD IsCsv = 0;
    
    while((ch = getopt(argc, argv, "d:s:ch")) != -1)
    {
        switch(ch)
        {
            case 'd':
            {
                Device = optarg;
                break;
            }
            case 's':
            {
                File = optarg;
                break;
            }
            case 'c':
            {
                IsCsv = 1;
                break;
            } 
            default:
            {
                Help ();
                return 0;
            }
        }
    }

    IpSet = GetUserIpSet ();

    if (File != "")
    {
        Split Sp (File, IsCsv);
        Sp.SplitFlows ();
        return 0;
    }

    if (Device == "")
    {
        Help ();
        return 0;
    }

    int Ret = pthread_create(&Tid, NULL, F2CsvThread, NULL);
    assert (Ret == 0);

    Ret = pthread_create(&Tid, NULL, UpdateFlowTable, NULL);
    assert (Ret == 0);
           
    Capture Cap(Device);
    Cap.CapturePacket ();
	
	return 0;
}

