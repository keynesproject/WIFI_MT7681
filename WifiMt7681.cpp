#include "WifiMt7681.h"

SoftwareSerial g_SoftSerial( SOFT_UART_RXPIN, SOFT_UART_TXPIN );

#ifdef DEBUG
#define DBG(message)    DebugSerial.print(message)
#define DBGL(message)   DebugSerial.println(message)
#define DBGW(message)   DebugSerial.write(message)
#else
#define DBG(message)
#define DBGW(message)
#define DBGL(message)   
#endif // DEBUG

WifiMt7681::WifiMt7681()
{
}

WifiMt7681::~WifiMt7681()
{
    
}

bool WifiMt7681::Initial( byte PinES, byte PinReset )
{
    Uart.begin( UART_BAUDRATE );
#ifdef DEBUG 
    //The default baud rate for debugging is 9600
    DebugSerial.begin( UART_BAUDRATE );   
#endif    
    Uart.flush();
    Uart.setTimeout(3000);
       
    m_PinEs = PinES;
    m_PinReset = PinReset;
    pinMode(m_PinEs, OUTPUT);
    pinMode(m_PinReset, OUTPUT);
    digitalWrite(m_PinEs,HIGH);
    digitalWrite(m_PinReset,HIGH);
    
    //重新啟動模組;//
    HardwareReset();
    
    //檢查模組是否啟動;//
    if( !CheckModular() )
    {
        DBGL( "WIFI Check Modular Error!" );
        return false;
    }
    
    //預設模組傳輸模式為AT模式;//
    SetTransportMode( false );    
        
    return true;
}

void WifiMt7681::HardwareReset()
{
    digitalWrite( m_PinReset, LOW );
    delay( 500 );
    digitalWrite( m_PinReset, HIGH );  
    delay( 4000 ); 
}

bool WifiMt7681::SetupAp( String Ssid, String Pwd, String ApIP, int ApPort, byte Ecn )
{
    String AtCmd;
    String CmdResult;
        
    AtCmd = "WA=1";
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
        
    AtCmd = "Assid=" + Ssid;
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
    
    AtCmd = "Assidl=";
    AtCmd.concat(Ssid.length());
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;        
        
    if( Ecn == 4 || Ecn == 7 || Ecn == 9 )
    {
        AtCmd = "Aam=";
        AtCmd.concat( Ecn );
    }
    else 
    {
        AtCmd = "Aam=9";
    }
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
    
    AtCmd = "Apw=" + Pwd;
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
        
    AtCmd = "Apwl=";
    AtCmd.concat( Pwd.length() );
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
    /*
    AtCmd = "dhcp=0";
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
    */
    AtCmd = "Aip=" + ApIP;
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
        
    AtCmd = "mask=255,255,255,0";
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
        
    AtCmd = "gw=" + ApIP;
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
    
    AtCmd = "dns=" + ApIP;
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
    
    AtCmd = "UType=1";
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
           
    AtCmd = "ULPort=";    
    AtCmd.concat( ApPort );
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
    
    /*    
    if( !SetSerialParameter() )
        return false;
    */
    
    delay(1000);
    
    //重新啟動模組;//      
    AtCmd = "Rb=1";
    CmdResult = SendAtCmd( AtCmd, eREPLY_NONE );
    if( CmdResult.length() != 0 )
        return false;
    //模組重新啟動;//
    DBGL( "WIFI Reboot....." );
    
    //檢查啟動狀態;//
    if( CheckModular() == false )
    {
        DBGL( "WIFI Reboot Error....." );
        return false;
    }
   
    return true;
}

bool WifiMt7681::SetupSta( String Ssid, String Pwd, String RemoteIP, int RemotePort, byte Ecn )
{
    String AtCmd;
    String CmdResult;
        
    AtCmd = "WA=0";
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
    
    AtCmd = "WM=2";
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
        
    AtCmd = "Sssid=" + Ssid;
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
    
    AtCmd = "Sssidl=";
    AtCmd.concat(Ssid.length());
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;        
        
    if( Ecn == 4 || Ecn == 7 || Ecn == 9 )
    {
        AtCmd = "Sam=";
        AtCmd.concat( Ecn );
    }
    else 
    {
        AtCmd = "Sam=9";
    }
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
    
    AtCmd = "Spw=" + Pwd;
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
        
    AtCmd = "Spwl=";
    AtCmd.concat( Pwd.length() );
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
    
    AtCmd = "dhcp=1";
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;

    AtCmd = "UType=2";
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
     
    AtCmd = "UIp=" + RemoteIP;
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
       
    AtCmd = "URPort=";
    AtCmd.concat( RemotePort );
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;  
           
    AtCmd = "ULPort=8080";
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
    
    AtCmd = "WC=1";
    CmdResult = SendAtCmd( AtCmd, eREPLY_NONE, 8000 );
    if( CmdResult.length() != 0 )
        return false;
    
    /*    
    if( !SetSerialParameter() )
        return false;
    */
    delay(1000);
    
    //重新啟動模組;//
    AtCmd = "Rb=1";
    CmdResult = SendAtCmd( AtCmd, eREPLY_NONE );
    if( CmdResult.length() != 0 )
        return false;
        
    //模組重新啟動;//
    DBGL( "WIFI Reboot....." );
    
    //檢查啟動狀態;//
    if( CheckModular() == false )
    {
        DBGL( "WIFI Reboot Error....." );
        return false;
    }
   
    return true;
}

