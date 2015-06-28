
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
#define count 7 ///// Can be either 7 for the A parking lot or 4  for the B parking lot/////
#define speed 80
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

        // This method will do THE MAIN data processing job.
        ModuleState::MODULE_EXITCODE Driver::body() {
	  timeval time1,time2,time3,time4,time5,storedtime,storedtime2;
	  float distance;
	  int counter = 0;
	  //int count = 5;
	  bool readIR= true;
	  bool gettime= true;
	  bool gettime2= true;		//can be removed later
	  bool gettime3= true;		//can be removed later
	  bool parking= false;
	  bool turning = false;
	  bool turning2= false;
	  //bool CheckScenario= true;
	  double timeshift;
	  double timeshift2;
	  double timeshift3;
	  double Timechange;
	  double Adjust = 0;
	  VehicleControl vc;

	  while (getModuleState() == ModuleState::RUNNING) {
	    // In the following, you find example for the various data sources that are available:
	    
	    // 1. Get most recent vehicle data:
	    Container containerVehicleData = getKeyValueDataStore().get(Container::VEHICLEDATA);
	    VehicleData vd = containerVehicleData.getData<VehicleData> ();
	    cerr << "Most recent vehicle data: '" << vd.toString() << "'" << endl;
	    
	    // 2. Get most recent sensor board data:
	    Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
	    SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
	    cerr << "Most recent sensor board data: '" << sbd.toString() << "'" << endl;
	    
	    // 3. Get most recent user button data:
	    Container containerUserButtonData = getKeyValueDataStore().get(Container::USER_BUTTON);
	    UserButtonData ubd = containerUserButtonData.getData<UserButtonData> ();
	    cerr << "Most recent user button data: '" << ubd.toString() << "'" << endl;
	    
	    // 4. Get most recent steering data as fill from lanedetector for example:
	    Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
	    SteeringData sd = containerSteeringData.getData<SteeringData> ();
	    cerr << "Most recent steering data: '" << sd.toString() << "'" << endl; 


	 /*	int USFrontRight = sbd.getDistance(4);
	 	if(CheckScenario){
	 	if (USFrontRight< 10)
	  		count = 4;
	    if (USFrontRight> 10)
	  	    count = 7;
	    CheckScenario = false; 
		}
*/
	    if (count ==4){
	  		turning = true;
	  	}else if(count==7){
	  	    turning = false;
	  		Adjust = 4;
	    }else if(count == 100){
	  		turning = false;
	    }



	    if(turning){
	       	if(gettime2){
	       		gettimeofday(&time5,NULL);
		     	storedtime2 = time5;
		     	gettime2=false;
	      	}
	     	gettimeofday(&time4,NULL);
	      
	        timeshift2 = (time4.tv_sec-storedtime2.tv_sec) * 1000;
	        timeshift2 += (time4.tv_usec-storedtime2.tv_usec) / 1000;
	        timeshift2 = timeshift2/1000;

	        cerr << "time SHIFT 2: '" << timeshift2 << "'" << endl;
	        if(timeshift2>1.0&&timeshift2<2.0){
	    		vc.setSteeringWheelAngle(0);
	    		vc.setSpeed(80);
	    	}
	        if(timeshift2>5.0&&timeshift2<17.0){
	    		vc.setSteeringWheelAngle(24);
	    		vc.setSpeed(80);
	    		cerr << "Let's turn a bit to the Right"  << endl;
	    	}
		    if(timeshift2>17.5&&timeshift2<27.8){
		        vc.setSteeringWheelAngle(-25);
		        vc.setSpeed(80);
		        cerr << "Now let's turn a bit to the Left"  << endl;
		   	}

		    if(timeshift2>28.7&&timeshift2<49.0){
		    	vc.setSteeringWheelAngle(0);
		    	vc.setSpeed(80);
		    	cerr << "Good good, Now let's drive Straight"  << endl;
		    }

		    if(timeshift2>50.0&&timeshift2<63.3){
		    	vc.setSteeringWheelAngle(-25);
		    	vc.setSpeed(80);
		    	cerr << "Alright, Now we drive Left"  << endl;
		    }

		    if(timeshift2>45.7&&timeshift2<74.5){
		    	vc.setSteeringWheelAngle(0);
		    	vc.setSpeed(45);
		    	cerr << "Now backwards, to check the first park"  << endl;
		    }

		    if(timeshift2>75.5&&timeshift2<75.9){
		    	vc.setSteeringWheelAngle(0);
		    	vc.setSpeed(80);
		    	counter=0;
		    	turning=false;
		    }
	    }

cerr << "the IR data '" << sbd.getDistance(0) << "'" << endl;
//cerr << "the US data '" << sbd.getDistance(4) << "'" << endl;
		if(parking==false && turning == false && turning2 == false){
		    vc.setSpeed(speed);

		    int IRFrontRight = sbd.getDistance(0);
		    
		    if(IRFrontRight> 250 && IRFrontRight<800 && readIR){
		      gettimeofday(&time1,NULL);
		      readIR = false;
		    }
		    if((IRFrontRight<250 || IRFrontRight>800) && readIR == false){
		      gettimeofday(&time2,NULL);
		      counter++;
		      readIR = true;
		    }
		    
		     Timechange = (time2.tv_sec-time1.tv_sec) * 1000;
		     Timechange += (time2.tv_usec-time1.tv_usec) / 1000;
		     Timechange = Timechange/1000;
		    cerr << "time change: '" << Timechange << "'" << endl;
		    
		    distance = speed*Timechange;
		    cerr << "Distance '" << distance << "'" << endl;
		    cerr << "counter '" << counter << "'" << endl;


		    if(counter<count){
		   		if(Timechange > 3 && Timechange < 5){
		      		parking= true;
		   		}
		    }else{
		    	vc.setSpeed(speed);
				turning2 = true;	
		    }
	    }		

	    if(parking && turning == false){

		    if(gettime){
				storedtime = time2;
				gettime=false;
		    }
		    gettimeofday(&time3,NULL);
		    	cerr << "Nice '" << Timechange << "' more than enough!" << endl;
		    	timeshift = (time3.tv_sec-storedtime.tv_sec) * 1000;
		    	timeshift += (time3.tv_usec-storedtime.tv_usec) / 1000;
		    	timeshift = timeshift/1000;
		    	cerr << "time SHIFT: '" << timeshift << "'" << endl;

		    if(timeshift>1.0 &&timeshift<1.5){
		    	vc.setSteeringWheelAngle(25);
			vc.setSpeed(74);
			usleep(1000000);	
		    	vc.setSpeed(45);
		    }
		    if(timeshift>10&&timeshift<10.5){
	        	vc.setSpeed(74);
	    	}
	    }
	   
	    

	    if(turning2){
	    	if(gettime3){
	       		gettimeofday(&time4,NULL);
		     	storedtime2 = time4;
		     	gettime3=false;
	      	}

	     	gettimeofday(&time5,NULL);

	        timeshift3 = (time5.tv_sec-storedtime2.tv_sec) * 1000;
	        timeshift3 += (time5.tv_usec-storedtime2.tv_usec) / 1000;
	        timeshift3 = timeshift3/1000;

	        cerr << "time SHIFT 3: '" << timeshift3 << "'" << endl;
	        if(timeshift3>4.0&&timeshift3<18.4){
	    		vc.setSteeringWheelAngle(-25);
	    		vc.setSpeed(80);
	    		cerr << "No parking spot, let's make a turn left!" << endl;
	    	}
	        if(timeshift3>19.1&&timeshift3<25.5){
	    		vc.setSteeringWheelAngle(0);
	    		vc.setSpeed(45);
	    		cerr << "Now backwards!" << endl;
	    	}

		    if(timeshift3>(26.4+Adjust)&&timeshift3<(42.8+Adjust)){
		        vc.setSteeringWheelAngle(-25);
		        vc.setSpeed(80);
		        cerr << "Now more left!" << endl;
		   	}
		    if(timeshift3>(43.6+Adjust)&&timeshift3<(52.0+Adjust)){
		    	vc.setSteeringWheelAngle(0);
		    	vc.setSpeed(45);
		    	cerr << "Now backwards, to check the first park"  << endl;
		    }
		    if(timeshift3>(53.5+Adjust)&&timeshift3<(53.9+Adjust)){ //Was 52.5
		    	vc.setSteeringWheelAngle(0);
		    	vc.setSpeed(80);
		    	counter=0;
		    	turning2=false;
		    }
	    }
	    
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
