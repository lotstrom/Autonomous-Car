/* ---------------------------------------------------------------------------
** 
** proxyArduino.cpp
** Source file for Arduino Proxy.
**  
**  Manages transfer of data between the Udoo board and Arduino
**
** -------------------------------------------------------------------------*/
#include <cstring>
#include <cmath>

#include <string>

#include "core/macros.h"
#include "core/base/KeyValueConfiguration.h"
#include "core/io/URL.h"
#include "core/wrapper/SerialPortFactory.h"
#include "core/wrapper/SerialPort.h"
#include "core/io/ContainerConference.h"
#include "core/data/Container.h"
#include "core/data/Constants.h"
#include "core/data/control/VehicleControl.h"
#include "core/data/environment/VehicleData.h"

#include "SteeringData.h"
#include "SensorBoardData.h"
#include "proxyArduino.h"
#include "ProxyData.h"

#include <stdexcept>

namespace msv{
	using namespace std;
	using namespace core::base;
	using namespace core::io;
	using namespace core::data;
	using namespace core::data::control;
	using namespace core::data::environment;
	proxyArduino::proxyArduino(const int32_t &argc, char **argv) :
		ConferenceClientModule(argc, argv, "proxy-Arduino"),
		m_partialData(),
		m_receivedPacket() 
		{}
	
	proxyArduino::~proxyArduino(){

	}

	void proxyArduino::handleConnectionError(){
		cerr<< "[Proxy] Connection failed."<<endl;

	}
	void proxyArduino::setUp() {
	    // This method will be call automatically _before_ running body().
    }

    void proxyArduino::tearDown() {
	    // This method will be call automatically _after_ return from body().
	    struct ServoEscPacket end;
	    end._servo = 95;
	    end._motor = 74;
	    serialPort->send(encodePacket(end));
	    usleep(1000000);
	    serialPort->stop();
        OPENDAVINCI_CORE_DELETE_POINTER(serialPort);
	    //serialPort->stop();
        //OPENDAVINCI_CORE_DELETE_POINTER(serialPort);
	    cout << "called !!";
    }
	void proxyArduino::receivedPartialString(const string &s){
		m_partialData.write(s.c_str(), s.length());

        //cout << "Received: '" << s << "', Buffer: '" << m_partialData.str() << "'" << endl;
        //Decode
        try {
        	decodePacket();
        }
        catch(std::out_of_range const& e){
        	cout << "Out of range caught" << endl;
        	m_partialData.str("");
        }
        m_partialData.seekp(0, ios_base::end);

	}
	string proxyArduino::encodePacket(struct ServoEscPacket packet)
	{		
		if(firstSend) packet._motor=74;
		//else if(packet._motor!=74) packet._motor=79;
		cout<<"Servo:" << packet._servo << " Esc:" << packet._motor << endl;
		char buffer[MAX_ServoEscPacket_SIZE];
		int len = ServoEscPacket_write_delimited_to(&packet,buffer,0);
		string p(buffer,len);
		stringstream sstr;
		if (p.length() > 0){
			sstr << (int)p.length() << "#" << p << "!";
		}
		else {
			sstr <<"0#!";
		}
		return sstr.str();
	}
	void proxyArduino::decodePacket(void)
	{
		m_partialData.seekg(0, ios_base::beg);

        // Start decoding only if we have received enough data.
        while(m_partialData.str().length() > 3) {
        	//cout << "still gettin serial" <<endl;
            unsigned int lengthOld = m_partialData.str().length();
            const char *receiveBuffer = m_partialData.str().c_str();
            char *delim = NULL;
            unsigned int lengthOfPayload = strtol(receiveBuffer, &delim, 10);
            if (lengthOfPayload == 0) {
                // Remove empty Netstring: 0:, (size is 3).

                m_partialData.str(m_partialData.str().substr(3));
                continue; // Try to decode next Netstring if any.
            }
            //debug
            //cout << "delim : " << delim[0] << endl;
            
            if (*delim == 0x23) {
            	//cout<<delim[1+lengthOfPayload]<<endl;
                // Found colon sign. Now, check if (receiveBuffer + 1 + lengthOfPayload) == ','.
                if ((delim[1 + lengthOfPayload]) == 0x21) {
                    // Successfully found a complete Netstring.
                    m_receivedPacket = m_partialData.str().substr((delim + 1) - receiveBuffer, lengthOfPayload);
                    canSend = true;
                    //cout << "can send" <<endl;
                    // Remove decoded Netstring: "<lengthOfPayload> : <payload> ,"
                    int lengthOfNetstring = (delim + 1 + lengthOfPayload + 1) - receiveBuffer;

                    m_partialData.str(m_partialData.str().substr(lengthOfNetstring));
                }
            }

            if (lengthOld == m_partialData.str().length()) {
                // This should not happen, received data might be corrupt.

                // Reset buffer.
                m_partialData.str("");
            }
        } 

	}
	struct ServoEscPacket proxyArduino::getVehicleData(){
		struct ServoEscPacket servoPacket;
		Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
		Container containerVehicleControl = getKeyValueDataStore().get(Container::VEHICLECONTROL);
		SteeringData sd = containerSteeringData.getData<SteeringData> ();
		VehicleControl vc = containerVehicleControl.getData<VehicleControl> ();
		int speed = (int)vc.getSpeed();
		//int angle = (int)sd.getExampleData();
		int angle = mapAngle((int)sd.getExampleData());
		servoPacket._servo=angle;
		servoPacket._motor=speed;
		return servoPacket;
	}
	int proxyArduino::mapAngle(int angle)
{
    int realWorldAngle;
    angle *= -1;
    if (angle > 0)
    {
        realWorldAngle = (int) angle ;
    }
    else if (0 > angle)
    {
        realWorldAngle = (int) angle ;
    }
    else
    {
        realWorldAngle = 0;
    }

    realWorldAngle += 90;
    // cout << "Angle is : " << angle << ", realWorldAngle is : " << realWorldAngle << endl;
    if (realWorldAngle>130) realWorldAngle = 130;
    if (realWorldAngle<70) realWorldAngle = 70;
    return realWorldAngle;
}

