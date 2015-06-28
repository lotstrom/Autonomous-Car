
#include <stdio.h>
#include <math.h>

#include "core/io/ContainerConference.h"
#include "core/data/Container.h"
#include "core/data/Constants.h"
#include "core/data/control/VehicleControl.h"
#include "core/data/environment/VehicleData.h"
#include <pthread.h>

// Data structures from msv-data library:
#include "SteeringData.h"
#include "SensorBoardData.h"
#include "UserButtonData.h"

#include "Driver.h"


 
namespace msv {

        using namespace std;
        using namespace core::base;
        using namespace core::data;
        using namespace core::data::control;
        using namespace core::data::environment;

        Driver::Driver(const int32_t &argc, char **argv) :
	        ConferenceClientModule(argc, argv, "Driver") {
        }

        Driver::~Driver() {}

        void Driver::setUp() {
	        // This method will be call automatically _before_ running body().
        }

        void Driver::tearDown() {
	        // This method will be call automatically _after_ return from body().
        }

        // This method will do the main data processing job.
        ModuleState::MODULE_EXITCODE Driver::body() {

        double speed = 2;
        int counter = 0;
        int INTERSECTION = 1;
        int STRAIGHT = 1;

 

	        while (getModuleState() == ModuleState::RUNNING) {
                // In the following, you find example for the various data sources that are available:

		        // 1. Get most recent vehicle data:
		        Container containerVehicleData = getKeyValueDataStore().get(Container::VEHICLEDATA);
		        VehicleData vd = containerVehicleData.getData<VehicleData> ();
		        //cerr << "Most recent vehicle data: '" << vd.toString() << "'" << endl;

		        // 2. Get most recent sensor board data:
		        Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
		        SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
		        //cerr << "Most recent sensor board data: '" << sbd.toString() << "'" << endl;

		        // 3. Get most recent user button data:
		        Container containerUserButtonData = getKeyValueDataStore().get(Container::USER_BUTTON);
		        UserButtonData ubd = containerUserButtonData.getData<UserButtonData> ();
		        //cerr << "Most recent user button data: '" << ubd.toString() << "'" << endl;

		        // 4. Get most recent steering data as fill from lanedetector for example:
		        Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
		        SteeringData sd = containerSteeringData.getData<SteeringData> ();
                
                Container containerPoint = getKeyValueDataStore().get(Container::USER_DATA_2);
		        SteeringData p = containerPoint.getData<SteeringData> ();

                Container containerIntersection = getKeyValueDataStore().get(Container::USER_DATA_3);
		        SteeringData i = containerIntersection.getData<SteeringData> ();
                
                int point = p.getExampleData();

                double intersection = i.getExampleData();
                double degrees = sd.getExampleData();
                double steer;
        
                // Create vehicle control data.
		        VehicleControl vc;

                // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
		        vc.setSpeed(speed);
                


// Looking for intersections by  around 180 degree angles.
if(INTERSECTION == 1) {
    if(intersection < 185 && intersection > 175) {
        speed = 0;
        cout << "STOPPING... at angle: '" << intersection << "'" << endl;
        counter++;
    
        // Starts moving when still for 80 readings.
        if (counter == 80) {
            cout << "CONTINUING...'" << endl;
            speed = 2;
            INTERSECTION = 0;
            counter = 0;        
        }
    }
}
                
if(STRAIGHT == 1) {
    
    // Ignoring Intersection angles in lane detector.
    if(degrees > 130 && degrees < 140) {
        steer = 0;
    }

    // Desired range of X-axis positions of dash line.
    else if(point < 110 && point > 70) {
        
        // Keeps driving straight.
        if((degrees < 95 && degrees > 85)||(degrees < -85 && degrees > -95)) {
            steer = 0;
        }           
        // Starts left curve.
        else if(degrees > -150 && degrees < -50) {
            steer = -5;
        }
        // Starts right curve.
        else if(degrees > 60 && degrees < 150) {
            steer = 15;
        }
    }

    // When car is slightly deviated to the right.
    else if(point > 40 && point <71 ) {
        
        // Stabilize
        if((degrees < 92 && degrees > 88)||(degrees < -88 && degrees > -92)) {
            steer = -2.5;
        }                          
        // Sharper left turn
        else if(degrees > -150 && degrees < -50) {
            steer = -10;        
        }
        // Stabilize
        else if(degrees >60 && degrees < 150) {
            steer = 5;                         
        }
    } 

    // When car is slightly deviated to the left.
    else if(point > 109 && point < 140 ) {
            
        // Stabilize
        if((degrees < 92 && degrees > 88)||(degrees < -88 && degrees > -92)) {
            steer = 2;
        }      
        // Stabilize             
        else if(degrees > -150 && degrees < -50) {
            steer = 0.5;             
        }
        // Sharper right turn        
        else if(degrees > 60 && degrees < 150) {
            steer = 15;             
        }
    }             
    
    // When car is deviating extremely to the right.        
    else if(point < 41) {

        // Turn left
        if((degrees < 92 && degrees > 88)||(degrees < -88 && degrees > -92)) {
            steer = -5;
        }      
        // Sharper left turn             
        else if(degrees > -150 && degrees < -50) {
            steer = -12.5;
        }
        // Stabilize
        else if(degrees > 60 && degrees < 150) {
            steer = -1;           
        }
    }   

    // When car is deviating extremely to the left.    
    else if(point > 139) {
           
        // Turn right
        if((degrees < 92 && degrees > 88)||(degrees < -88 && degrees > -92)) {
            steer = 2.5;
        }      
        // Stabilize             
        else if(degrees > -150 && degrees < -50) {
            steer = 1;
        }
        // Sharper right turn       
        else if(degrees > 60 && degrees < 150) {
            steer = 20;                       
        }
    }
}

 //Very simple, check if there is an actual degree, if not its 0
                 
                 
		        cerr << "Most recent steering data: '" << degrees << "'" << endl;
                cerr << "steering angle: '" << steer << "'" << endl;



                // Design your control algorithm here depending on the input data from above.



		       

                // With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
                double desiredSteeringWheelAngle = steer; // 4 degree but SteeringWheelAngle expects the angle in radians!
		        vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);

                // You can also turn on or off various lights:
                vc.setBrakeLights(false);
                vc.setLeftFlashingLights(false);
                vc.setRightFlashingLights(true);

		        // Create container for finally sending the data.
		        Container c(Container::VEHICLECONTROL, vc);
		        // Send container.
		        getConference().send(c);
	        }

	        return ModuleState::OKAY;
        }
} // msv

