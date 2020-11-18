/***********************************************************
 * Author: Wen Li
 * Date  : 3/28/2020
 * Describe: Context for flow analysis
 * History:
   <1> 3/28/2020 , create
************************************************************/

#ifndef _CONTEXT_H_
#define _CONTEXT_H_
#include <BasicType.h>
#include <Log.h>
#include <Packet.h>

using namespace std;

class Flow
{
public:
    DWORD m_SrcIp;
    DWORD m_DstIp;
    WORD  m_SrcPort;
    WORD  m_DstPort;
    DWORD m_ProtoType;
    DWORD m_TimeStamp;

    DWORD m_PacketNum;
    DWORD m_SduNum;
    DWORD m_Fin;

    string m_ProcName;

    vector <IpPacket *> m_PakcetData;

public:
    Flow (DWORD SrcIp, DWORD DstIp, WORD SrcPort, WORD DstPort, DWORD ProtoType)
    {
        m_SrcIp = SrcIp;
        m_DstIp = DstIp;
        m_SrcPort = SrcPort;
        m_DstPort = DstPort;
        m_ProtoType = ProtoType;

        m_TimeStamp = time (NULL);

        m_PacketNum = 0;
        m_SduNum = 0;
        m_Fin = 0;

        m_ProcName = "";
    }

    ~Flow ()
    {
        DelPackets ();
    }

    inline VOID SetProName (string ProcName)
    {
        m_ProcName = ProcName;
        return;
    }

    VOID DelPackets ()
    {
        for (auto pIt = m_PakcetData.begin(); pIt != m_PakcetData.end (); pIt++)
        {
            IpPacket *Packet = *pIt;
            delete Packet;
        }

        m_PakcetData.clear ();
    }


    typedef struct 
    {
        bool operator()(Flow *L, Flow *R) 
        {
            if (L->m_SrcIp != R->m_SrcIp)
            {
                return (L->m_SrcIp < R->m_SrcIp);
            }
            else if (L->m_DstIp != R->m_DstIp)
            {
                return (L->m_DstIp < R->m_DstIp);
            }
            else if (L->m_SrcPort != R->m_SrcPort)
            {
                return (L->m_SrcPort < R->m_SrcPort);
            }
            else if (L->m_DstPort != R->m_DstPort)
            {
                return (L->m_DstPort < R->m_DstPort);
            }
            else
            {
                return (L->m_ProtoType < R->m_ProtoType);
            }
        }
    } EqualFlow; 
};

typedef set<Flow*, typename Flow::EqualFlow> T_FlowSet;

class FlowManage
{
private:
    Flow *m_Fkey;
    T_FlowSet m_FlowSet;
    

public:
    FlowManage ()
    {
        m_Fkey = new Flow (0, 0, 0, 0, 0);
    }

    ~FlowManage ()
    {
        if (m_Fkey)
        {
            delete m_Fkey;
        }
    }

    inline Flow* AddFlow (Flow *Fkey)
    {
        Flow *NewF = new Flow (Fkey->m_SrcIp, Fkey->m_DstIp,
                               Fkey->m_SrcPort, Fkey->m_DstPort, Fkey->m_ProtoType);
        assert (NewF != NULL);
        
        auto It = m_FlowSet.insert (NewF);
        if (It.second == false)
        {
            return NULL;
        }

        //DebugLog ("Add flow-Pro: %d Src: %u-%u, Dst: %u-%u\r\n", F.m_ProtoType, F.m_SrcIp, F.m_SrcPort, F.m_DstIp, F.m_DstPort);
        return NewF;        
    }

    inline Flow* QueryFlow (DWORD SrcIp, DWORD DstIp, WORD SrcPort, WORD DstPort, DWORD ProtoType)
    {
        Flow* Fctx = NULL;

        m_Fkey->m_SrcIp = SrcIp;
        m_Fkey->m_DstIp = DstIp;
        m_Fkey->m_SrcPort = SrcPort;
        m_Fkey->m_DstPort = DstPort;
        m_Fkey->m_ProtoType = ProtoType;
        
        auto It = m_FlowSet.find (m_Fkey);
        if (It != m_FlowSet.end())
        {
            Fctx = (Flow*)(*It);
            Fctx->m_TimeStamp = time (NULL);
        }

        return Fctx;
    }

    inline Flow* GetFlow (DWORD SrcIp, DWORD DstIp, WORD SrcPort, WORD DstPort, DWORD ProtoType)
    {
        Flow* Fctx;

        m_Fkey->m_SrcIp = SrcIp;
        m_Fkey->m_DstIp = DstIp;
        m_Fkey->m_SrcPort = SrcPort;
        m_Fkey->m_DstPort = DstPort;
        m_Fkey->m_ProtoType = ProtoType;
        
        auto It = m_FlowSet.find (m_Fkey);
        if (It != m_FlowSet.end())
        {
            Fctx = (Flow*)(*It);
            Fctx->m_TimeStamp = time (NULL);
        }
        else
        {
            DebugLog ("Add a new flow:%u %u-%u %u-%u\r\n", \
                      m_Fkey->m_ProtoType, m_Fkey->m_SrcIp, m_Fkey->m_SrcPort, m_Fkey->m_DstIp, m_Fkey->m_DstPort);
            Fctx = AddFlow (m_Fkey);
        }

        return Fctx;
    }

    inline T_FlowSet::iterator begin ()
    {
        return m_FlowSet.begin();
    }

    inline T_FlowSet::iterator end ()
    {
        return m_FlowSet.end();
    }

    inline DWORD GetFlowNum ()
    {
        return m_FlowSet.size ();
    }
    
};


#endif 