	void proxyArduino::updateSensorData(struct IrUsPacket data){
		//Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
		cout << "IR : "<< data._ir1 <<"US 1: " << data._us1 << "US 2: " << data._us2 << endl;
		SensorBoardData sbd;
		sbd.update(0,data._ir1);
		sbd.update(3,data._us1);
		sbd.update(4,data._us2);
		Container c(Container::USER_DATA_0,sbd);
		getConference().send(c);
	}

	ModuleState::MODULE_EXITCODE proxyArduino::body(){
		
		const string SERIAL_PORT = "/dev/ttyACM0";
		const uint32_t SERIAL_SPEED = 115200;
		canSend = false;
		cerr << "[Proxy] Connecting to device " << SERIAL_PORT << " at " << SERIAL_SPEED << " bps" << endl;

		serialPort = core::wrapper::SerialPortFactory::createSerialPort(SERIAL_PORT, SERIAL_SPEED);
		serialPort->setPartialStringReceiver(this);
		serialPort->setConnectionListener(this);
		serialPort->start();
		usleep(1000000);
		//Send start packet (fix arduino crash)
		//serialPort->send("5#start!");

		//string testString = "4:85:1500:";
		firstSend = true;
		while (getModuleState() == ModuleState::RUNNING){
			if(canSend){
				string toSend = encodePacket(getVehicleData());
				cout << "Encoded Buffer: " << toSend << endl;
				serialPort->send(toSend);
				if (firstSend) {
					usleep(5000000);
					firstSend = false; }
				canSend = false;
			}

			usleep(10000);
			//cout << "running";
			struct IrUsPacket sensorData;
			//cout << "Received packet: '" << m_receivedPacket<< "'" << endl;
			IrUsPacket_read_delimited_from((char*)m_receivedPacket.c_str(), &sensorData, 0);
			//cout << "IR:" << sensorData._ir1 << "US1:" << sensorData._us1 << "US2:" << sensorData._us2 << endl;
			updateSensorData(sensorData);
			//serialPort->send("\r\n");
			//toSend << "4:"<<(int) packet._servo <<":"<<(int) packet._motor<<":";
			//cout << "Sending: " << toSend.str() << endl;
			//serialPort->send(toSend.str());	
			//cout<<servoPacket._servo<<endl;
		}


		
		return ModuleState::OKAY;
	}

}