String WifiMt7681::GetIP( byte Mode )
{
    String Data;
    String Cmd;
    
    if( Mode == 0 )
        Cmd = "Aip=?";
    else if( Mode == 1 )
        Cmd = "ip=?";
    else if( Mode == 2 )
        Cmd = "UIp=?";
    else
        return "";
    
    Data = SendAtCmd( Cmd, eREPLY_DATA );
    
    char head[4] = {0x0D,0x0A};  
    Data.replace( "at+", "" );
    Data.replace( ",", "." );
    Data.replace( Cmd, "" );
    Data.replace( head, "" );    
    
    return Data;   
}

int WifiMt7681::GetPort( byte Mode )
{    
    String Data;
    String Cmd;
    
    if( Mode == 0 )
        Cmd = "ULPort=?";
    else if( Mode == 1 )
        Cmd = "URPort=?";

    Data = SendAtCmd( Cmd, eREPLY_DATA );
    
    char head[4] = {0x0D,0x0A};  
    Data.replace( "at+", "" );
    Data.replace( Cmd, "" );
    Data.replace( head, "" );  
        
    return Data.toInt();
}

void WifiMt7681::StartTransport( bool Start )
{
    if( Start == true )
    {
        //啟動傳輸,開啟透傳模式;//
        SetTransportMode( true );
    }
    else
    {
        //結束傳輸,設定回At模式;//
        SetTransportMode( false );
    }
}

//透過Wifi傳送資料;//
bool WifiMt7681::Send( String Data )
{
    //資料都是透過透傳模式傳輸,所以直接把資料送往WIFI模組即可;//    
    Uart.print( Data );
    delay(100);
    return true;
}

bool WifiMt7681::Send( char Character )
{    
    Uart.print( Character );
    delay(100);
    return true;
}

bool WifiMt7681::SendL( String Data )
{
    Uart.println( Data );
    delay(100);
    return true;
}

//從Wifi接收資料;//
int WifiMt7681::Receive( char *Buffer )
{
    return 0;
}

void WifiMt7681::SetTransportMode( bool Open )
{
    //開啟透傳模式;//
    if( Open == true )
    {
        DBGL( "WIFI Open TS MODE" );
        SendAtCmd( "TS=1", eREPLY_OK );
        return;
    }
    
    //開啟AT模式,ES腳位電位拉低0.5至5秒之間;//
    digitalWrite( m_PinEs, LOW );
    delay( 1000 );
    digitalWrite( m_PinEs, HIGH );
    DBGL( "WIFI Close TS MODE" );
}

bool WifiMt7681::CheckModular()
{
    unsigned long Start;
    Start = millis();
    while( millis() - Start < 15000 ) 
    {
        //切換為AT Mode;//
        SetTransportMode( false );
        
        //傳送字串,看是否有收到任何回應;//
        String RecvData = SendAtCmd( "",  eREPLY_DATA );
        
        //表示有收到回應,模組已啟動;//
        if( RecvData.length() != 0 )
        {
            //清空Uart資料;//
            DBG( "Trash DATA :" );
            while( Uart.available() > 0 )
            {
                char Trash = Uart.read();
                DBG( Trash );
            }
            Uart.flush();
            DBGL();
            return true;
        }
    }
    
    return false;    
}

bool WifiMt7681::SetSerialParameter()
{
    String AtCmd;
    String CmdResult;
    
    AtCmd = "Ub=";
    AtCmd.concat( UART_BAUDRATE );
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
    
    AtCmd = "Ud=8";
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
    
    AtCmd = "Up=0";
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
    
    AtCmd = "Us=1";
    CmdResult = SendAtCmd( AtCmd );
    if( CmdResult.length() != 0 )
        return false;
    
    return true;
}

String WifiMt7681::SendAtCmd( String AtCmd, ReplyMode Mode, unsigned long WaitTime )
{    
    unsigned long Start;
    String SendCmd;
    String RecvData;
    SendCmd = "at+" + AtCmd + "\r\n";
    DBG("CMD: ");
    DBG(SendCmd);
    Uart.print( SendCmd );
    
    switch( Mode )
    {
        case eREPLY_OK:
        {            
            Start = millis();
            while( millis() - Start < WaitTime ) 
            {
                if(Uart.find("ok\r\n")==true)
                {
                    DBGL("CMD OK");
                    return "";
                }
            }
        }
        break;
        
        case eREPLY_DATA:
        {
            Start = millis();
            for( int i=0; i<3; i++ )
            {
                if( WaitTime == 7000 )
                    WaitTime = 3000;
                
                while( millis() - Start < WaitTime ) 
                {
                    while( Uart.available() > 0 )
                    {
                        char a = Uart.read();
                        RecvData = RecvData + a;
                    }
                    if( RecvData.indexOf( SendCmd ) != -1 )
                    {
                        if ( RecvData.indexOf( "\r\n", SendCmd.length()+1 ) != -1 )
                        {
                            DBG("Recv AtCmd Data: ");
                            DBG(RecvData);
                            return RecvData;
                        }
                    }
                }
            }
            return "";
        }        
       
        case eREPLY_NONE:
        {
            if( WaitTime != 7000 )
            {
                delay( WaitTime );
            }
            return "";
        }
        break;
        
        default:
            return "ERROR";
    }
    
    DBGL("CMD ERROR");
    return "ERROR";
}
