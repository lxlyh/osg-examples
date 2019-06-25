#ifndef SCRIBEDNODEVISITOR_H
#define SCRIBEDNODEVISITOR_H

#include <osg/NodeVisitor>
#include <osg/Geometry>
using namespace osg;

class ScribedNodeVisitor : public NodeVisitor
{
public:
    ScribedNodeVisitor();
    virtual void apply(Geometry& geometry);
};

class UnScribedNodeVisitor : public NodeVisitor
{
public:
    UnScribedNodeVisitor();
    virtual void apply(Geometry& geometry);
};

#endif // SCRIBEDNODEVISITOR_H
