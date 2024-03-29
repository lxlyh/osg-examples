﻿#ifndef ADSORPTIONHANDLER_H
#define ADSORPTIONHANDLER_H

#include <osg/Node>
#include <osg/ref_ptr>
using namespace osg;

#include <osgGA/GUIEventHandler>
using namespace osgGA;

class AdsorptionHandler : public GUIEventHandler
{
    static const auto sizeMinimumOfNodePath = 4;
    static const auto indexFirstOfPicked = sizeMinimumOfNodePath - 1;
    static const auto KEY_S = 83;
public:
    AdsorptionHandler();
    virtual bool handle(const GUIEventAdapter& ea,GUIActionAdapter& aa);
    bool sPressed,lbPressed;
    Vec3 vertexPicked,vertexTargeted;
    ref_ptr<MatrixTransform> modelMatrixTransformPicked;
    Matrix modelMatrixPicked;
};

#endif // ADSORPTIONHANDLER_H
