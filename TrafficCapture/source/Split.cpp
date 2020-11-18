/***********************************************************
 * Author: Wen Li
 * Date  : 11/10/2020
 * Describe: traffic flow split
 * History:
   <1> 11/10/2020 , create
************************************************************/

#include "Split.h"
#include "FlowCsv.h"

FlowManage Split::m_Flm;
extern T_IPSet *IpSet;


static VOID ReadPacket(BYTE *user, const struct pcap_pkthdr *Hdr, BYTE *PktData)
{
    if (Hdr->caplen < ETH_HEADER_LEN + 28)
    {
        return;
    }

    BYTE* IpData = PktData+ETH_HEADER_LEN;
    if (*IpData != 0x45)
    {
        return;
    }
            
    IpPacket *Packet = new IpPacket(IpData, Hdr->caplen-ETH_HEADER_LEN, IpSet);
    assert (Packet != NULL);
    Flow* F = Split::m_Flm.GetFlow (Packet->m_SrcIp, Packet->m_DstIp, 
                                    Packet->m_SrcPort, Packet->m_DstPort, Packet->m_ProtoType);
    if (F->m_SduNum >= CAPTURE_NUM)
    {
        return;
    }
        
    F->m_PacketNum++;
    if (Packet->m_PayloadLen == 0)
    {
        delete Packet;
        return;
    }

    F->m_SduNum++;
    F->m_PakcetData.push_back (Packet);
    return;
}

VOID Split::LoadPcap (const char *PcapFile)
{
    pcap_t *pCapHandle;
    char errbuf[PCAP_ERRBUF_SIZE];
        
    if (PcapFile == NULL)
    {
        return;
    }
            
    pCapHandle = pcap_open_offline(PcapFile, errbuf);
    if (pCapHandle == NULL)
    {
        printf("pcap_open_offline fail, pbPcapFile=%s\r\n", PcapFile);
        return;
    }
            
    pcap_loop(pCapHandle, 0, (pcap_handler)ReadPacket, NULL);   
    pcap_close(pCapHandle);
}

static VOID DumpFlow(BYTE *user, const struct pcap_pkthdr *Hdr, BYTE *PktData)
{     
    pcap_dump(user, Hdr, PktData);	
    return;
}


VOID Split::Dump (Flow *F)
{       
    pcap_t *pCapHandle;
    char errbuf[PCAP_ERRBUF_SIZE];
    struct bpf_program Fp;
    char FilterExp[1024] = {0};
        
    char  FlowPcap [256];
    snprintf(FlowPcap, sizeof(FlowPcap), "SplitPCAPs/%.2X_%.2X_%.2X_%.2X_%.2X.pcap", 
             F->m_SrcIp, F->m_SrcPort, F->m_DstIp, F->m_DstPort, F->m_ProtoType);
            
    pCapHandle = pcap_open_offline(m_PcapFile.c_str (), errbuf);
    if (pCapHandle == NULL)
    {
        printf("pcap_open_offline fail, pbPcapFile=%s\r\n", m_PcapFile.c_str ());
        return;
    }

    string Protocol = "tcp";
    if (F->m_ProtoType != 6)
    {
        Protocol = "udp";
    }

    snprintf (FilterExp, sizeof (FilterExp), 
              "(src %u and src port %d and dst %u and dst port %d and %s) || (src %u and src port %d and dst %u and dst port %d and %s)",
              F->m_SrcIp, F->m_SrcPort, F->m_DstIp, F->m_DstPort, Protocol.c_str(),
              F->m_DstIp, F->m_DstPort, F->m_SrcIp, F->m_SrcPort, Protocol.c_str());

    //printf ("FilterExp = %s \r\n", FilterExp);
    if (pcap_compile(pCapHandle, &Fp, FilterExp, 0, 0xffffff) == -1) 
    {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", FilterExp, pcap_geterr(pCapHandle));
        return;
    }
            
    if (pcap_setfilter(pCapHandle, &Fp) == -1) 
    {
        fprintf(stderr, "Couldn't install filter %s: %s\n", FilterExp, pcap_geterr(pCapHandle));
        return;
    }

    pcap_dumper_t* Dumper = pcap_dump_open(pCapHandle, FlowPcap);
    assert (Dumper != NULL);;
            
    pcap_loop(pCapHandle, 0, (pcap_handler)DumpFlow, (u_char*)Dumper);   
    pcap_close(pCapHandle);
}


VOID Split::SplitFlows()
{
    DWORD Index = 0;
    DWORD FlowNum = m_Flm.GetFlowNum ();
    for (auto It = m_Flm.begin (); It != m_Flm.end (); It++)
    {
        Flow *F = *It;
        if (F->m_SduNum < CAPTURE_NUM)
        {
            continue;
        }

        printf ("dump [%-4d/%4d] flow\r", Index++, FlowNum);
        Dump (F);
    }
    printf ("dump [%-4d/%-4d] flow\r\n", Index, FlowNum);

    if (m_IsCsv)
    {
        FlowCsv FC (&m_Flm, NULL);
        printf ("dump CSVs of flows...\r\n");
        FC.Dump ();
    }
}


