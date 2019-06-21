#include "common.h"

using std::cout;
using std::endl;

ref_ptr<Geode> makeAxis(float radious)
{
     //创建保存几何信息的对象
    auto geom = new osg::Geometry();
    auto limit = radious;

    //创建四个顶点
    //为每个顶点碎片和热指定一种颜色
    auto v = new osg::Vec3Array();
    auto c = new osg::Vec4Array();
    auto i = 0;

    v->push_back(osg::Vec3(-limit * 2, 0, 0));
    v->push_back(osg::Vec3(limit * 2, 0, 0));
    c->push_back(osg::Vec4(1.0f,0.0f, 0.0f, 1.0f));
    c->push_back(osg::Vec4(1.0f,0.0f, 0.0f, 1.0f));
    geom->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::LINES, i, 2));
    i+=2;

    v->push_back(osg::Vec3(0,-limit * 2, 0));
    v->push_back(osg::Vec3(0,limit * 2, 0));
    c->push_back(osg::Vec4(0.0f,1.0f, 0.0f, 1.0f));
    c->push_back(osg::Vec4(0.0f,1.0f, 0.0f, 1.0f));
    geom->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::LINES, i, 2));
    i+=2;

    v->push_back(osg::Vec3(0,0,-limit * 2));
    v->push_back(osg::Vec3(0,0,limit * 2));
    c->push_back(osg::Vec4(0.0f,0.0f, 1.0f, 1.0f));
    c->push_back(osg::Vec4(0.0f,0.0f, 1.0f, 1.0f));
    geom->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::LINES, i, 2));
    i+=2;

    geom->setVertexArray(v);
    geom->setColorArray(c);
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    ref_ptr<Geode> geode = new osg::Geode();
    geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(0.5f),  osg::StateAttribute::ON);

    auto ss = geom->getOrCreateStateSet();
    auto blendFunc = new osg::BlendFunc();
    blendFunc->setSource(osg::BlendFunc::SRC_ALPHA);
    blendFunc->setDestination(osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);
    ss->setAttributeAndModes(blendFunc);

    geode->addDrawable(geom);

    return geode;
}

ref_ptr<Geode> makeGround(float radius) {
    float width = 2 * radius;
    float height = 2 * radius;

    Vec3 v000(Vec3(0,0,0) - Vec3(radius, radius, 0.0f));
    Vec3 vX(Vec3(radius, -radius, 0.0f));
    Vec3 vY(Vec3(-radius, radius, 0.0f));

    Vec3 dx(Vec3(width / 100.0f,0.0f,0.0f));
    Vec3 dy(Vec3(0.0f,height / 100.0f,0.0f));

    auto primitives = new DrawElementsUShort(GL_LINES);

    auto coords = new Vec3Array;
    auto colors = new Vec4Array();
    int i = -1;
    for(auto iy = 0;iy < dy.y() + 1;++iy)
    {
        for(auto ix = 0;ix < dx.x() + 1;++ix)
        {
            coords->push_back(v000 + dx * ix);
            coords->push_back(vY + dx * ix);
            primitives->push_back(++i);
            primitives->push_back(++i);
            colors->push_back(Vec4(1,1,1,0.2));
            colors->push_back(Vec4(1,1,1,0.2));
        }

        coords->push_back(v000 + dy * iy);
        coords->push_back(vX + dy * iy);
        primitives->push_back(++i);
        primitives->push_back(++i);
        colors->push_back(Vec4(1,1,1,0.2));
        colors->push_back(Vec4(1,1,1,0.2));
    }

    auto geom = new Geometry();
    geom->setVertexArray(coords);
    geom->setColorArray(colors);
    geom->setColorBinding(Geometry::BIND_PER_VERTEX);
    geom->addPrimitiveSet(primitives);

    auto ss = geom->getOrCreateStateSet();
    auto blendFunc = new osg::BlendFunc();
    blendFunc->setSource(osg::BlendFunc::SRC_ALPHA);
    blendFunc->setDestination(osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);
    ss->setAttributeAndModes(blendFunc);

    auto geode = new Geode();
    geode->addChild(geom);
    return geode;
}

//创建棋盘
ref_ptr<osg::Node> makeBase(float radius)
{
    float width = 2 * radius;
    float height = 2 * radius;

    int numTilesX = 1;
    int numTilesY = 1;

    Vec3 v000(Vec3(0,0,0) - Vec3(width*0.5f,height*0.5f,0.0f));
    Vec3 dx(Vec3(width / ((float)numTilesX),0.0,0.0f));
    Vec3 dy(Vec3(0.0f,height / ((float)numTilesY),0.0f));

    // fill in vertices for grid, note numTilesX+1 * numTilesY+1...
    auto coords = new Vec3Array;
    int iy;
    for(iy = 0;iy <= numTilesY;++iy)
        for(int ix = 0;ix <= numTilesX;++ix)
            coords->push_back(v000 + dx * (float)ix + dy * (float)iy);

    //Just two colours - black and white.
    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,0.1f)); // white

    osg::ref_ptr<osg::DrawElementsUShort> whitePrimitives = new osg::DrawElementsUShort(GL_QUADS);

    int numIndicesPerRow=numTilesX+1;
    for(iy=0;iy<numTilesY;++iy)
    {
        for(int ix=0;ix<numTilesX;++ix)
        {
            osg::DrawElementsUShort* primitives = whitePrimitives;
            primitives->push_back(ix     + (iy+1) * numIndicesPerRow);
            primitives->push_back(ix     + iy     * numIndicesPerRow);
            primitives->push_back((ix+1) + iy     * numIndicesPerRow);
            primitives->push_back((ix+1) + (iy+1) * numIndicesPerRow);
        }
    }

    osg::Geometry* geom = new osg::Geometry;
    geom->setVertexArray(coords);
    geom->setColorArray(colors, osg::Array::BIND_PER_PRIMITIVE_SET);
    geom->addPrimitiveSet(whitePrimitives.get());

    auto ss = geom->getOrCreateStateSet();

    auto blendFunc = new osg::BlendFunc();
    blendFunc->setSource(osg::BlendFunc::SRC_ALPHA);
    blendFunc->setDestination(osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);
    ss->setAttributeAndModes(blendFunc);
    ss->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);//取消深度测试

    ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(geom);

    return geode;
}

