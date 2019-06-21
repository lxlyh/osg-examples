#include "threedmaxmanipulator.h"
#include "common.h"
#include <osg/BoundsChecking>
using namespace osg;

#include <osgUtil/LineSegmentIntersector>
using namespace osgUtil;

#include <osgViewer/View>
using namespace osgViewer;

int ThreeDMaxManipulator::_minimumDistanceFlagIndex = allocateRelativeFlag();

ThreeDMaxManipulator::ThreeDMaxManipulator(MatrixTransform* world) : inherited( DEFAULT_SETTINGS ) {
    _minimumDistance = 0.05;
    _wheelZoomFactor = 0.1;
    midPressed = altPressed= false;

    if( _flags & SET_CENTER_ON_WHEEL_FORWARD_MOVEMENT )
        setAnimationTime( 0.2 );

    this->world = world;
    initWorldMatrix = world->getMatrix();
}

ThreeDMaxManipulator::ThreeDMaxManipulator(int flags) : inherited( flags ) {
    _minimumDistance = 0.05;
    _wheelZoomFactor = 0.1;

    if( _flags & SET_CENTER_ON_WHEEL_FORWARD_MOVEMENT )
        setAnimationTime( 0.2 );
}

ThreeDMaxManipulator::ThreeDMaxManipulator( const ThreeDMaxManipulator& om, const osg::CopyOp& copyOp ) :
    Callback(om, copyOp),
    inherited( om, copyOp ),
    _center( om._center ),
    _rotation( om._rotation ),
    _distance( om._distance ),
    _trackballSize( om._trackballSize ),
    _wheelZoomFactor( om._wheelZoomFactor ),
    _minimumDistance( om._minimumDistance )
{}

void ThreeDMaxManipulator::setByMatrix(const osg::Matrixd& matrix) {
    _center = osg::Vec3d( 0., 0., -_distance ) * matrix;
    _rotation = matrix.getRotate();

    // fix current rotation
    if( getVerticalAxisFixed() )
        fixVerticalAxis( _center, _rotation, true );
}

void ThreeDMaxManipulator::setByInverseMatrix(const osg::Matrixd& matrix) {
    setByMatrix( osg::Matrixd::inverse( matrix ) );
}

osg::Matrixd ThreeDMaxManipulator::getMatrix() const
{
    return osg::Matrixd::translate( 0., 0., _distance ) *
           osg::Matrixd::rotate( _rotation ) *
           osg::Matrixd::translate( _center );
}

osg::Matrixd ThreeDMaxManipulator::getInverseMatrix() const
{
    return osg::Matrixd::translate( -_center ) *
           osg::Matrixd::rotate( _rotation.inverse() ) *
           osg::Matrixd::translate( 0.0, 0.0, -_distance );
}

// doc in parent
void ThreeDMaxManipulator::setTransformation( const osg::Vec3d& eye, const osg::Quat& rotation )
{
    _center = eye + rotation * osg::Vec3d( 0., 0., -_distance );
    _rotation = rotation;

    // fix current rotation
    if( getVerticalAxisFixed() )
        fixVerticalAxis( _center, _rotation, true );
}

// doc in parent
void ThreeDMaxManipulator::getTransformation( osg::Vec3d& eye, osg::Quat& rotation ) const
{
    eye = _center - _rotation * osg::Vec3d( 0., 0., -_distance );
    rotation = _rotation;
}

// doc in parent
void ThreeDMaxManipulator::setTransformation( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up )
{
    Vec3d lv( center - eye );

    Vec3d f( lv );
    f.normalize();
    Vec3d s( f^up );
    s.normalize();
    Vec3d u( s^f );
    u.normalize();

    osg::Matrixd rotation_matrix( s[0], u[0], -f[0], 0.0f,
                            s[1], u[1], -f[1], 0.0f,
                            s[2], u[2], -f[2], 0.0f,
                            0.0f, 0.0f,  0.0f, 1.0f );

    _center = center;
    _distance = lv.length();
    _rotation = rotation_matrix.getRotate().inverse();

    // fix current rotation
    if( getVerticalAxisFixed() )
        fixVerticalAxis( _center, _rotation, true );
}


// doc in parent
void ThreeDMaxManipulator::getTransformation( osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up ) const
{
    center = _center;
    eye = _center + _rotation * osg::Vec3d( 0., 0., _distance );
    up = _rotation * osg::Vec3d( 0., 1., 0. );
}

bool ThreeDMaxManipulator::handleMouseWheel( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
    osgGA::GUIEventAdapter::ScrollingMotion sm = ea.getScrollingMotion();

    // handle centering
    if( _flags & SET_CENTER_ON_WHEEL_FORWARD_MOVEMENT )
    {

        if( ((sm == GUIEventAdapter::SCROLL_DOWN && _wheelZoomFactor > 0.)) ||
            ((sm == GUIEventAdapter::SCROLL_UP   && _wheelZoomFactor < 0.)) )
        {

            if( getAnimationTime() <= 0. )
            {
                // center by mouse intersection (no animation)
                setCenterByMousePointerIntersection( ea, us );
            }
            else
            {
                // start new animation only if there is no animation in progress
                if( !isAnimating() )
                    startAnimationByMousePointerIntersection( ea, us );
            }

        }
    }

    switch( sm )
    {
        // mouse scroll up event
        case GUIEventAdapter::SCROLL_UP:
        {
            // perform zoom
            zoomModel( _wheelZoomFactor, true );
            us.requestRedraw();
            us.requestContinuousUpdate( isAnimating() || _thrown );
            return true;
        }

        // mouse scroll down event
        case GUIEventAdapter::SCROLL_DOWN:
        {
            // perform zoom
            zoomModel( -_wheelZoomFactor, true );
            us.requestRedraw();
            us.requestContinuousUpdate( isAnimating() || _thrown );
            return true;
        }

        // unhandled mouse scrolling motion
        default:
            return false;
   }
}

