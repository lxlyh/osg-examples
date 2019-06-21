#include "adsorptionhandler.h"

#include <osgUtil/LineSegmentIntersector>
using namespace osgUtil;

#include <osgViewer/View>
using namespace osgViewer;

#include <osg/MatrixTransform>
#include <osg/Vec3>
using namespace osg;

#include <iostream>
using namespace std;

std::ostream& operator<<(std::ostream& out,Vec3 v3) {
    out << "x:" << v3.x() << " y:" << v3.y() << " z:" << v3.z();
    return out;
}

AdsorptionHandler::AdsorptionHandler()
{}

bool AdsorptionHandler::handle(const GUIEventAdapter& ea,GUIActionAdapter& aa) {
    auto view = dynamic_cast<osgViewer::View*>(&aa);

    switch(ea.getEventType()) {
    case GUIEventAdapter::KEYDOWN:
        if(GUIEventAdapter::KEY_S == ea.getKey()) sPressed = true;
    return true;
    case GUIEventAdapter::KEYUP:
        if(GUIEventAdapter::KEY_S == ea.getKey()) sPressed = false;
    return true;
    case GUIEventAdapter::MOVE: {
        if(!sPressed) break;
        LineSegmentIntersector::Intersections lis;
        if(!view->computeIntersections(ea.getX(),ea.getY(),lis)) break;
        auto hit  = *lis.begin();
        if(3 >= hit.nodePath.size()) return false;

        auto point = hit.getWorldIntersectPoint();
        auto score = DBL_MAX;

        for(auto p : hit.nodePath) {
            cout << p << ' ' << p->className() << ' ' << p->getName() << endl;
        }

        auto matrix = dynamic_cast<MatrixTransform*>(hit.nodePath.at(3));
        auto model = dynamic_cast<Drawable*>(hit.nodePath.at(4));
        auto geometra = dynamic_cast<Geometry*>(hit.drawable.get());
        auto vertexs =  dynamic_cast<Vec3Array*>(geometra->getVertexArray());
        Vec3 center;
        cout << "vertexs:" << endl;

        for(auto v : *vertexs) {
            auto mv = (v * matrix->getMatrix());
            auto sample = point - mv;
            auto difference = sample.normalize();

            if(difference < score) {
                score = difference;
                center = mv;
            }
        }

        cout << "point:" << point << endl;
        cout << "center:" << center << endl;

//        Vec3 pickedObjectCenter;
//        for(auto wm : hit.drawable->getWorldMatrices()) {
//            auto sample = point - wm.getTrans();
//            auto difference = sample.normalize();

//            if(difference < score) {
//                score = difference;
//                pickedObjectCenter = wm.getTrans();
//            }
//        }
//        auto hoverObjectCenter = pickedObjectCenter + hit.drawable->getBound().center();
//        auto hoverObject = hit.drawable;
    }
    return true;
    case GUIEventAdapter::PUSH:
    if(!sPressed) break;

    return true;
    case GUIEventAdapter::RELEASE:
    if(!sPressed) break;
    return true;
    }
    return false;
}
