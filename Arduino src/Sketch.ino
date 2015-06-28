/* ---------------------------------------------------------------------------
** 
** MSV.ino
** Arduino main code.
**  -> Decodes/receives received steering data from serial
**  -> Encodes/sends sensor data through serial
**
** -------------------------------------------------------------------------*/
#include <ComponentController.h>
#include <ProxyData.h>
#include <Servo.h>




ComponentController Control;

//char serialBuffer[50];
//char packetBuffer[50];
boolean validPacket = false;
int packet_len = 0;
boolean go = true;
String payload;
struct ServoEscPacket m_payload;


Servo servo;
Servo esc;

int main(void)
{
  //Initialise arduino
  init();
#if defined(USBCON)
  USB.attach();
#endif

  //Setup

  Serial.begin(115200);
  Control.setup();

  //esc.writeMicroseconds(1350);
  //Loop
  delay(1000);
  Control.writeServo(95);
  while ( true ) {
    //Code loop
    delay(10);
    //servo.write(servoAngle);
    
    if (serialEventRun) {  
      //if (Serial.available()<1) Serial.println(getGoSignal(m_payload._servo,m_payload._motor));
      
      while (Serial.available()) {
        //Clear internal buffers
        go=true;
        char c = (char)Serial.read();
        if (!isDigit(c)) packet_len=0;
        else {
          packet_len = packet_len*10+(int(c)-'0');
          if (packet_len>50) {
            go=false;
            packet_len = 0;
            //may drop entire packet
          }
          if (Serial.peek() == 0x23 && go){
            int length = packet_len;
            Serial.read();
            //Serial.println(length);
            char packetBuffer[length];
            //if(packet_len !=4 ) errors++;
            for (int i=0;i<length;i++)
            {
              packetBuffer[i]=Serial.read();
            } //end for
            
            if (Serial.read() == 0x21) {              
              //Serial.println(packetBuffer);
              payload = packetBuffer;
              m_payload = decode(packetBuffer);
              validPacket = true;
            }
            packet_len = 0;
          } //end if
          
        }//end else
      }//end serial while
    }//end serial if
    //Serial.println("stuff");
    if(validPacket) {
        if (m_payload._servo!=0) Control.writeServo(m_payload._servo);
        validPacket = false;
        Control.writeEsc(m_payload._motor);
      }
    //Serial event loop
    delay(25);
    Serial.print(getSensorReply());
  }
  return 0;
}


String getSensorReply(){
  //Allocate buffer
  char buffer[MAX_IrUsPacket_SIZE];
  //Create IrUs struct 
  struct IrUsPacket packet;  
  //To-Do - get real sensor data
  packet._ir1 = Control.read_IR(); 
  packet._us1 = Control.read_US_1();
  packet._us2 = Control.read_US_2();
  //Write struct to buffer
  int len = IrUsPacket_write_delimited_to(&packet,buffer,0);
  String reply(buffer);
  String strLen (reply.length(),DEC);
  return strLen+"#"+reply+"!";
}
//Decode packet
struct ServoEscPacket decode(char *buffer)
{
  struct ServoEscPacket data;
  ServoEscPacket_read_delimited_from(buffer,&data,0);
  return data;
}

