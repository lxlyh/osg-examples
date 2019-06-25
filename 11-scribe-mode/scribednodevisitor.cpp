#include "scribednodevisitor.h"

ScribedNodeVisitor::ScribedNodeVisitor() : NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN) {

}

void ScribedNodeVisitor::apply(Geometry& geometry) {
    for(auto ps : geometry.getPrimitiveSetList()) {
        ps->setMode(osg::PrimitiveSet::LINE_STRIP);
    }
}

UnScribedNodeVisitor::UnScribedNodeVisitor() : NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

void UnScribedNodeVisitor::apply(Geometry& geometry) {
    for(auto ps : geometry.getPrimitiveSetList()) {
        ps->setMode(osg::PrimitiveSet::TRIANGLE_STRIP);
    }
}
