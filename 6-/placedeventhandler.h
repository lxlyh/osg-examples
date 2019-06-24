#ifndef PLACEDEVENTHANDLER_H
#define PLACEDEVENTHANDLER_H

#include <iostream>

using namespace std;


#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>

using namespace osgGA;

#include <osg/Vec3>
#include <osg/Material>

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
    Vec3 screen2World(osgViewer::View* view,float x, float y);
};

#endif // PLACEDEVENTHANDLER_H