void ThreeDMaxManipulator::zoomModel( const float dy, bool pushForwardIfNeeded )
{
    // scale
    float scale = 1.0f + dy;

    // minimum distance
    float minDist = _minimumDistance;
    if( getRelativeFlag( _minimumDistanceFlagIndex ) )
        minDist *= _modelSize;

    if( _distance*scale > minDist )
    {
        // regular zoom
        _distance *= scale;
    }
    else
    {
        if( pushForwardIfNeeded )
        {
            // push the camera forward
            float scale = -_distance;
            Matrixd rotation_matrix( _rotation );
            Vec3d dv = (Vec3d( 0.0f, 0.0f, -1.0f ) * rotation_matrix) * (dy * scale);
            _center += dv;
        }
        else
        {
            // set distance on its minimum value
            _distance = minDist;
        }
    }
}

// doc in parent
bool ThreeDMaxManipulator::performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
    // pan model
    float scale = -0.3f * _distance * getThrowScale( eventTimeDelta );
    panModel( dx*scale, dy*scale );
    return true;
}

void ThreeDMaxManipulator::panModel( const float dx, const float dy, const float dz )
{
    Matrix rotation_matrix;
    rotation_matrix.makeRotate( _rotation );
    Vec3d dv( dx, dy, dz );
    _center += dv * rotation_matrix;
}

void ThreeDMaxManipulator::setCenter( const Vec3d& center )
{
    _center = center;
}

const Vec3d& ThreeDMaxManipulator::getCenter() const
{
    return _center;
}

void ThreeDMaxManipulator::setMinimumDistance( const double& minimumDistance, bool relativeToModelSize )
{
    _minimumDistance = minimumDistance;
    setRelativeFlag( _minimumDistanceFlagIndex, relativeToModelSize );
}

double ThreeDMaxManipulator::getMinimumDistance( bool *relativeToModelSize ) const
{
    if( relativeToModelSize )
        *relativeToModelSize = getRelativeFlag( _minimumDistanceFlagIndex );

    return _minimumDistance;
}

void ThreeDMaxManipulator::setDistance( double distance )
{
    _distance = distance;
}

double ThreeDMaxManipulator::getDistance() const
{
    return _distance;
}

#include <iostream>
bool ThreeDMaxManipulator::handleKeyDown( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
    switch(ea.getKey()){
    case KEY_ALT:
        altPressed = true;
        return true;
    case KEY_Z:
        world->setMatrix(initWorldMatrix);
        return true;
    }

    return inherited::handleKeyDown(ea,us);
}


/// Handles GUIEventAdapter::KEYUP event.
bool ThreeDMaxManipulator::handleKeyUp( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
    switch(ea.getKey()){
    case KEY_ALT:
        altPressed = false;
        midPressed = false;
        return true;
    }

    return inherited::handleKeyDown(ea,us);
}

/// Handles GUIEventAdapter::PUSH event.
bool ThreeDMaxManipulator::handleMousePush( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
    auto view = dynamic_cast<osgViewer::View*>(&us);
    LineSegmentIntersector::Intersections lis;
    if(altPressed && GUIEventAdapter::MIDDLE_MOUSE_BUTTON == ea.getButtonMask()) {
        if(!view->computeIntersections(ea.getX(),ea.getY(),lis)) return false;
        centerX = mouseX = ea.getX();
        centerY = mouseY = ea.getY();
        centerRotated = lis.begin()->getWorldIntersectPoint();
        std::cout << centerRotated << std::endl;
        midPressed = true;
        //centerRotated = Vec3(50,0,0);
        return true;
    }

    return inherited::handleMousePush(ea,us);
}

bool ThreeDMaxManipulator::handleMouseRelease( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
    auto view = dynamic_cast<osgViewer::View*>(&us);
    auto tdmm = dynamic_cast<ThreeDMaxManipulator*>(view->getCameraManipulator());
    if(GUIEventAdapter::MIDDLE_MOUSE_BUTTON == ea.getButtonMask()) {
        centerRotated = tdmm->getCenter();
        midPressed = false;
        return true;
    }

    return inherited::handleMousePush(ea,us);
}


/// Handles GUIEventAdapter::DRAG event.
bool ThreeDMaxManipulator::handleMouseDrag( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
    auto view = dynamic_cast<osgViewer::View*>(&us);
    LineSegmentIntersector::Intersections lis;

    if(altPressed && GUIEventAdapter::MIDDLE_MOUSE_BUTTON == ea.getButtonMask()) {
        if(!midPressed) handleMousePush( ea, us );

        auto wmt = world->getMatrix().getTrans();
        std::cout << "wmt:" << wmt << std::endl;
        world->setMatrix(
                    world->getMatrix() *
                    Matrix::rotate(Quat(DegreesToRadians(ea.getX() - mouseX),Z_AXIS)) *
                    Matrix::rotate(Quat(DegreesToRadians(mouseY - ea.getY()),X_AXIS))
//                    Matrix::translate(-wmt) *
//                    Matrix::translate(-centerRotated) *

//                    Matrix::translate(centerRotated) *
//                    Matrix::translate(wmt));
                    );

        wmt = world->getMatrix().getTrans();
//        world->setMatrix(
//                    Matrix::translate(-mt) *
//                    Matrix::rotate(Quat(DegreesToRadians(mouseY - ea.getY()),X_AXIS)) *
//                    Matrix::translate(mt));


        mouseX = ea.getX();
        mouseY = ea.getY();
        return true;
    }

    return inherited::handleMouseDrag(ea,us);
}

