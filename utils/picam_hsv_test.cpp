#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <sys/timeb.h>
#include "raspicam.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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
    camera.setWidth(640);
    camera.setHeight(480);
    camera.setBrightness(50);
    camera.setSharpness (0);
    camera.setContrast(0);
    camera.setSaturation(0);
    camera.setShutterSpeed(0);
    camera.setISO (400);
    camera.setExposureCompensation(0);
    camera.setFormat(raspicam::RASPICAM_FORMAT_BGR);
    camera.setFrameRate(90);
    size_t nFramesCaptured=100;

    if (!camera.open()) {
        cerr<<"Error opening camera"<<endl;
        return -1;
    }

    cout<<"Connected to camera ="<<camera.getId() <<" bufs="<<camera.getImageBufferSize( )<<endl;
    unsigned char* data=new unsigned char[camera.getImageBufferSize()];
    Timer timer;

    cout<<"Capturing...."<<endl;
    size_t i=0;
    timer.start();
    cv::Mat hsv, frame_threshold;
    do {
        camera.grab();
        camera.retrieve(data);

        cv::Mat bgr(1080, 1920, CV_8UC3, &data[0]);
        
        // cv::cvtColor(yuvImage, bgr, cv::COLOR_YUV420p2BGR);
        cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);
        cv::inRange(hsv, cv::Scalar(100, 100, 100), cv::Scalar(105, 200, 255), frame_threshold);

        bgr.release();
        hsv.release();
        frame_threshold.release();
    } while (++i<nFramesCaptured || nFramesCaptured==0); //stops when nFrames captured or at infinity lpif nFramesCaptured<0

    timer.end();

    cerr<< timer.getSecs()<< " seconds for "<< nFramesCaptured << "  frames : FPS " 
        << ( ( float ) ( nFramesCaptured ) / timer.getSecs() ) <<endl;
}


// // https://stackoverflow.com/questions/22855510/yuv420-to-rgb-color-conversion-error
// vec3 yuv420_to_rgb(vec3 c) {
//     float Y = c.x;
//     float U = c.y;
//     float V = c.z;
    
//     float R = Y + 1.402 * (V - 128.0);
//     float G = Y - 0.344 * (U - 128.0) - 0.714 * (V - 128.0);
//     float B = Y + 1.772 * (U - 128.0);


//     if (R < 0.0){ R = 0.0; } if (G < 0.0){ G = 0.0; } if (B < 0.0){ B = 0.0; }
//     if (R > 255.0 ){ R = 255.0; } if (G > 255.0) { G = 255.0; } if (B > 255.0) { B = 255.0; }
    
//     return vec3(R, G, B);
// }

// // http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
// vec3 rgb2hsv(vec3 c) {
// 	vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
//     vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
//     vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

//     float d = q.x - min(q.w, q.y);
//     float e = 1.0e-10;
//     return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
// }

// vec3 inRange(vec3 hsv, vec3 bot, vec3 top) {
//     bvec3 botBool = greaterThanEqual(hsv, bot);
//     bvec3 topBool = lessThanEqual(hsv, top);
//     if (all(botBool) && all(topBool)) {
//     	return vec3(255, 255, 255);
//     } else {
//     	return vec3(0, 0, 0);
//     }
// }

// void mainImage( out vec4 fragColor, in vec2 fragCoord )
// {
//     // Normalized pixel coordinates (from 0 to 1)
//     vec2 uv = fragCoord/iResolution.xy;

//     // Get texture colors
//     vec3 col = texture(iChannel0, uv).rgb;
    
//     // Output to screen
//     fragColor = vec4(inRange(rgb2hsv(col), vec3(0.4, 0.8, 0.75), vec3(0.45, 1.0, 1.0)), 1.0);
// }