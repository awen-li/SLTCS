/***********************************************************
 * Author: Wen Li
 * Date  : 11/17/2020
 * Describe: static flow table
 * History:
   <1> 11/17/2020 , create
************************************************************/
#include "FlowTable.h"


VOID FlowTable::GetProcessId()
{
	DIR *Dir;
	struct dirent *SD;

    Dir = opendir("/proc");
	if (Dir == NULL)
	{
        printf ("opendir fail...\r\n");
        return;
	}

	while (SD = readdir(Dir))
	{
		if (SD->d_name[0] > '0' && SD->d_name[0] <= '9')
		{
            DWORD PID = atoi(SD->d_name);
            if (PID < 1024)
            {
                continue;
            }
            
            FILE* PFile;
            CHAR Buffer[256] = {0}; 
            sprintf(Buffer,"/proc/%u/status", PID);
            PFile = fopen (Buffer, "r");
            if (PFile == NULL)
            {
                continue;
            }

            CHAR* Result = fgets (Buffer, sizeof (Buffer), PFile);
            if (Result == NULL)
            {
                continue;
            }
            CHAR ProcName[128];
            sscanf(Buffer, "Name: %s", ProcName);
            
            auto It = m_PID2Proc.find (PID);
            if (It != m_PID2Proc.end ())
            {
                ProcInfo* Pi = It->second;
                if (Pi->m_Name == string (ProcName))
                {
                    continue;
                }
            }

            ProcInfo *Pi = new ProcInfo (PID, string (ProcName));
            assert (Pi != NULL);
            m_PID2Proc[PID] = Pi;
            //printf ("Update Process: Id = %u, Name = %s \r\n", PID, ProcName);
		}
	}

    closedir (Dir);
    
	return;
}


VOID FlowTable:: GetProSockets()
{
    CHAR Buffer[256];
    for (auto It = m_PID2Proc.begin (); It != m_PID2Proc.end (); It++)
    {
        DIR *Dir;
    	struct dirent *SD;
        DWORD PID = It->first;

        sprintf(Buffer, "/proc/%u/fd", PID);
        Dir = opendir(Buffer);
    	if (Dir == NULL)
    	{
            //printf ("opendir %s fail...\r\n", Buffer);
            continue;
    	}

    	while (SD = readdir(Dir))
    	{
            char Link[256];
            memset (Link, 0, sizeof (Link));
            
            sprintf(Buffer, "/proc/%u/fd/%s", PID, SD->d_name);
            ssize_t L = readlink(Buffer, Link, sizeof (Link));
            if (L == 0 || strncmp (Link, "socket", sizeof("socket")-1) != 0)
            {
                continue;
            }
            
            DWORD Socket;
            sscanf(Link, "socket:[%u]", &Socket);
            //printf ("%s Link to: %s -> %u\r\n", SD->d_name, Link, Socket);

            ProcInfo *PI = It->second;
            PI->Socket.insert (Socket);
    	}

        closedir(Dir);
    }
    
    
}

VOID FlowTable::LoadTcpInfo(map<DWORD, Flow*>& Inode2Flow)
{
    FILE* PFile;
    CHAR Buffer[256] = {0}; 

    PFile = fopen ("/proc/net/tcp", "r");
    if (PFile == NULL)
    {
        printf ("open tcp file fail...\r\n");
        return;
    }

    
    while (!feof (PFile))
    {
        /* 0: 0100007F:024B 00000000:0000 0A 00000000:00000000 00:00000000 00000000  0   0 37572  1 ffff8eb872c74000 100 0 0 10 0 */
        CHAR* Result = fgets (Buffer, sizeof (Buffer), PFile);
        if (Result == NULL)
        {
            continue;
        }

        //printf ("Buffer = %s \r\n", Buffer);
        char *Base = strstr (Buffer, ":");
        if (Base == NULL)
        {
            continue;
        }
        Base += 2;
   
        CHAR SrcIp[16]  = {0};
        CHAR SrcPort[8] = {0};
        CHAR DstIp[16]  = {0};
        CHAR DstPort[8] = {0};
        CHAR INode[16]  = {0};

        strncpy (SrcIp,   Base, 8); Base += 8 + 1;
        strncpy (SrcPort, Base, 4); Base += 4 + 1;
        strncpy (DstIp,   Base, 8); Base += 8 + 1;
        strncpy (DstPort, Base, 4); Base += 4 + 58;
        strncpy (INode,   Base, 8);

        DWORD SIp   = strtol(SrcIp, NULL, 16);
        DWORD SPort = strtol(SrcPort, NULL, 16);
        DWORD DIp   = strtol(DstIp, NULL, 16);
        DWORD DPort = strtol(DstPort, NULL, 16);
        DWORD iNodeId = atoi (INode);

        if (SIp == 0 || SPort == 0 || DIp == 0 || DPort == 0 || iNodeId == 0)
        {
            continue;
        }

        Flow *F = m_Flm.GetFlow (ntohl(SIp), ntohl(DIp), SPort, DPort, 6);
        assert (F != NULL);
 
        Inode2Flow[iNodeId] = F;
  
        //printf ("\t%s(%X):%s(%X), %s(%X):%s(%X) - %s (%u)\r\n", SrcIp, SIp, SrcPort, SPort, DstIp, DIp, DstPort, DPort, INode, iNodeId);
    }
}

DWORD FlowTable::UpdateFlowTable()
{
    GetProcessId ();

    GetProSockets();

    map<DWORD, Flow*> Inode2Flow;
    LoadTcpInfo(Inode2Flow);

    for (auto It = m_PID2Proc.begin (); It != m_PID2Proc.end (); It++)
    {
        ProcInfo *Proc = It->second;

        for (auto Sit = Proc->Socket.begin (); Sit != Proc->Socket.end (); Sit++)
        {
            DWORD Inode = *Sit;

            auto Fit = Inode2Flow.find (Inode);
            if (Fit == Inode2Flow.end ())
            {
                continue;
            }

            Flow *F = Fit->second;
            F->SetProName (Proc->m_Name);
            //printf ("=> %X:%X, %X:%X -> %s\r\n", F->m_SrcIp, F->m_SrcPort, F->m_DstIp, F->m_DstPort, Proc->m_Name.c_str());
        }
    }
}



string FlowTable::GetFlowProc (DWORD SrcIp, DWORD DstIp, WORD SrcPort, WORD DstPort, DWORD ProtoType)
{
    Flow *F = m_Flm.QueryFlow (SrcIp, DstIp, SrcPort, DstPort, ProtoType);
    if (F == NULL)
    {
        return "None";
    }

    //printf ("GetFlowProc success \r\n");
    return F->m_ProcName;
}


