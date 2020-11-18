/***********************************************************
 * Author: Wen Li
 * Date  : 4/02/2019
 * Describe: packet capture from interface
 * History:
   <1> 4/02/2019 , create
************************************************************/
#include "Capture.h"

extern FlowManage m_Flmange;
extern T_IPSet *IpSet;


#define FILENAME "PCAPs/capture.pcap"

BOOL Capture::IsDeviceOnline ()
{
    char EBuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *DeviceList;

    INT Ret = pcap_findalldevs(&DeviceList, EBuf);
    if (Ret != 0)
    {
        return M_FALSE;
    }

    while (DeviceList != NULL)
    {
        printf ("Get online device: %s\r\n", DeviceList->name);
        if (m_Device.compare (DeviceList->name) == 0)
        {
            return M_TRUE;
        }

        DeviceList = DeviceList->next;
    }
    
    return M_FALSE;
}

pcap_t* Capture::InitPcapHandle ()
{
    DWORD LocalNet = 0;
    DWORD NetMask  = 0;
    pcap_t *CapHandle;
    CHAR Error[PCAP_ERRBUF_SIZE];
    struct bpf_program FCode;
    
    CapHandle = pcap_open_live(m_Device.c_str(), m_CapLen, M_TRUE, 1000, Error);
    assert (CapHandle != NULL);

    m_CapLen = pcap_snapshot(CapHandle);
    if(m_CapLen != CAP_LEN) 
    {
        printf ("snaplen changes from %u to %u\r\n", CAP_LEN, m_CapLen);
    }
    
    if(pcap_lookupnet(m_Device.c_str(), &LocalNet, &NetMask, Error) < 0)
    {
        LocalNet = NetMask = 0;
        printf("%s\n", Error);
    }
    
    assert (pcap_compile(CapHandle,  &FCode, "", 1, NetMask) >= 0);    
    assert (pcap_setfilter(CapHandle, &FCode) >= 0);

    return CapHandle;
}

static void Analysis(BYTE *user, struct pcap_pkthdr *Hdr, BYTE *PktData)
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
    Flow* F = m_Flmange.GetFlow (Packet->m_SrcIp, Packet->m_DstIp, 
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
            
    //printf("[Flow]%X-%d-%X-%d-%d : %u\r\n", 
    //       F->m_SrcIp, F->m_SrcPort, F->m_DstIp, F->m_DstPort, F->m_ProtoType, F->m_SduNum);
    pcap_dump(user, Hdr, PktData);        
}


VOID Capture::CapturePacket()
{
    pcap_dumper_t* Dumper;
    
    if (!IsDeviceOnline ())
    {
        printf ("Device %s is not online.\r\n", m_Device.c_str());
        return;
    }
    
    m_CapHandle = InitPcapHandle ();  
    Dumper = pcap_dump_open(m_CapHandle, FILENAME);
    assert (Dumper != NULL);
    
    if(pcap_loop(m_CapHandle, -1, (pcap_handler)Analysis, (u_char*)Dumper) < 0)
    {
        printf("Error in pcap_loop\r\n");
        pcap_close(m_CapHandle);
    }
    
    return;
}


