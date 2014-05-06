#include <iostream>
#include <fstream>
#include <sstream>

// Include standard OpenCV headers
#include "opencv/cv.h"
#include "opencv/highgui.h"

// Include Boost headers for system time and threading
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/thread.hpp"
#include "boost/program_options.hpp"

using namespace std;
using namespace boost::posix_time;
using namespace cv;
namespace po = boost::program_options;


// Code for capture thread
void captureFunc(Mat *frame, VideoCapture *capture){
    //loop infinitely
    for(;;){
		//capture from webcame to Mat frame
      (*capture) >> (*frame);
  }
}


void satCallback(int saturation, void *userInput){
    
};


//main
int main (int argc, char *argv[])
{   
	//vars
    time_duration td, td1, visuElapsed;
    ptime nextFrameTimestamp, currentFrameTimestamp, initialLoopTimestamp, finalLoopTimestamp, visuClock;
    int delayFound = 0;
    int totalDelay= 0;
    
    bool isRecording = false;
    bool outputRAW = false;
    bool display=true;
    
    //options
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produces this help message")
    ("fps", po::value<int>(), "sets framerate of capture to arg")
    ("exposure", po::value<int>(), "sets exposure of the image, default is 700, minimum is 0")
    ("brightness", po::value<int>(), "sets brightness of the image, default is 0, maximum is unknown")
    ("gain", po::value<int>(), "sets gain of the image, default is 200, minimum is 0")
    ("nodisplay", "does not display a liveview during record (for slower computers or very high framerates)")
    ("raw", "sets output to RAW uncompressed movie, WARNING this creates very big files")
    ;
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    
    
    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }
    

    if (vm.count("nodisplay")) {
        cout << "Will not display a live feed during record." << "\n";
        display=false;
    }

    if (vm.count("fps")) {
        cout << "Framerate will be set to: " 
        << vm["fps"].as<int>() << ".\n";
    } else {
        assert(vm.count("fps") && "Framerate was not set. Use --fps N to set it !\n");
    }
    
    if (vm.count("raw")) {
        outputRAW=true;
    }

	// initialize capture on default source
	VideoCapture capture(0); // 0 is fisrt device, which just seem to work...

    if (vm.count("exposure")) {
        cout << "Exposure will be set to: " 
        << vm["exposure"].as<int>() << ".\n";
        capture.set(CV_CAP_PROP_EXPOSURE,vm["exposure"].as<int>());
    } else {
        capture.set(CV_CAP_PROP_EXPOSURE,700);
    }

    if (vm.count("brightness")) {
        cout << "Brightness will be set to: " 
        << vm["brightness"].as<int>() << ".\n";
        capture.set(CV_CAP_PROP_BRIGHTNESS,vm["brightness"].as<int>());
    } else {
        capture.set(CV_CAP_PROP_BRIGHTNESS,0);
    }
    
    if (vm.count("gain")) {
        cout << "Gain will be set to: " 
        << vm["gain"].as<int>() << ".\n";
        capture.set(CV_CAP_PROP_GAIN,vm["gain"].as<int>());
    } else {
        capture.set(CV_CAP_PROP_GAIN,200);
    }


    // for(int i=1; i<=18; i++)
    // {
    //     std::cout<<capture.get(i)<<endl;

    // }
// 
//0 CV_CAP_PROP_BRIGHTNESS Brightness of the image (only for cameras).
// 0 CV_CAP_PROP_SATURATION Saturation of the image (only for cameras). -> cannot be changed
// 0 CV_CAP_PROP_HUE Hue of the image (only for cameras). -> cannot be changed

//200 CV_CAP_PROP_GAIN Gain of the image (only for cameras).
//700 CV_CAP_PROP_EXPOSURE Exposure (only for cameras).

//179 CV_CAP_PROP_WHITE_BALANCE Currently unsupported -> doesnt exist !

	// set framerate to record and capture at
	int framerate = vm["fps"].as<int>();

	// Get the properties from the camera
	double width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	double height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);

	// print camera frame size
	cout << "Camera properties\n";
	cout << "width = " << width << endl <<"height = "<< height << endl;

	// Create a matrix to keep the retrieved frame
	Mat frame;
    Mat frameVisu;

