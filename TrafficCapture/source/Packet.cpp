/***********************************************************
 * Author: Wen Li
 * Date  : 3/28/2020
 * Describe: Packet parse
 * History:
   <1> 3/28/2020 , create
************************************************************/
#include <Packet.h>
#include <Log.h>


DWORD IpPacket::Ipv4Parse (T_IPSet *IpSet)
{
    BYTE *IpData;

    if (m_PktLen < IPHDR_LEN)
    {
        DebugLog ("Ipv4Parse, m_PktLen = %u\r\n", m_PktLen);
        return M_FAIL;
    }
    
    Ipv4Hdr* Ipv4Header = (Ipv4Hdr*)m_PktData;
    
    IpData = (BYTE *)Ipv4Header;
    if(IpData[0]&0x40)
	{
		DWORD SrcIp;
		DWORD DstIp;

		DWORD IpHdrLen = (Ipv4Header->h_verlen&0x0f)<<2;
		if(IpHdrLen < IPHDR_LEN)
		{
			return M_FAIL;
		}

		Ipv4Header->total_len = ntohs(Ipv4Header->total_len);
        if (Ipv4Header->total_len > m_PktLen)
        {
            Ipv4Header->total_len = m_PktLen;
        }

        Ipv4Header->destIP   = ntohl(Ipv4Header->destIP);
        Ipv4Header->sourceIP = ntohl(Ipv4Header->sourceIP);

        DWORD UserIp = Ipv4Header->sourceIP;
        if (IpSet->find (Ipv4Header->sourceIP) == IpSet->end ())
        {
            UserIp = Ipv4Header->destIP;
        }
    
		DWORD Proto = (DWORD)Ipv4Header->proto;
        switch(Proto)
    	{
    	case LV4_TCP:
    		{
                if (m_PktLen < IPHDR_LEN+TCPHDR_LEN)
                {
                    DebugLog ("Ipv4Parse, LV4_TCP, m_PktLen = %u\r\n", m_PktLen);
                    return M_FAIL;
                }
                
    			TcpHdr* THdr = (TcpHdr*)((BYTE*)Ipv4Header + IpHdrLen);
                
                BYTE TcpHdrLen = (THdr->bHdrLen&0xf0)>>2;
    			if(TcpHdrLen < TCPHDR_LEN)
    			{
    				return M_FAIL;
    			}

                THdr->wSrcPort = ntohs(THdr->wSrcPort);
    			THdr->wDstPort = ntohs(THdr->wDstPort);
    			THdr->dwSeqNum = ntohl(THdr->dwSeqNum);

                m_ProtoType = LV4_TCP;
                m_PayloadLen = m_PktLen-(IPHDR_LEN+TcpHdrLen);
                m_Payload    = (BYTE*)(THdr+1);
                
                if (UserIp == Ipv4Header->sourceIP)
                {
                    m_SrcIp = Ipv4Header->sourceIP;
                    m_DstIp = Ipv4Header->destIP;

                    m_SrcPort = THdr->wSrcPort;
                    m_DstPort = THdr->wDstPort;
                }
                else if (UserIp == Ipv4Header->destIP)
                {
                    m_SrcIp = Ipv4Header->destIP;
                    m_DstIp = Ipv4Header->sourceIP;

                    m_SrcPort = THdr->wDstPort;
                    m_DstPort = THdr->wSrcPort;
                }
                else
                {
                    DebugLog ("User ip not configured yet...\r\n");
                }
    						
    			break;
    		}
    	case LV4_UDP:
    		{
                if (m_PktLen < IPHDR_LEN+UDPHDR_LEN)
                {
                    DebugLog ("Ipv4Parse, LV4_UDP, m_PktLen = %u\r\n", m_PktLen);
                    return M_FAIL;
                }
                
    			UdpHdr *UHdr = (UdpHdr*)((BYTE*)Ipv4Header + IpHdrLen);

    			UHdr->wSrcPort = ntohs(UHdr->wSrcPort);
    			UHdr->wDstPort = ntohs(UHdr->wDstPort);

                m_ProtoType = LV4_UDP;
                m_PayloadLen = m_PktLen-(IPHDR_LEN+UDPHDR_LEN);
                m_Payload    = (BYTE*)(UHdr+1);
                
                if (UserIp == Ipv4Header->sourceIP)
                {
                    m_SrcIp = Ipv4Header->sourceIP;
                    m_DstIp = Ipv4Header->destIP;

                    m_SrcPort = UHdr->wSrcPort;
                    m_DstPort = UHdr->wDstPort;
                }
                else if (UserIp = Ipv4Header->destIP)
                {
                    m_SrcIp = Ipv4Header->destIP;
                    m_DstIp = Ipv4Header->sourceIP;

                    m_SrcPort = UHdr->wDstPort;
                    m_DstPort = UHdr->wSrcPort;
                }
                else
                {
                    DebugLog ("User ip not configured yet...\r\n");
                }

    			break;
    		}
    	default:
    		{
    			break;
    		}
    	}

        DebugLog ("Pro: %d Src: %u-%u, Dst: %u-%u, Pakcet-Len: %u, Payload-Len: %u\r\n",\
                  m_ProtoType, m_SrcIp, m_SrcPort, m_DstIp, m_DstPort, m_PktLen, m_PayloadLen);
		return M_SUCCESS;

	}
	else if(IpData[0]&0x60)
	{
		/* ipv6 */
        DebugLog ("Ipv6 packets, not support..\r\n");
		return M_FAIL;
	}
	else
	{
        DebugLog ("Unsupport protocol: %#x\r\n", (DWORD)IpData[0]);
		return M_FAIL;
	}
     
}
    

