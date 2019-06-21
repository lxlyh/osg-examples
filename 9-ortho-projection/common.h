#ifndef COMMON_H
#define COMMON_H

#include <osg/LineWidth>
#include <osg/PositionAttitudeTransform>
#include <osg/TexGen>
#include <osg/TexMat>
#include <osg/Depth>
#include <osg/StateSet>
#include <osg/ShapeDrawable>
#include <osg/StateAttribute>
#include <osg/TextureCubeMap>
#include <osg/Material>
#include <osg/BlendFunc>

using namespace osg;

#include <osgDB/ReadFile>

#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

std::ostream& operator<<(std::ostream& out,Vec3 v3);
ref_ptr<Geode> makeAxis(float radius = 50.0f);
ref_ptr<Geode> makeGround(float radius = 50.0f);
ref_ptr<Node> makeBase(float radius = 50.0f);
osg::Node* createSkyBox();
#endif // COMMON_H
