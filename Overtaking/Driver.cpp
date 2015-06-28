
#include <stdio.h>
#include <math.h>
#include <sys/time.h>


#include "core/io/ContainerConference.h"
#include "core/data/Container.h"
#include "core/data/Constants.h"
#include "core/data/control/VehicleControl.h"
#include "core/data/environment/VehicleData.h"

// Data structures from msv-data library:
#include "SteeringData.h"
#include "SensorBoardData.h"
#include "UserButtonData.h"

#include "Driver.h"
#include "ProxyArduino.h"

namespace msv
{

int oldAngle = 0;

using namespace std;
using namespace core::base;
using namespace core::data;
using namespace core::data::control;
using namespace core::data::environment;

Driver::Driver(const int32_t &argc, char **argv) :
    ConferenceClientModule(argc, argv, "Driver")
    // ,prox()
{
}

Driver::~Driver() {}

void Driver::setUp()
{
    // This method will be call automatically _before_ running body().
}

void Driver::tearDown()
{
    // This method will be call automatically _after_ return from body().
}

// This method will do the main data processing job.
ModuleState::MODULE_EXITCODE Driver::body()
{
  // timeval time1,time2,storedtime2;
     //double timeshift2;
      //bool getTime =true;

    while (getModuleState() == ModuleState::RUNNING)
    {
        // In the following, you find example for the various data sources that are available:

        // // 1. Get most recent vehicle data:
        // Container containerVehicleData = getKeyValueDataStore().get(Container::VEHICLEDATA);
        // VehicleData vd = containerVehicleData.getData<VehicleData> ();
        // // cerr << "Most recent vehicle data: '" << vd.toString() << "'" << endl;

        // 2. Get most recent sensor board data:
        Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
        SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
        cerr << "Most recent sensor board data: '" << sbd.toString() << "'" << endl;

        // // 3. Get most recent user button data:
        // Container containerUserButtonData = getKeyValueDataStore().get(Container::USER_BUTTON);
        // UserButtonData ubd = containerUserButtonData.getData<UserButtonData> ();
        // // cerr << "Most recent user button data: '" << ubd.toString() << "'" << endl;

        // // 4. Get most recent steering data as fill from lanedetector for example:
        Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
        SteeringData sd = containerSteeringData.getData<SteeringData> ();
        // cerr << "Most recent steering data: '" << sd.toString() << "'" << endl;

//         double angle ;
// //sd.getExampleData();

//         cout << "Steering : " << angle << endl;

        int USFrontCenter = sbd.getDistance(3);
        int USFrontRight = sbd.getDistance(4);
        int IRFrontRight = sbd.getDistance(0); 
        bool Isovertake;
//bool straightovertake;
//bool Isovertake;

// if((USFrontCenter<45 && USFrontCenter>5) && (USFrontRight<45 && USFrontRight>5 )){
// //straightovertake=true;
// cerr << "overtake true: '" << endl;
// angle=35;

// }
//if((USFrontCenter<40 && USFrontCenter>5)&&straightovertake){
//angle=-20;
//Isovertake=true;
//}
//if(Isovertake&&(IRFrontRight<800 && IRFrontRight >300)){
//angle=20;
//straightovertake=false;

//}

       // if (oldAngle != (int) angle)
       // {
            //     oldAngle = (int) angle;
            //     sendProxy(oldAngle);
            //
            VehicleControl vc;


            if((USFrontCenter<55 && USFrontCenter>7) && (USFrontRight<55 && USFrontRight>7 )){
               
                 
                usleep(100000);
                Isovertake=true;

             
            }
            else {
           // vc.setSpeed(40);
          //  double desiredSteeringWheelAngle = angle;

                //angle from lanedetection 
                double angle = sd.getExampleData();
                vc.setSteeringWheelAngle((int)angle);
                Container c(Container::VEHICLECONTROL, vc);
                getConference().send(c);
            }

            if(Isovertake &&( USFrontCenter <50 && USFrontCenter>5)){
                 
              
           
            vc.setSteeringWheelAngle(-25);
            Container c(Container::VEHICLECONTROL, vc);
            getConference().send(c);
            usleep(2000000);

            }
            if(IRFrontRight>200&&IRFrontRight<400)
{
     vc.setSteeringWheelAngle(25);
            Container c(Container::VEHICLECONTROL, vc);
            getConference().send(c);
            usleep(2000000);

}
            //cerr << "Time: '" << timeshift2 << "'" << endl;



           // if(timeshift2>4&&timeshift2<10){

            //}
            int angle1 =vc.getSteeringWheelAngle();
            cout << "VC Angle" << angle1 << endl;
           // cout<<"time " << timeshift << endl;
       // }


        // vc.setBrakeLights(false);
        // vc.setLeftFlashingLights(false);
        // vc.setRightFlashingLights(true);

    }

    return ModuleState::OKAY;
}

// void Driver::sendProxy(double angle)
// {
//     int realWorldAngle;
//     // double constant = 30 / 26;
//     // 0-26 .. 100-
//     angle *= -1;
//     if (angle > 0)
//     {
//         realWorldAngle = (int) angle * 1.5;
//     }
//     else if (0 > angle)
//     {
//         realWorldAngle = (int) angle * 1.5;
//     }
//     else
//     {
//         realWorldAngle = 0;
//     }

//     realWorldAngle += 90;

//     // cout << "Angle is : " << angle << ", realWorldAngle is : " << realWorldAngle << endl;

//     prox.send_ServoEsc(realWorldAngle, 1465);
// }

} // msv

