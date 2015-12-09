/*
Hi-Link M35-MT7681 library

2015/09/14 1.0 Created by Keynes Lu (KeynesProject@gmail.com)
                       
Modified version
V1.0  released the first version of Hi-Link M35 library
*/
#ifndef __WIFI_MT7681_H__
#define __WIFI_MT7681_H__

#include <Arduino.h>
#include <SoftwareSerial.h>

#define Uart Serial

//#define DEBUG
#ifdef  DEBUG
#define DebugSerial  g_SoftSerial
#endif    
         
#define UART_BAUDRATE   115200         
#define SOFT_UART_RXPIN 5
#define SOFT_UART_TXPIN 6
extern SoftwareSerial g_SoftSerial;

class WifiMt7681
{    
public:
    WifiMt7681();
    ~WifiMt7681();
    
    bool Initial( byte PinES, byte PinReset );
    
    void HardwareReset();
    
    //AP模式設定
    // Ssid : AP名稱
    // Pwd  : 連線密碼
    // ApIP : AP IP位址,用逗號區隔數字,EX: 192,168,0,1
    // ApPort : 連線埠
    // Ecn  : 加密方式, 4:Wpa_aes/wpa_tkip  7:Wpa2_aes/wpa2_tkip  9:Wpa/wpa2_aes
    bool SetupAp( String Ssid, String Pwd, String ApIP, int ApPort, byte Ecn = 9 );
        
    //Sta模式設定
    // Ssid : 連線目標AP名稱
    // Pwd  : 連線密碼
    // RemoteIP : 通訊目標IP位址,用逗號區隔數字,EX: 192,168,0,1或直接打入網域名稱,EX:www.google.com
    // RemotePort : 通訊目標連線埠
    // Ecn  : 加密方式, 4:Wpa_aes/wpa_tkip  7:Wpa2_aes/wpa2_tkip  9:Wpa/wpa2_aes
    bool SetupSta( String Ssid, String Pwd, String RemoteIP, int RemotePort = 80, byte Ecn = 9 );
    
    //啟動網路傳輸;//
    void StartTransport( bool Start );
    
    //透過Wifi傳送資料;//
    bool Send( String Data ); 
    bool Send( char Character ); 
    bool SendL( String Data ); 
    
    //從Wifi接收資料,回傳:收到資料長度;//
    int Receive( char *Buffer );
        
    //取得IP,Mode:0:AP, 1:STA, 2:透傳目標IP;//
    String GetIP( byte Mode );
    
    //取的連線埠, Mode:0:本地PORT, 1:目標Port;//    
    int GetPort( byte Mode );
public:
    
    //檢查模組是否啟動;//
    bool CheckModular();
    
    //設定Uart串口數值;//
    bool SetupUart();
    
    //設定為透傳或AT模式, Open:true開啟透傳模式;//
    void SetTransportMode( bool Open );
    
    bool SetSerialParameter();
    
    enum ReplyMode
    {
       eREPLY_OK = 0,
       eREPLY_DATA,
       eREPLY_NONE
    }; 
    //回傳 MODE
    //eREPLY_OK:回傳字串長度為0表示成功，其餘為失敗
    //eREPLY_DATA:回傳字串長度為0表示失敗,其餘為模組回傳的資料
    //eREPLY_NONE:一律回傳0
    String SendAtCmd( String AtCmd, ReplyMode Mode = eREPLY_OK, unsigned long WaitTime = 7000 );
    
private:
    byte m_PinEs;
    byte m_PinReset;    
};

#endif
