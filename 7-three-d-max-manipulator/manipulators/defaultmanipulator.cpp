#include "defaultmanipulator.h"
#include "common.h"
#include <osg/BoundsChecking>
using namespace osg;

#include <osgUtil/LineSegmentIntersector>
using namespace osgUtil;

#include <osgViewer/View>
using namespace osgViewer;

int DefaultManipulator::_minimumDistanceFlagIndex = allocateRelativeFlag();

DefaultManipulator::DefaultManipulator(int flags) : inherited( flags ) {
    _minimumDistance = 0.05;
    _wheelZoomFactor = 0.1;
    altPressed = false;

    if( _flags & SET_CENTER_ON_WHEEL_FORWARD_MOVEMENT )
        setAnimationTime( 0.2 );
}

DefaultManipulator::DefaultManipulator( const DefaultManipulator& om, const osg::CopyOp& copyOp ) :
    Callback(om, copyOp),
    inherited( om, copyOp ),
    _center( om._center ),
    _rotation( om._rotation ),
    _distance( om._distance ),
    _trackballSize( om._trackballSize ),
    _wheelZoomFactor( om._wheelZoomFactor ),
    _minimumDistance( om._minimumDistance )
{}

void DefaultManipulator::setByMatrix(const osg::Matrixd& matrix) {
    _center = osg::Vec3d( 0., 0., -_distance ) * matrix;
    _rotation = matrix.getRotate();
}

void DefaultManipulator::setByInverseMatrix(const osg::Matrixd& matrix) {
    setByMatrix( osg::Matrixd::inverse( matrix ) );
}

osg::Matrixd DefaultManipulator::getMatrix() const
{
    return osg::Matrixd::translate( 0., 0., _distance ) *
           osg::Matrixd::rotate( _rotation ) *
           osg::Matrixd::translate( _center );
}

osg::Matrixd DefaultManipulator::getInverseMatrix() const
{
    return osg::Matrixd::translate( -_center ) *
           osg::Matrixd::rotate( _rotation.inverse() ) *
           osg::Matrixd::translate( 0.0, 0.0, -_distance );
}

// doc in parent
void DefaultManipulator::setTransformation( const osg::Vec3d& eye, const osg::Quat& rotation )
{
    _center = eye + rotation * osg::Vec3d( 0., 0., -_distance );
    _rotation = rotation;
}

// doc in parent
void DefaultManipulator::getTransformation( osg::Vec3d& eye, osg::Quat& rotation ) const
{
    eye = _center - _rotation * osg::Vec3d( 0., 0., -_distance );
    rotation = _rotation;
}

// doc in parent
void DefaultManipulator::setTransformation( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up )
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
}


// doc in parent
void DefaultManipulator::getTransformation( osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up ) const
{
    center = _center;
    eye = _center + _rotation * osg::Vec3d( 0., 0., _distance );
    up = _rotation * osg::Vec3d( 0., 1.,0.);
}

bool DefaultManipulator::handleMouseWheel( const GUIEventAdapter& ea, GUIActionAdapter& us )
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

void DefaultManipulator::zoomModel( const float dy, bool pushForwardIfNeeded )
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
bool DefaultManipulator::performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
    if(altPressed) {
        // rotate camera
        if( getVerticalAxisFixed() )
            rotateWithFixedVertical( dx, dy );
    } else {
        // pan model
        float scale = -0.3f * _distance * getThrowScale( eventTimeDelta );
        panModel( dx*scale, dy*scale );
    }
    return true;
}

void DefaultManipulator::rotateWithFixedVertical( const float dx, const float dy )
{
    CoordinateFrame coordinateFrame = getCoordinateFrame( _center );
    Vec3d localUp = getUpVector( coordinateFrame );
    localUp.set(localUp.x(),localUp.z(),localUp.y());
    rotateYawPitch( _rotation, dx, dy,localUp);
}

void DefaultManipulator::panModel( const float dx, const float dy, const float dz )
{
    Matrix rotation_matrix;
    rotation_matrix.makeRotate( _rotation );
    Vec3d dv( dx, dy, dz );
    _center += dv * rotation_matrix;
}

void DefaultManipulator::setCenter( const Vec3d& center )
{
    _center = center;
}

const Vec3d& DefaultManipulator::getCenter() const
{
    return _center;
}

void DefaultManipulator::setMinimumDistance( const double& minimumDistance, bool relativeToModelSize )
{
    _minimumDistance = minimumDistance;
    setRelativeFlag( _minimumDistanceFlagIndex, relativeToModelSize );
}

double DefaultManipulator::getMinimumDistance( bool *relativeToModelSize ) const
{
    if( relativeToModelSize )
        *relativeToModelSize = getRelativeFlag( _minimumDistanceFlagIndex );

    return _minimumDistance;
}

void DefaultManipulator::setDistance( double distance )
{
    _distance = distance;
}

double DefaultManipulator::getDistance() const
{
    return _distance;
}

bool DefaultManipulator::handleKeyDown( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
    auto view = dynamic_cast<osgViewer::View*>(&us);
    LineSegmentIntersector::Intersections lis;
    Vec3d eye;
    Quat rotation;

    switch(ea.getKey()) {
    case KEY_ALT:
        altPressed = true;
        if(!view->computeIntersections(ea.getX(),ea.getY(),lis)) break;
        getTransformation(eye,rotation);
        _center = lis.begin()->getWorldIntersectPoint();
        _distance = sqrt(pow(eye.x() - _center.x(),2) + pow(eye.y() - _center.y(),2) + pow(eye.z() - _center.z(),2));
        return true;
    case KEY_Z:
        return true;
    }

    return inherited::handleKeyDown(ea,us);
}

/// Handles GUIEventAdapter::KEYUP event.
bool DefaultManipulator::handleKeyUp( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
    switch(ea.getKey()) {
    case KEY_ALT:
        altPressed = false;
        return true;
    }

    return inherited::handleKeyDown(ea,us);
}
