/***********************************************************
 * Author: Wen Li
 * Date  : 4/02/2019
 * Describe: packet capture from interface
 * History:
   <1> 4/02/2019 , create
************************************************************/

#ifndef _CAPTURE_H_
#define _CAPTURE_H_
#include <BasicType.h>
#include <pcap.h>
#include <Packet.h>
#include <Context.h>


using namespace std;

#define CAP_LEN     (2048)

class Capture
{
protected:
    string m_Device;
    DWORD  m_CapLen;
    pcap_t* m_CapHandle;

    BOOL IsDeviceOnline ();
    pcap_t* InitPcapHandle ();

public:

    Capture (string Device)
    {
        m_Device = Device;
        assert (m_Device != "");

        m_CapLen = CAP_LEN;
        m_CapHandle = NULL;
    }
    
    VOID CapturePacket();

    inline VOID CloseCapture ()
    {
        pcap_close(m_CapHandle);
        m_CapHandle = NULL;
    }
    
};


#endif 
