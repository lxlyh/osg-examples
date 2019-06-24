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

AdsorptionHandler::AdsorptionHandler() : sPressed(false),lbPressed(false)
{}

bool AdsorptionHandler::handle(const GUIEventAdapter& ea,GUIActionAdapter& aa) {
    auto view = dynamic_cast<osgViewer::View*>(&aa);
    LineSegmentIntersector::Intersections lis;

    switch(ea.getEventType()) {
    case GUIEventAdapter::KEYDOWN:
        if(GUIEventAdapter::KEY_S == ea.getKey()) sPressed = true;
    return true;
    case GUIEventAdapter::KEYUP:
        if(GUIEventAdapter::KEY_S == ea.getKey()) sPressed = false;
    return true;
    case GUIEventAdapter::MOVE: {
        if(!sPressed || lbPressed) break;
        if(!view->computeIntersections(ea.getX(),ea.getY(),lis)) break;
        auto hit  = *lis.begin();

        auto point = hit.getWorldIntersectPoint();
        auto score = DBL_MAX;

        cout << "point:" << point << endl;

        modelMatrixTransformPicked = dynamic_cast<MatrixTransform*>(hit.nodePath[3]);
        modelMatrixPicked = modelMatrixTransformPicked->getMatrix();
        auto geometra = dynamic_cast<Geometry*>(hit.drawable.get());
        auto vertexs =  dynamic_cast<Vec3Array*>(geometra->getVertexArray());

        for(auto v : *vertexs) {
            for(auto wm : hit.drawable->getWorldMatrices()) {
                auto mv = (v * wm);
                auto sample = point - mv;
                auto difference = sample.normalize();

                if(difference < score) {
                    score = difference;
                    vertexPicked = mv;
                }
            }
        }
    }
    return true;
    case GUIEventAdapter::PUSH:
        if(!sPressed) break;
        lbPressed = true;
    return true;
    case GUIEventAdapter::DRAG:
        if(!sPressed) break;
        if(!view->computeIntersections(ea.getX(),ea.getY(),lis)) {
            cout << "compute faild:" << endl;
            modelMatrixTransformPicked->setMatrix(modelMatrixPicked);
        } else {
            auto hit  = *lis.begin();
            auto point = hit.getWorldIntersectPoint();
            auto score = DBL_MAX;
            cout << "point:" << point << endl;

            if(modelMatrixTransformPicked == dynamic_cast<MatrixTransform*>(hit.nodePath[3])) {
                return true;
            }

            auto geometra = dynamic_cast<Geometry*>(hit.drawable.get());
            auto vertexs =  dynamic_cast<Vec3Array*>(geometra->getVertexArray());

            for(auto v : *vertexs) {
                for(auto wm : hit.drawable->getWorldMatrices()) {
                    auto mv = (v * wm);
                    auto sample = point - mv;
                    auto difference = sample.normalize();

                    if(difference < score) {
                        score = difference;
                        vertexTargeted = mv;
                    }
                }
            }

            Vec3 trans = -(vertexPicked - vertexTargeted);
            cout << "trans:" << trans << endl;
            modelMatrixTransformPicked->setMatrix(modelMatrixPicked * Matrix::translate(trans));
        }
    return true;
    case GUIEventAdapter::RELEASE:
    if(!sPressed) break;
    lbPressed = false;
    modelMatrixPicked = modelMatrixTransformPicked->getMatrix();
    return true;
    }
    return false;
}
