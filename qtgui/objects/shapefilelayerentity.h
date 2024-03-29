#ifndef SHAPEFILELAYERENTITY_H
#define SHAPEFILELAYERENTITY_H

#include <objects/layerentity.h>

namespace objecttree {

class ShapefileLayerEntity : public LayerEntity
{
    Q_OBJECT
public:
    explicit ShapefileLayerEntity(ObjectTreeModel *_model, int LayerEntity_idx = -1);
    virtual ~ShapefileLayerEntity();
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;

    virtual QMenu *contextMenu() const override ;

public slots:
    void onActionShapeColor();

private:
    QMenu *mContextMenu;

    void initMenu();
};

} // ns

#endif // SHAPEFILELAYERENTITY_H
