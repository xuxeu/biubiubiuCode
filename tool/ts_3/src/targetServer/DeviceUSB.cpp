/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  DeviceUSB.cpp
 * @Version        :  1.0.0
 * @Brief           :  USB设备层的接口抽象实现
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007年3月21日 11:34:32
 * @History        : 
 *   
 */



/************************引用部分*****************************/
#include "DeviceUSB.h"

/************************前向声明部分***********************/

/************************定义部分*****************************/

/************************实现部分****************************/
//constructor of class
DeviceUSB::DeviceUSB(GUID guid)
{
    m_isExit = false;
    m_Guid = guid;
    m_hFileIn = INVALID_HANDLE_VALUE;
    m_hFileOut= INVALID_HANDLE_VALUE;
    m_TxOverLap.hEvent = INVALID_HANDLE_VALUE;
    m_RxOverLap.hEvent = INVALID_HANDLE_VALUE;
    memset(completeDeviceName, 0, sizeof(completeDeviceName));
    inPipe = QString::fromAscii("PIPE00", strlen("PIPE00"));
    outPipe = QString::fromAscii("PIPE01", strlen("PIPE01"));
    m_ver = getWindowsVersion();
}

//destructor of class
DeviceUSB::~DeviceUSB()
{
    close();
}

bool DeviceUSB::open(ServerConfig* config)
{
    if(NULL == config)
    {
        sysLoger(LWARNING, "DeviceUSB::open: Parameter config is bad pointer!");
        return false;
    }
    USBConfig *pusbCfg = dynamic_cast<USBConfig*>(config->getDeviceConfig());
    if(NULL == pusbCfg)
    {
        sysLoger(LWARNING, "DeviceUSB::open:There is no deviceconfig for USB device!");
        return false;
    }
    m_Guid = pusbCfg->getGUID();
    inPipe = pusbCfg->getPipeInname().toAscii().data();
    outPipe = pusbCfg->getPipeOutName().toAscii().data();
    char strGUID[GUID_LEN];
    guidToString(m_Guid, true, strGUID, sizeof(strGUID));
    sysLoger(LDEBUG, "DeviceUSB::open:GUID:%s, inPipe:%s, outPipe:%s", strGUID,
                                    inPipe.toAscii().data(), outPipe.toAscii().data());
    if(inPipe.length() > 0)
    {
        m_hFileIn = open_file(inPipe.toAscii().data());        
    }
    if(outPipe.length() > 0)
    {
        m_hFileOut= open_file(outPipe.toAscii().data());        
    }
    if((m_hFileIn == INVALID_HANDLE_VALUE) || (m_hFileOut == INVALID_HANDLE_VALUE))
    {
        sysLoger(LINFO, "DeviceUSB::open: open the device for write or read failure!");
        return false;
    }
    //创建异步事件
    /*初始化异步结构*/
    ZeroMemory(&m_RxOverLap, sizeof(OVERLAPPED));
    ZeroMemory(&m_TxOverLap, sizeof(OVERLAPPED));
    /*为接收和发送的异步结构分别创建一个事件*/
    m_RxOverLap.hEvent = CreateEvent(NULL,true,false,NULL);    //创建一个手工设置的信号量,初始状态是没有信号
    m_TxOverLap.hEvent = CreateEvent(NULL,true,false,NULL);    //创建一个手工设置的信号量,初始状态是没有信号
    ResetEvent(m_RxOverLap.hEvent);
    ResetEvent(m_TxOverLap.hEvent);
    if(m_TxOverLap.hEvent == INVALID_HANDLE_VALUE || m_TxOverLap.hEvent == INVALID_HANDLE_VALUE ) 
    {
        sysLoger(LWARNING, "DeviceUart:: create overlap event failure!");
        return false;
    }
    return true;
}

bool DeviceUSB::close()
{
    m_isExit = true;
    
    if(m_TxOverLap.hEvent != INVALID_HANDLE_VALUE)
    {
        SetEvent(m_TxOverLap.hEvent);
        CloseHandle(m_TxOverLap.hEvent);
        m_TxOverLap.hEvent = INVALID_HANDLE_VALUE;
    }
    if(m_RxOverLap.hEvent != INVALID_HANDLE_VALUE)
    {
        SetEvent(m_RxOverLap.hEvent);
        CloseHandle(m_RxOverLap.hEvent);
        m_RxOverLap.hEvent = INVALID_HANDLE_VALUE;
    }
    if(m_hFileIn != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hFileIn);
        m_hFileIn = INVALID_HANDLE_VALUE;
    }
    if(m_hFileOut != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hFileOut);
        m_hFileOut = INVALID_HANDLE_VALUE;
    }
    inPipe.clear();
    outPipe.clear();
    sysLoger(LDEBUG, "DeviceUSB::close: Success to close the USB device!");
    return true;
}

