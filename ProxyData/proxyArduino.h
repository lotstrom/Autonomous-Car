/* ---------------------------------------------------------------------------
** 
** proxyArduino.h
** Header file for Arduino Proxy.
**  
**  Manages transfer of data between the Udoo board and Arduino
**
** Author: Robert Petre
** -------------------------------------------------------------------------*/
#ifndef PROXYARDUINO_H_
#define PROXYARDUINO_H_

#include "core/base/ConferenceClientModule.h"
#include "core/base/Mutex.h"
#include "core/wrapper/ConnectionListener.h"
#include "core/wrapper/AbstractProtocol.h"
#include "core/wrapper/StringSender.h"
#include "core/wrapper/SerialPortFactory.h"
#include "core/wrapper/SerialPort.h"
#include "ProxyData.h"

namespace msv {
	using namespace std;
	class proxyArduino : public core::base::ConferenceClientModule, public core::wrapper::ConnectionListener, public core::wrapper::AbstractProtocol {
	private:
		proxyArduino(const proxyArduino &);
		proxyArduino& operator=(const proxyArduino &);
	public:
		proxyArduino(const int32_t &argc, char **argv);
		virtual ~proxyArduino();
		core::base::ModuleState::MODULE_EXITCODE body();
		virtual void handleConnectionError();
		virtual void receivedPartialString(const string &partialData);
	private:
		virtual void setUp();
        virtual void tearDown();
        string encodePacket(struct ServoEscPacket packet);
        void decodePacket();
        struct ServoEscPacket getVehicleData();
        int mapAngle(int angle);
        void updateSensorData(struct IrUsPacket data);
	private:
		stringstream m_partialData;
		string m_receivedPacket;
		bool canSend;
		core::wrapper::SerialPort *serialPort;
		bool firstSend;
	};
}

#endif