#ifndef MYVIEWER_H
#define MYVIEWER_H

#include <osgViewer/Viewer>
#include <osg/FrameStamp>
#include <QDebug>
#include <functional>

using namespace osgViewer;
using namespace osg;

class MyViewer : public Viewer
{

public:
    std::function<void(FrameStamp*)> f;
    MyViewer();
    virtual ~MyViewer();

    virtual void advance(double simulationTime=USE_REFERENCE_TIME);
    FrameStamp* getFrameStamp();
};

#endif // MYVIEWER_H
