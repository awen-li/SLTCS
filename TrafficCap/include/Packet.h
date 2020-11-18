/***********************************************************
 * Author: Wen Li
 * Date  : 3/28/2020
 * Describe: Packet parse
 * History:
   <1> 3/28/2020 , create
************************************************************/

#ifndef _PACKET_H_
#define _PACKET_H_ 
#include <BasicType.h>

using namespace std;

enum ETHTYPE
{
    ETH_IPV4  = 0x0800,
    ETH_IPV6  = 0x86DD,
    ETH_ARP   = 0x0806,
};

/*******************************************************************************************
 ethnet protocol
********************************************************************************************/
enum
{
    ETH_HEADER_LEN = 14,
    IPV4_VERSION   = 4,
    IPV6_VERSION   = 6,
    IPHDR_LEN      = 20,
    TCPHDR_LEN     = 20,
    UDPHDR_LEN     = 8
};

enum IP_LV4_PROTO
{
	LV4_TCP    = 6,  
	LV4_UDP    = 17,
}; 


struct IpEthHder
{
    BYTE DestMac[6];
    BYTE SrcMac[6];
    WORD EthType;
};


struct Ipv4Hdr  
{ 
	BYTE h_verlen;
	BYTE tos;            
	WORD total_len;      
	WORD ident;           
	WORD frag_and_flags;  
	BYTE ttl;            
	BYTE proto;           
	WORD checksum;        
	DWORD sourceIP;       
	DWORD destIP;       
};

struct TcpHdr 
{
	WORD  wSrcPort;
	WORD  wDstPort;
	DWORD dwSeqNum;
	DWORD dwAckNum; 
	BYTE  bHdrLen;
	BYTE  bTclFlg; 
	WORD  wWindowSize;
	WORD  wCheckSum; 
	WORD  wrgentPointer; 
};


struct UdpHdr 
{
	WORD wSrcPort;       
	WORD wDstPort;     
	WORD wLength ;      
	WORD wCheckSum;    
};

typedef set<DWORD> T_IPSet;

class IpPacket
{
public:
    DWORD m_SrcIp;
    DWORD m_DstIp;
    WORD  m_SrcPort;
    WORD  m_DstPort;
    DWORD m_ProtoType;

    DWORD m_PktLen;
    DWORD m_PayloadLen;
    BYTE* m_Payload;

private:  
    BYTE  m_PktData[PACKET_SIZE+4];
    
    DWORD Ipv4Parse (T_IPSet *IpSet);
    
public:
    IpPacket (BYTE* PktData, DWORD PktLen, T_IPSet *IpSet)
    {
        m_SrcIp = 0;
        m_DstIp = 0;

        if (PktLen >= PACKET_SIZE)
        {
            PktLen = PACKET_SIZE;
        }
        
        memcpy (m_PktData, PktData, PktLen);
        m_PktLen  = PktLen;

        Ipv4Parse(IpSet);
    }

    ~IpPacket ()
    {
    }
};


#endif