//Find an available writing path.
    int i = 0;
    ostringstream fileName;
    while(true){
        ostringstream tempFileName;
        tempFileName<<"./capture"<<i<<".avi";
        ifstream f((tempFileName.str()).c_str());
        if (!f.good()) {
            f.close();
            // std::cout<<"BROKEN"<<std::endl;
            fileName<<tempFileName.str();
            break;
        } else {
            f.close();
            // cout<<i<<endl;
            i+=1;
        }
    }
    
    cout<<"Will write to file" << fileName.str()<<endl;
    
    // Create the video writer
    VideoWriter video(fileName.str(),(outputRAW?0:CV_FOURCC('M','J','P','G')), framerate, cvSize((int)width,(int)height)); // Replacing CC_FOURCC by 0 does uncompressed raw images, very big. // Lossless compression is CV_FOURCC('I','Y','U','V') // Nice comrpession is CV_FOURCC('D','I','V','X')
    
    // initialize initial timestamps
    nextFrameTimestamp = microsec_clock::local_time();
    currentFrameTimestamp = nextFrameTimestamp;
    td = (currentFrameTimestamp - nextFrameTimestamp);

    //Visualization clock
    visuClock = microsec_clock::local_time();

    int saturation=3;

    namedWindow( "Display window", WINDOW_AUTOSIZE );
    //namedWindow("Display window - controls");

    resizeWindow("Display window", width, height);

    // createTrackbar("Saturation", "Display window - controls", &saturation, 10, &satCallback);

    char keyPressed=0;



	// loop infinitely
    for(;;)
    {

        visuElapsed = (microsec_clock::local_time() - visuClock);
        if(visuElapsed.total_milliseconds() > 50.){
            visuClock=microsec_clock::local_time();
            capture >> frameVisu;
            if(!frameVisu.empty()) {
                imshow( "Display window", frameVisu );
                // cout<<"HERE"<<endl;
                keyPressed = waitKey(30);
            }
        }

        if( keyPressed == 27){
            break;
        }


        if( keyPressed == 'r'){
            isRecording = true;
            // destroyAllWindows();
            break;
        }

		//output will be in following format
		//[TIMESTAMP OF PREVIOUS FRAME] [TIMESTAMP OF NEW FRAME] [TIME DELAY OF WRITING]
        
    }
    int show = 0;
    int hasErrors=0;
    if(isRecording){

        // start thread to begin capture and populate Mat frame
        boost::thread captureThread(captureFunc, &frame, &capture);

        for(;;)
        {
             // wait for X microseconds until 1second/framerate time has passed after previous frame write
            while(td.total_microseconds() < 1000000/framerate){
            //determine current elapsed time
                currentFrameTimestamp = microsec_clock::local_time();
                td = (currentFrameTimestamp - nextFrameTimestamp);
            }

            //   determine time at start of write
            initialLoopTimestamp = microsec_clock::local_time();
            
            show++;
            

            if(display && show%( (int)(framerate/20) + hasErrors )==0){
                frameVisu = frame;
                if(!frameVisu.empty()) {
                    imshow( "Display window", frameVisu );
                    // cout<<"HERE"<<endl;
                    keyPressed = waitKey(1);
                }
            }


            // Save frame to video
            video << frame;
            
            //write previous and current frame timestamp to console
            //            cout << nextFrameTimestamp << " " << currentFrameTimestamp << " ";
            cout << "Wrote image "<<currentFrameTimestamp<<std::endl;

            // add 1second/framerate time for next loop pause
            nextFrameTimestamp = nextFrameTimestamp + microsec(1000000/framerate);
        
            // reset time_duration so while loop engages
            td = (currentFrameTimestamp - nextFrameTimestamp);
        
            //determine and print out delay in ms, should be less than 1000/FPS
            //occasionally, if delay is larger than said value, correction will occur
            //if delay is consistently larger than said value, then CPU is not powerful
            // enough to capture/decompress/record/compress that fast.
            finalLoopTimestamp = microsec_clock::local_time();
            td1 = (finalLoopTimestamp - initialLoopTimestamp);
            delayFound = td1.total_milliseconds();
            if(1000./framerate-delayFound<0) {
                cout<<"Frame time computing is too long, delay in ms is: "<<delayFound<<" while framerate is: "<<framerate<<" and time between two frames should be: "<<1000./framerate<<" ms maximum. Please consider using --nodisplay option."<<endl;
                hasErrors++;
                show=0;
            }

        }
    }

	// Exit
    return 0;
}
