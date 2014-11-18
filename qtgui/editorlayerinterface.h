#ifndef EDITORLAYERINTERFACE_H
#define EDITORLAYERINTERFACE_H

namespace qmapcontrol {
class Geometry;
}

#include <memory>

class EditorLayerInterface
{
public:
    virtual void clearEditorLayer() = 0;
    virtual void addEditorLayerGeometry (std::shared_ptr<qmapcontrol::Geometry> geometry) = 0;
};

#endif // EDITORLAYERINTERFACE_H
