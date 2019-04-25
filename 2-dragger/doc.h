#ifndef DOC_H
#define DOC_H

#include <osg/Group>
#include <osgManipulator/Selection>
#include <osgManipulator/TabBoxDragger>
#include <osgManipulator/TabBoxTrackballDragger>
#include <osgManipulator/TranslatePlaneDragger>

#include <osgManipulator/RotateCylinderDragger>
#include <osgManipulator/RotateSphereDragger>

#include <osgManipulator/TrackballDragger>
#include <osgManipulator/ScaleAxisDragger>
#include <osgManipulator/Scale1DDragger>
#include <osgManipulator/Scale2DDragger>

#include <osgManipulator/TranslateAxisDragger>


using namespace osg;
using namespace osgManipulator;

extern Selection* globelSections;
extern TrackballDragger* globelDragger;
extern Group* globelScene;

#endif // DOC_H