osg::TextureCubeMap* readCubeMap()
{
    osg::TextureCubeMap* cubemap = new osg::TextureCubeMap;
    #define CUBEMAP_FILENAME(face) "skyboxes/sky/" #face ".jpg"

    osg::Image* imagePosX = osgDB::readImageFile(CUBEMAP_FILENAME(sunny3_left));
    osg::Image* imageNegZ = osgDB::readImageFile(CUBEMAP_FILENAME(sunny3_back));
    osg::Image* imageNegX = osgDB::readImageFile(CUBEMAP_FILENAME(sunny3_right));
    osg::Image* imagePosZ = osgDB::readImageFile(CUBEMAP_FILENAME(sunny3_front));

    osg::Image* imagePosY = osgDB::readImageFile(CUBEMAP_FILENAME(sunny3_down));
    osg::Image* imageNegY = osgDB::readImageFile(CUBEMAP_FILENAME(sunny3_up));

    if (imagePosX && imageNegX && imagePosY /*&& imageNegY && imagePosZ && imageNegZ*/)
    {
       cubemap->setImage(osg::TextureCubeMap::POSITIVE_X, imagePosX);
       cubemap->setImage(osg::TextureCubeMap::NEGATIVE_X, imageNegX);
       cubemap->setImage(osg::TextureCubeMap::POSITIVE_Y, imagePosY);
       cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Y, imageNegY);
       cubemap->setImage(osg::TextureCubeMap::POSITIVE_Z, imagePosZ);
       cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Z, imageNegZ);

       cubemap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
       cubemap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
       cubemap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);

       cubemap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
       cubemap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    }

    return cubemap;
}


// Update texture matrix for cubemaps
struct TexMatCallback : public osg::NodeCallback
{
public:

TexMatCallback(osg::TexMat& tm) :
    _texMat(tm)
    {}

    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
        if (cv)
        {
            const osg::Matrix& MV = *cv->getModelViewMatrix();
            const osg::Matrix R = osg::Matrix::rotate( osg::DegreesToRadians(112.0f), 0.0f,0.0f,1.0f)*
            osg::Matrix::rotate( osg::DegreesToRadians(90.0f), 1.0f,0.0f,0.0f);

            osg::Quat q = MV.getRotate();
            const osg::Matrix C = osg::Matrix::rotate( q.inverse() );

            _texMat.setMatrix( C*R );
        }

        traverse(node,nv);
    }

    osg::TexMat& _texMat;
};


class MoveEarthySkyWithEyePointTransform : public osg::Transform
{
public:
    /** Get the transformation matrix which moves from local coords to world coords.*/
    virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const
    {
       osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
       if (cv)
       {
            osg::Vec3 eyePointLocal = cv->getEyeLocal();
            matrix.preMult(osg::Matrix::translate(eyePointLocal));
       }
       return true;
    }

    /** Get the transformation matrix which moves from world coords to local coords.*/
    virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const
    {
       osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
       if (cv)
       {
            osg::Vec3 eyePointLocal = cv->getEyeLocal();
            matrix.postMult(osg::Matrix::translate(-eyePointLocal));
       }
       return true;
    }
};

osg::Node* createSkyBox()
{
    osg::StateSet* stateset = new osg::StateSet();

    osg::TexEnv* te = new osg::TexEnv;
    te->setMode(osg::TexEnv::REPLACE);
    stateset->setTextureAttributeAndModes(0, te, osg::StateAttribute::ON);

    osg::TexGen *tg = new osg::TexGen;
    tg->setMode(osg::TexGen::NORMAL_MAP);
    stateset->setTextureAttributeAndModes(0, tg, osg::StateAttribute::ON);

    osg::TexMat *tm = new osg::TexMat;
    stateset->setTextureAttribute(0, tm);

    osg::TextureCubeMap* skymap = readCubeMap();
    stateset->setTextureAttributeAndModes(0, skymap, osg::StateAttribute::ON);

    stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    stateset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );

    // clear the depth to the far plane.
    osg::Depth* depth = new osg::Depth;
    depth->setFunction(osg::Depth::ALWAYS);
    depth->setRange(1.0,1.0);
    stateset->setAttributeAndModes(depth, osg::StateAttribute::ON );

    stateset->setRenderBinDetails(-1,"RenderBin");

    osg::Drawable* drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),1));

    osg::Geode* geode = new osg::Geode;
    geode->setCullingActive(false);
    geode->setStateSet(stateset);
    geode->addDrawable(drawable);

    osg::Transform* transform = new MoveEarthySkyWithEyePointTransform;
    transform->setCullingActive(false);
    transform->addChild(geode);

    osg::ClearNode* clearNode = new osg::ClearNode;
    //   clearNode->setRequiresClear(false);
    clearNode->setCullCallback(new TexMatCallback(*tm));
    clearNode->addChild(transform);

    return clearNode;
}

