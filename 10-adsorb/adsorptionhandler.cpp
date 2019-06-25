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

const int sizeMinimumOfNodePath = 4;
const int indexFirstOfPicked = sizeMinimumOfNodePath - 1;

AdsorptionHandler::AdsorptionHandler() : sPressed(false),lbPressed(false)
{}

bool AdsorptionHandler::handle(const GUIEventAdapter& ea,GUIActionAdapter& aa) {
    auto view = dynamic_cast<osgViewer::View*>(&aa);
    LineSegmentIntersector::Intersections lis;

    switch(ea.getEventType()) {
    case GUIEventAdapter::MOVE: {
        if(lbPressed) break;
        if(!view->computeIntersections(ea.getX(),ea.getY(),lis)) break;
        LineSegmentIntersector::Intersection hit;
        for(auto li : lis) {
            if(sizeMinimumOfNodePath <= li.nodePath.size()) {
                hit = li;
                break;
            }
        }

        if(sizeMinimumOfNodePath > hit.nodePath.size()) break;
        auto point = hit.getWorldIntersectPoint();
        auto score = DBL_MAX;

        modelMatrixTransformPicked = dynamic_cast<MatrixTransform*>(hit.nodePath[indexFirstOfPicked]);
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
        if(GUIEventAdapter::LEFT_MOUSE_BUTTON != ea.getButtonMask()) break;
        if(!modelMatrixTransformPicked.valid()) break;
        lbPressed = true;
    return true;
    case GUIEventAdapter::DRAG:
        if(GUIEventAdapter::LEFT_MOUSE_BUTTON != ea.getButtonMask()) break;
        if(!lbPressed) break;
        if(!view->computeIntersections(ea.getX(),ea.getY(),lis)) {
            modelMatrixTransformPicked->setMatrix(modelMatrixPicked);
        } else {
            auto hit  = *lis.begin();
            if(sizeMinimumOfNodePath > hit.nodePath.size()) break;
            auto point = hit.getWorldIntersectPoint();
            auto score = DBL_MAX;

            if(modelMatrixTransformPicked == dynamic_cast<MatrixTransform*>(hit.nodePath[3])) {
                modelMatrixTransformPicked->setMatrix(modelMatrixPicked);
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
            modelMatrixTransformPicked->setMatrix(modelMatrixPicked * Matrix::translate(trans));
        }
    return true;
    case GUIEventAdapter::RELEASE:
        if(GUIEventAdapter::LEFT_MOUSE_BUTTON != ea.getButton()) break;
        if(!modelMatrixTransformPicked.valid()) break;
        lbPressed = false;
        modelMatrixPicked = modelMatrixTransformPicked->getMatrix();
    return true;
    }
    return false;
}