void DeviceUSB::write(char *pData, int size)
{
    DWORD bytesWriten = 0;
    int ret = 0;
    if((pData != NULL) && (m_hFileOut != INVALID_HANDLE_VALUE))
    {
        ret = WriteFile(m_hFileOut, pData, size, &bytesWriten, &m_TxOverLap);
        /*等待写操作的完成*/
        ret = GetOverlappedResult(m_hFileOut, &m_TxOverLap, &bytesWriten, true);
        ResetEvent(m_TxOverLap.hEvent);
        /*重新设定时间句柄*/
        if (!ret || m_isExit) 
        {
            return;   
        }
        sysLoger(LDEBUG, "DeviceUSB::write:Required writing size:%d, writed size:%d, return:%d.", size, bytesWriten, ret);
        return;
    }
    return;
}

int DeviceUSB::read(char *pBuf, int size)
{
    DWORD bytesRead = 0;
    int ret = 0;
    if((NULL == pBuf) || (m_hFileIn == INVALID_HANDLE_VALUE))
    {
        return 0;
    }
    ret = ReadFile(m_hFileIn, pBuf, size, &bytesRead, &m_RxOverLap);
    /*等待读操作的完成*/
    ret = GetOverlappedResult(m_hFileIn,&m_RxOverLap,&bytesRead,true);
    if (!ret || m_isExit)
        return -1;
    /*重新设定时间句柄*/
    ResetEvent(m_RxOverLap.hEvent);
    sysLoger(LDEBUG, "DeviceUSB::read:Required reading size:%d, readed size:%d, return:%d.", size, bytesRead, ret);
    return bytesRead;
}


 HANDLE DeviceUSB::OpenOneDeviceForWin2k(
                                                                      HDEVINFO                    HardwareDeviceInfo,
                                                                      PSP_INTERFACE_DEVICE_DATA   DeviceInfoData,
                                                                      char *devName)
/*++
Routine Description:

    Given the HardwareDeviceInfo, representing a handle to the plug and
    play information, and deviceInfoData, representing a specific usb device,
    open that device and fill in all the relevant information in the given
    USB_DEVICE_DESCRIPTOR structure.

Arguments:

    HardwareDeviceInfo:  handle to info obtained from Pnp mgr via SetupDiGetClassDevs()
    DeviceInfoData:      ptr to info obtained via SetupDiEnumDeviceInterfaces()

Return Value:

    return HANDLE if the open and initialization was successfull,
        else INVLAID_HANDLE_VALUE.

--*/
{
    PSP_INTERFACE_DEVICE_DETAIL_DATA     functionClassDeviceData = NULL;
    ULONG                                predictedLength = 0;
    ULONG                                requiredLength = 0;
    HANDLE                               hOut = INVALID_HANDLE_VALUE;

    //
    // allocate a function class device data structure to receive the
    // goods about this particular device.
    //
    SetupDiGetInterfaceDeviceDetail (
                                                         HardwareDeviceInfo,
                                                         DeviceInfoData,
                                                         NULL, // probing so no output buffer yet
                                                         0, // probing so output buffer length of zero
                                                         &requiredLength,
                                                         NULL); // not interested in the specific dev-node


    predictedLength = requiredLength;
    // sizeof (SP_FNCLASS_DEVICE_DATA) + 512;

    functionClassDeviceData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc (predictedLength);
    if(NULL == functionClassDeviceData) 
    {
        sysLoger(LDEBUG, "DeviceUSB::OpenOneDevice: alloc buffer failed , size:%d", predictedLength);
        return INVALID_HANDLE_VALUE;
    }
    functionClassDeviceData->cbSize = sizeof (SP_INTERFACE_DEVICE_DETAIL_DATA);

    sysLoger(LDEBUG, "start to get deviceinterface info %s\n", devName);
    // Retrieve the information from Plug and Play.
    //
    if (! SetupDiGetInterfaceDeviceDetail (
               HardwareDeviceInfo,
               DeviceInfoData,
               functionClassDeviceData,
               predictedLength,
               &requiredLength,
               NULL)) 
    {
        free( functionClassDeviceData );
        sysLoger(LDEBUG, "DeviceUSB::OpenOneDevice: SetupDiGetInterfaceDeviceDetail failed ,");
        return INVALID_HANDLE_VALUE;
    }

    strcpy( devName,functionClassDeviceData->DevicePath) ;
    sysLoger(LDEBUG, "Attempting to open %s\n", devName);

    hOut = CreateFile (
                  functionClassDeviceData->DevicePath,
                  GENERIC_READ | GENERIC_WRITE,
                  FILE_SHARE_READ | FILE_SHARE_WRITE,
                  NULL, // no SECURITY_ATTRIBUTES structure
                  OPEN_EXISTING, // No special create flags
                  0, // No special attributes
                  NULL); // No template file

    if (INVALID_HANDLE_VALUE == hOut) 
    {
        sysLoger(LINFO, "FAILED to open %s\n", devName);
    }
    free( functionClassDeviceData );
    return hOut;
}

