#include <jni.h>

#include <raspicam_cv.h>

#include <iostream>

JNIEXPORT jlong JNICALL
Java_org_photonvision_picamera_PiCamera_createPicamCV
  (JNIEnv* env, jclass)
{
    cout << "Creating a CV Picamera!";

    raspicam::RaspiCam_Cv Camera;
    Camera.set( CV_CAP_PROP_FORMAT, CV_8UC1 );

    cout<<"Opening Camera..."<<endl;
	if (!Camera.open()) {
        cerr << "Error opening the camera" << endl;
        return -1;
    }

    return reinterpret_cast<jlong>(Camera);
}

JNIEXPORT void JNICALL
Java_org_photonvision_picamera_PiCamera_destroyPicamCV
  (JNIEnv* env, jclass, jlong handle)
{
    cout << "Destroying CV Picamera!";
    delete reinterpret_cast<CppObject *>(handle);
}

JNIEXPORT jbool JNICALL
Java_org_photonvision_picamera_PiCamera_grabSinkFrame
  (JNIEnv* env, jclass, jlong cameraNativeObject, jlong imageNativeObj)
{
  try {
    raspicam::RaspiCam_Cv camera = *((raspicam::RaspiCam_Cv*)cameraNativeObject)
    cv::Mat& image = *((cv::Mat*)imageNativeObj);
    
    auto success = Camera.grab();
    if(success) {
        Camera.retrieve(image);
    }

    return success;
  } catch (const std::exception& e) {
    ThrowJavaException(env, &e);
    return false;
  } catch (...) {
    ThrowJavaException(env, 0);
    return false;
  }
}