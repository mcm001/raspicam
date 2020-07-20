#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <sys/timeb.h>
#include "raspicam.h"

using namespace std;

//timer functions
#include <sys/time.h>
#include <unistd.h>
class Timer{
    private:
    struct timeval _start, _end;

public:
    Timer(){}
    void start(){
        gettimeofday(&_start, NULL);
    }
    void end(){
        gettimeofday(&_end, NULL);
    }
    double getSecs(){
    return double(((_end.tv_sec  - _start.tv_sec) * 1000 + (_end.tv_usec - _start.tv_usec)/1000.0) + 0.5)/1000.;
    }

};

int main ( int argc,char **argv ) {

    raspicam::RaspiCam camera;
    camera.setWidth(1920);
    camera.setHeight(1080);
    camera.setFormat(raspicam::RASPICAM_FORMAT_YUV420);
    size_t nFramesCaptured=100;

    if ( !camera.open() ) {
        cerr<<"Error opening camera"<<endl;
        return -1;
    }

    cout<<"Connected to camera ="<<camera.getId() <<" bufs="<<camera.getImageBufferSize( )<<endl;
    unsigned char* data=new unsigned char[camera.getImageBufferSize()];
    Timer timer;

    cout<<"Capturing...."<<endl;
    size_t i=0;
    timer.start();
    do {
        camera.grab();
        camera.retrieve(data);

        cv::Mat yuvImage(1920, 1080, CV_8UC1, &data[0]);
        cv::Mat bgr;
        cv::cvtColor(yuvImage, bgr, cv::COLOR_YUV2BGR);
    } while (++i<nFramesCaptured || nFramesCaptured==0); //stops when nFrames captured or at infinity lpif nFramesCaptured<0

    timer.end();

    cerr<< timer.getSecs()<< " seconds for "<< nFramesCaptured << "  frames : FPS " 
        << ( ( float ) ( nFramesCaptured ) / timer.getSecs() ) <<endl;
}