HANDLE DeviceUSB::OpenOneDeviceForWinXP (
                                                                      HDEVINFO                    HardwareDeviceInfo,
                                                                      PSP_DEVICE_INTERFACE_DATA   DeviceInfoData,
                                                                      char *devName)
/*++
Routine Description:

    Given the HardwareDeviceInfo, representing a handle to the plug and
    play information, and deviceInfoData, representing a specific usb device,
    open that device and fill in all the relevant information in the given
    USB_DEVICE_DESCRIPTOR structure.

Arguments:

    HardwareDeviceInfo:  handle to info obtained from Pnp mgr via SetupDiGetClassDevs()
    DeviceInfoData:      ptr to info obtained via SetupDiEnumDeviceInterfaces()

Return Value:

    return HANDLE if the open and initialization was successfull,
        else INVLAID_HANDLE_VALUE.

--*/
{
    PSP_DEVICE_INTERFACE_DETAIL_DATA     functionClassDeviceData = NULL;
    ULONG                                predictedLength = 0;
    ULONG                                requiredLength = 0;
    HANDLE                               hOut = INVALID_HANDLE_VALUE;

    //
    // allocate a function class device data structure to receive the
    // goods about this particular device.
    //
    SetupDiGetDeviceInterfaceDetail (
                                                         HardwareDeviceInfo,
                                                         DeviceInfoData,
                                                         NULL, // probing so no output buffer yet
                                                         0, // probing so output buffer length of zero
                                                         &requiredLength,
                                                         NULL); // not interested in the specific dev-node


    predictedLength = requiredLength;
    // sizeof (SP_FNCLASS_DEVICE_DATA) + 512;

    functionClassDeviceData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc (predictedLength);
    if(NULL == functionClassDeviceData) 
    {
        return INVALID_HANDLE_VALUE;
    }
    functionClassDeviceData->cbSize = sizeof (SP_DEVICE_INTERFACE_DETAIL_DATA);

    //
    // Retrieve the information from Plug and Play.
    //
    if (! SetupDiGetDeviceInterfaceDetail (
               HardwareDeviceInfo,
               DeviceInfoData,
               functionClassDeviceData,
               predictedLength,
               &requiredLength,
               NULL)) 
    {
        free( functionClassDeviceData );
        return INVALID_HANDLE_VALUE;
    }

    strcpy( devName,functionClassDeviceData->DevicePath) ;
    //sysLoger(LINFO, "Attempting to open %s\n", devName);

    hOut = CreateFile (
                  functionClassDeviceData->DevicePath,
                  GENERIC_READ | GENERIC_WRITE,
                  FILE_SHARE_READ | FILE_SHARE_WRITE,
                  NULL, // no SECURITY_ATTRIBUTES structure
                  OPEN_EXISTING, // No special create flags
                  0, // No special attributes
                  NULL); // No template file

    if (INVALID_HANDLE_VALUE == hOut) 
    {
        //sysLoger(LINFO, "FAILED to open %s\n", devName);
    }
    free( functionClassDeviceData );
    return hOut;
}

