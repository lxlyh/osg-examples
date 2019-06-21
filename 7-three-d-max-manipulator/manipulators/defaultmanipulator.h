#ifndef DEFAULTMANIPULATOR_H
#define DEFAULTMANIPULATOR_H

#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
using namespace osg;

#include <osgGA/StandardManipulator>
using namespace osgGA;

namespace osgGA {
class DefaultManipulator : public StandardManipulator
{
    typedef StandardManipulator inherited;
    static const auto KEY_ALT = 16777251;
    static const auto KEY_Z = 90;
public:
    DefaultManipulator(int flags = DEFAULT_SETTINGS);
    DefaultManipulator( const DefaultManipulator& om, const osg::CopyOp& copyOp = osg::CopyOp::SHALLOW_COPY );

    META_Object( osgGA, DefaultManipulator )

    //CameraManipulator
    /** set the position of the matrix manipulator using a 4x4 Matrix.*/
    virtual void setByMatrix(const osg::Matrixd& matrix);

    /** set the position of the matrix manipulator using a 4x4 Matrix.*/
    virtual void setByInverseMatrix(const osg::Matrixd& matrix);

    /** get the position of the manipulator as 4x4 Matrix.*/
    virtual osg::Matrixd getMatrix() const;

    /** get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.*/
    virtual osg::Matrixd getInverseMatrix() const;

    //StandarManipulator
    /** Sets manipulator by eye position and eye orientation.*/
    virtual void setTransformation( const osg::Vec3d& eye, const osg::Quat& rotation );

    /** Sets manipulator by eye position, center of rotation, and up vector.*/
    virtual void setTransformation( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up );

    /** Gets manipulator's eye position and eye orientation.*/
    virtual void getTransformation( osg::Vec3d& eye, osg::Quat& rotation ) const;

    /** Gets manipulator's focal center, eye position, and up vector.*/
    virtual void getTransformation( osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up ) const;

    virtual void setCenter( const osg::Vec3d& center );
    const osg::Vec3d& getCenter() const;

    virtual void setMinimumDistance( const double& minimumDistance, bool relativeToModelSize = false );
    double getMinimumDistance( bool *relativeToModelSize = NULL ) const;

    /** Set the distance of camera to the center. */
    void setDistance( double distance );

    /** Get the distance of the camera to the center. */
    double getDistance() const;

protected:
    MatrixTransform* world;

    //StandardManipulator
    virtual bool handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
    virtual bool handleKeyUp( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
    virtual bool performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy );
    virtual void panModel( const float dx, const float dy, const float dz = 0.f );
    void rotateWithFixedVertical( const float dx, const float dy );
    bool altPressed;

    virtual bool handleMouseWheel( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
    virtual void zoomModel( const float dy, bool pushForwardIfNeeded = true );

    osg::Vec3d _center;
    osg::Quat  _rotation;
    double     _distance;
    double _trackballSize;
    double _wheelZoomFactor;

    double _minimumDistance;
    static int _minimumDistanceFlagIndex;
};
}
#endif // DEFAULTMANIPULATOR_H
