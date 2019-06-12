#ifndef PLACEDEVENTHANDLER_H
#define PLACEDEVENTHANDLER_H

#include <iostream>

using namespace std;


#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/OrbitManipulator>

using namespace osgGA;

#include <osg/Vec3>
#include <osg/Material>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>

using namespace osg;

#include <osgViewer/View>

using namespace osgViewer;

#include <osgUtil/LineSegmentIntersector>

using namespace osgUtil;

class PlacedEventHandler : public GUIEventHandler
{
public:
    PlacedEventHandler();
protected:
    virtual ~PlacedEventHandler();

    virtual bool handle(const GUIEventAdapter& ea,GUIActionAdapter& aa);
};

#endif // PLACEDEVENTHANDLER_H
