
#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "core/base/KeyValueConfiguration.h"
#include "core/data/Container.h"
#include "core/data/image/SharedImage.h"
#include "core/io/ContainerConference.h"
#include "core/wrapper/SharedMemoryFactory.h"

// Data structures from msv-data library:
#include "SteeringData.h"
#include "LaneDetector.h"

namespace msv
{

using namespace std;
using namespace core::base;
using namespace core::data;
using namespace core::data::image;
using namespace cv;

LaneDetector::LaneDetector(const int32_t &argc, char **argv) : ConferenceClientModule(argc, argv, "lanedetector"),
	m_hasAttachedToSharedImageMemory(false),
	m_sharedImageMemory(),
	m_image(NULL),
	m_cameraId(-1),
	m_debug(false) {}

LaneDetector::~LaneDetector() {}

void LaneDetector::setUp()
{
	// This method will be call automatically _before_ running body().
	if (m_debug)
	{
		// Create an OpenCV-window.
		cvNamedWindow("WindowShowImage", CV_WINDOW_AUTOSIZE);
		cvMoveWindow("WindowShowImage", 300, 100);
	}
}

void LaneDetector::tearDown()
{
	// This method will be call automatically _after_ return from body().
	if (m_image != NULL)
	{
		cvReleaseImage(&m_image);
	}

	if (m_debug)
	{
		cvDestroyWindow("WindowShowImage");
	}
}

bool LaneDetector::readSharedImage(Container &c)
{
	bool retVal = false;

	if (c.getDataType() == Container::SHARED_IMAGE)
	{
		SharedImage si = c.getData<SharedImage> ();

		// Check if we have already attached to the shared memory.
		if (!m_hasAttachedToSharedImageMemory)
		{
			m_sharedImageMemory
				= core::wrapper::SharedMemoryFactory::attachToSharedMemory(
					  si.getName());
		}

		// Check if we could successfully attach to the shared memory.
		if (m_sharedImageMemory->isValid())
		{
			// Lock the memory region to gain exclusive access. 
			m_sharedImageMemory->lock();
			{
				const uint32_t numberOfChannels = 3;
			
				if (m_image == NULL)
				{
					m_image = cvCreateImage(cvSize(si.getWidth(), si.getHeight()), IPL_DEPTH_8U, numberOfChannels);
				}

				
				if (m_image != NULL)
				{
					memcpy(m_image->imageData,
						   m_sharedImageMemory->getSharedMemory(),
						   si.getWidth() * si.getHeight() * numberOfChannels);
				}
			}

			
			m_sharedImageMemory->unlock();

			// Mirror the image.
			cvFlip(m_image, 0, -1);

			retVal = true;
		}
	}
	return retVal;
}

void LaneDetector::processImage()
{
	if (m_debug)
	{
		if (m_image != NULL)
		{
            //Cropping the image
            double angle;
            double intersection_angle;

            
            // First part for intersection finder, second for lane detection.            
            IplImage* copy_image = m_image;
            cvSetImageROI(copy_image, cvRect(275, 0, 75, 175));
            Mat unflipped(copy_image);
            Mat copy;
            flip(unflipped, copy, 0);
            Mat intersect = copy;
            Mat temp = copy;
            cvtColor(temp, temp, CV_RGB2GRAY);
            Canny(temp, intersect, 50, 100, 3);
			cvtColor(intersect, copy , CV_GRAY2BGR);
            vector<Vec4i> inter;           
            HoughLinesP(intersect, inter, 1, CV_PI / 180, 50, 10, 10);
           if(inter.size() != 0) {
				Vec4i l = inter[0];
				line(copy, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
				intersection_angle = atan2(l[1] - l[3], l[0] - l[2]) * 180 / CV_PI;
                cout << "[INTERSECTION angle]: " << intersection_angle << endl;

            if(intersection_angle > 170  && intersection_angle < 190)
			    {
                    cout << "[INTERSECTION angle]: " << intersection_angle << endl;
				    SteeringData sd;
				    sd.setExampleData(intersection_angle);
				    Container c(Container::USER_DATA_3, sd);
				    getConference().send(c);                 
			    }
            }
            // Lane detection starts here.
            cvSetImageROI(m_image, cvRect(350, 0, 350, 200));            
			Mat img(m_image);
            // Rotating image for lane detection            
            int len = max(img.cols, img.rows);
            Point2f pt(len/2., len/2.);
            Mat r = getRotationMatrix2D(pt, 43.2, 1.0);
            warpAffine(img, img, r, Size(len, len));
			Mat canny = img;
			Mat gray = canny;
            cvtColor(img, img, CV_RGB2GRAY);
			Canny(img, canny, 50, 100, 3);
			cvtColor(canny, gray, CV_GRAY2BGR);
            
            //The lines are stores in this vector below, i get them through HoughLinesP
            //Which operates on the cropped image and gets middle line (dashed) only
			vector<Vec4i> lines;
			HoughLinesP(canny, lines, 1, CV_PI / 180, 50, 10, 10);

            //Loops through all lines found, and calculates the angle of the lines, stores it in var angle
			//for(size_t i = 0; i < lines.size(); i++)
			//{
            if(lines.size() != 0) {
				Vec4i l = lines[0];
				line(gray, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
				angle = atan2(l[1] - l[3], l[0] - l[2]) * 180 / CV_PI;

                if(l[1] > 0) {

                    cout << "[Point]: " << Point(l[0], l[1]) << endl;
                    SteeringData point;
                    point.setExampleData(l[0]);
                    Container d(Container::USER_DATA_2, point);
                    getConference().send(d);
                }                
		        if(angle > -500  && angle < 500)
			    {
                    cout << "[Sending angle]: " << angle << endl;
				    SteeringData sd;
				    sd.setExampleData(angle);
				    Container c(Container::USER_DATA_1, sd);
				    getConference().send(c);
                   
			    }
            }
            //IplImage* copy_image = new IplImage(gray);
            //cvSetImageROI(copy_image, cvRect(0, 200, 350, 200));
            
            
            imshow("Intersection", copy);
			imshow("LaneDetector", gray);
			cvWaitKey(10);			
		}
	}
}

// This method will do the main data processing job.  Therefore, it tries to
// open the real camera first. If that fails, the virtual camera images from
// camgen are used.
ModuleState::MODULE_EXITCODE LaneDetector::body()
{
	// Get configuration data.
	KeyValueConfiguration kv = getKeyValueConfiguration();
	m_cameraId =
		kv.getValue<int32_t> ("lanedetector.camera_id");
	m_debug =
		kv.getValue<int32_t> ("lanedetector.debug") == 1;

	bool use_real_camera = true;

	// Try to open the camera device.
	CvCapture *capture = cvCaptureFromCAM(m_cameraId);
	if (!capture)
	{
		cerr <<
			 "Could not open real camera; falling back to SHARED_IMAGE." <<
			 endl;
		use_real_camera = false;
	}

	while (getModuleState() == ModuleState::RUNNING)
	{
		bool has_next_frame =
			false;

		// Use the shared memory image.
		if (!use_real_camera)
		{
			// Get the most recent available container for a SHARED_IMAGE.
			Container c = getKeyValueDataStore().get(Container::SHARED_IMAGE);

			if (c.getDataType() == Container::SHARED_IMAGE)
			{
				// Example for processing the received container.
				has_next_frame = readSharedImage(c);
			}
		}
		else
		{
			// Use the real camera.
			if (cvGrabFrame(capture))
			{
				m_image = cvRetrieveFrame(capture);
				has_next_frame = true;
			}
		}

		// Process the read image.
		if (true == has_next_frame)
		{
			processImage();
		}

		if (use_real_camera)
		{
			// Unset m_image only for the real camera to avoid memory leaks.
			m_image = NULL;
		}
	}

	if (capture != NULL)
	{
		cvReleaseCapture(&capture);
	}

	return ModuleState::OKAY;
}

} // msv

