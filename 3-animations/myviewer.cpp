#include "myviewer.h"

MyViewer::MyViewer() : Viewer ()
{
    this->_frameStamp = new FrameStamp();
    _frameStamp->setFrameNumber(0);
    _frameStamp->setReferenceTime(0);
    _frameStamp->setSimulationTime(0);
}

MyViewer::~MyViewer() {

}

FrameStamp* MyViewer::getFrameStamp(){
    return _frameStamp;
}

void MyViewer::advance(double simulationTime)
{
    Viewer::advance(simulationTime);

    try{
        f(_frameStamp);
    }catch(std::bad_function_call& e){

    }
}