HANDLE DeviceUSB::OpenUsbDevice( LPGUID  pGuid, char *outNameBuf)
/*++
Routine Description:

   Do the required PnP things in order to find
   the next available proper device in the system at this time.

Arguments:

    pGuid:      ptr to GUID registered by the driver itself
    outNameBuf: the generated name for this device

Return Value:

    return HANDLE if the open and initialization was successful,
        else INVLAID_HANDLE_VALUE.
--*/
{
    ULONG NumberDevices;
    HANDLE hOut = INVALID_HANDLE_VALUE;
    HDEVINFO                 hardwareDeviceInfo;
    SP_DEVICE_INTERFACE_DATA deviceInfoData;
    ULONG                    i;
    BOOLEAN                  done;
    
    NumberDevices = 0;

    //
    // Open a handle to the plug and play dev node.
    // SetupDiGetClassDevs() returns a device information set that contains info on all
    // installed devices of a specified class.
    //
    hardwareDeviceInfo = SetupDiGetClassDevs (
                           pGuid,
                           NULL, // Define no enumerator (global)
                           NULL, // Define no
                           (DIGCF_PRESENT | // Only Devices present
                            DIGCF_DEVICEINTERFACE)); // Function class devices.

    //
    // Take a wild guess at the number of devices we have;
    // Be prepared to realloc and retry if there are more than we guessed
    //
    NumberDevices = 4;
    done = FALSE;
    deviceInfoData.cbSize = sizeof (SP_DEVICE_INTERFACE_DATA);

    i=0;
    while (!done) 
    {
        NumberDevices *= 2;
        
        for (; i < NumberDevices; i++) 
        {

            // SetupDiEnumDeviceInterfaces() returns information about device interfaces
            // exposed by one or more devices. Each call returns information about one interface;
            // the routine can be called repeatedly to get information about several interfaces
            // exposed by one or more devices.

            if (SetupDiEnumDeviceInterfaces (hardwareDeviceInfo,
                                                                 0, // We don't care about specific PDOs
                                                                 pGuid,
                                                                 i,
                                                                 &deviceInfoData)) 
            {
                if(m_ver < WINXP)
                {
                    hOut = OpenOneDeviceForWin2k(hardwareDeviceInfo, &deviceInfoData, outNameBuf);
                }
                else
                {
                    hOut = OpenOneDeviceForWinXP (hardwareDeviceInfo, &deviceInfoData, outNameBuf);
                }
                if ( hOut != INVALID_HANDLE_VALUE ) 
                {
                    done = TRUE;
                    break;
                }
            } 
            else 
            {
                if (ERROR_NO_MORE_ITEMS == GetLastError())
                {
                    done = TRUE;
                    break;
                }
            }
        }
    }

   NumberDevices = i;

   // SetupDiDestroyDeviceInfoList() destroys a device information set
   // and frees all associated memory.

   SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
   //free ( *UsbDevices );
   return hOut;
}

BOOL DeviceUSB::GetUsbDeviceFileName( LPGUID  pGuid, char *outNameBuf)
/*++
Routine Description:

    Given a ptr to a driver-registered GUID, give us a string with the device name
    that can be used in a CreateFile() call.
    Actually briefly opens and closes the device and sets outBuf if successfull;
    returns FALSE if not

Arguments:

    pGuid:      ptr to GUID registered by the driver itself
    outNameBuf: the generated zero-terminated name for this device

Return Value:

    TRUE on success else FALSE

--*/
{
        HANDLE hDev = OpenUsbDevice( pGuid, outNameBuf );
        if ( hDev != INVALID_HANDLE_VALUE )
        {
                CloseHandle( hDev );
                return TRUE;
        }
        return FALSE;

}

HANDLE DeviceUSB::open_file( char *filename)
/*++
Routine Description:

    Called by main() to open an instance of our device after obtaining its name

Arguments:

    None

Return Value:

    Device handle on success else NULL

--*/
{

    int success = 1;
    HANDLE h;

    if( !GetUsbDeviceFileName((LPGUID) &m_Guid, completeDeviceName))
    {
        sysLoger(LINFO, "DeviceUSB::open_file:Failed to GetUsbDeviceFileName err - %d", GetLastError());
        return  INVALID_HANDLE_VALUE;
    }

    strcat (completeDeviceName, "\\");                      

    if((strlen(completeDeviceName) + strlen(filename)) > 255) 
    {
        sysLoger(LINFO, "DeviceUSB::open_file:Failed to open handle - possibly long filename");
        return INVALID_HANDLE_VALUE;
    }

    strcat (completeDeviceName, filename);

    sysLoger(LINFO, "DeviceUSB::open_file:completeDeviceName = (%s)", completeDeviceName);

    h = CreateFile(completeDeviceName,
            GENERIC_WRITE | GENERIC_READ,
            FILE_SHARE_WRITE | FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
            NULL);

    if (h == INVALID_HANDLE_VALUE) 
    {
        sysLoger(LINFO, "DeviceUSB::open_file:Failed to open (%s) = %d", completeDeviceName, GetLastError());
        success = 0;
    } 
    else 
    {
        sysLoger(LINFO, "DeviceUSB::open_file:Opened successfully.");
    }           

    return h;
}

