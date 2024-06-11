// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2024 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#include "objecttreemodel.h"

#include <mapobjectscontroller.h>
#include <objects/objecttreeentity.h>
#include <objects/layerentity.h>
#include <objects/shapefilelayerentity.h>
#include <objects/harbourentity.h>
#include <objects/nodeentity.h>
#include <objects/vesselentity.h>
#include <objects/firmentity.h>
#include <objects/shipentity.h>
#include <objects/fishfarmentity.h>
#include <objects/windmillentity.h>
#include <objects/populationentity.h>
#include <objects/benthosentity.h>
#include <objects/metiersentity.h>
#include <objects/nationentity.h>
#include <objects/szgroupentity.h>
#include <objects/bfunctionalgroupsentity.h>
#include <objects/ffarmtypesentity.h>
#include <objects/wfarmtypesentity.h>
#include <objects/shiptypesentity.h>

#include <displacemodel.h>
#include <QMapControl/QMapControl.h>

QList<objecttree::ObjectTreeEntity *> ObjectTreeModel::entityTemplates;

QString ObjectTreeModel::entityNames[] = {
    QT_TR_NOOP_UTF8("Layers"),
    QT_TR_NOOP_UTF8("GIS Shapefile Layers"),
    QT_TR_NOOP_UTF8("Environmental Forcing"),
    QT_TR_NOOP_UTF8("Monthly Output Layers"),
    QT_TR_NOOP_UTF8("Monthly Fishing Tariffs"),
    QT_TR_NOOP_UTF8("Nodes"),
    QT_TR_NOOP_UTF8("Fish Populations"),
    QT_TR_NOOP_UTF8("Fish Size Groups or Age"),
    QT_TR_NOOP_UTF8("Benthos"),
    QT_TR_NOOP_UTF8("Benthos Functional Groups"),
    QT_TR_NOOP_UTF8("Fishing Nations"),
    QT_TR_NOOP_UTF8("Fishing Firms"),
    QT_TR_NOOP_UTF8("Fishing Metiers"),
    QT_TR_NOOP_UTF8("Fishing Harbours"),
    QT_TR_NOOP_UTF8("Fishing Vessels"),
    QT_TR_NOOP_UTF8("Commercial Shipping"),
    QT_TR_NOOP_UTF8("Commercial Shipping Types"),
    QT_TR_NOOP_UTF8("Fish Farms"),
    QT_TR_NOOP_UTF8("Fish Farm Types"),
    QT_TR_NOOP_UTF8("Wind Farms"),
    QT_TR_NOOP_UTF8("Wind Farms Types"),
};

ObjectTreeModel::ObjectTreeModel(MapObjectsController *map, StatsController *stats, QObject *parent) :
    QAbstractItemModel(parent),
    mMapControl(map),
    mStatsController(stats),
    mModel (0),
    mModelIdx(-1)
{
    if (entityTemplates.size() == 0) {
        // Registers all entity templates
        entityTemplates.reserve(LastCategory);

        for (int i = 0; i < LastCategory; ++i)
            entityTemplates.push_back(0);

        entityTemplates[Layers] = new objecttree::LayerEntity(Layers, this);
        entityTemplates[ShapefileLayers] = new objecttree::ShapefileLayerEntity(this);
        entityTemplates[EnvLayers] = new objecttree::LayerEntity(EnvLayers, this);
        entityTemplates[OutputLayers] = new objecttree::LayerEntity(OutputLayers, this);
        entityTemplates[TariffsLayers] = new objecttree::LayerEntity(TariffsLayers, this);
        entityTemplates[Harbours] = new objecttree::HarbourEntity(this);
        entityTemplates[Nodes] = new objecttree::NodeEntity(this);
        entityTemplates[Vessels] = new objecttree::VesselEntity(this);
        entityTemplates[Firms] = new objecttree::FirmEntity(this);
        entityTemplates[Ships] = new objecttree::ShipEntity(this);
        entityTemplates[Fishfarms] = new objecttree::FishfarmEntity(this);
        entityTemplates[Windmills] = new objecttree::WindmillEntity(this);
        entityTemplates[Populations] = new objecttree::PopulationEntity(this);
        entityTemplates[Benthos] = new objecttree::BenthosEntity(this);
        entityTemplates[Metiers] = new objecttree::MetiersEntity(this);
        entityTemplates[Nations] = new objecttree::NationEntity(this);
        entityTemplates[SizeGroups] = new objecttree::SzGroupEntity(this);

        funcgroups = new objecttree::BFunctionalGroupsEntity(this, nullptr);
        entityTemplates[BenthosFunctionalGroups] = funcgroups;

        farmtypes = new objecttree::FFarmTypesEntity(this, nullptr);
        entityTemplates[FishfarmsTypes] = farmtypes;

        windfarmtypes = new objecttree::WFarmTypesEntity(this, nullptr);
        entityTemplates[WindfarmsTypes] = windfarmtypes;

        shiptypes = new objecttree::ShipTypesEntity(this, nullptr);
        entityTemplates[ShipsTypes] = shiptypes;

    }
}

int ObjectTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

int ObjectTreeModel::rowCount(const QModelIndex &parent) const
{
    if (isRootLevel(parent)) { // Categories level
        return LastCategory;
    } else if (isCategoryLevel(parent)) {
        if (entityTemplates[parent.row()])
            return entityTemplates[parent.row()]->rowCount();
        return 0;
    } else {
        return entity(parent)->rowCount();
    }
}

QModelIndex ObjectTreeModel::parent(const QModelIndex &child) const
{
    if (isRootLevel(child) || isCategoryLevel(child))
        return QModelIndex();

    return entity(child)->parent(child);
}

QModelIndex ObjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (isRootLevel(parent)) {
        return createCategoryEntity(row, column, (Category)row);
    } else if (isCategoryLevel(parent)) {
        return entityTemplates[parent.row()]->index(row, column, parent);
    } else {
        return entity(parent)->index(row, column, parent);
    }
}

QVariant ObjectTreeModel::data(const QModelIndex &index, int role) const
{
    if (isCategoryLevel(index)) {
        if (role != Qt::DisplayRole)
            return QVariant();

        if (index.column() != 0)
          return QVariant();

        return QVariant(entityNames[index.row()]);
    } else if(!isRootLevel(index)) {
        return entity(index)->data(index, role);
    }

    return QVariant();
}

Qt::ItemFlags ObjectTreeModel::flags(const QModelIndex &index) const
{
    if (!isCategoryLevel(index) && !isRootLevel(index))
        return entity(index)->flags(QAbstractItemModel::flags(index), index);

    return QAbstractItemModel::flags(index);

}

bool ObjectTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!isCategoryLevel(index) && !isRootLevel(index)) {
        if (entity(index)->setData(index, value, role)) {
            emit dataChanged(index, index);
            return true;
        }
    }
    return false;
}

void ObjectTreeModel::setCurrentModel(int idx, DisplaceModel *model)
{
    beginResetModel();
    mModel = model;
    mModelIdx = idx;

    if (mModel) {
        windfarmtypes->setInterestingList(mModel->getWindfarmsTypesList());
        shiptypes->setInterestingList(mModel->getShipsTypesList());
        farmtypes->setInterestingList(mModel->getFishfarmsTypesList());
        funcgroups->setInterestingList(mModel->getFunctionalGroupsList());
    }

    for (auto entity : entityTemplates) {
        entity->modelChanged(idx);
    }
    endResetModel();
}

QModelIndex ObjectTreeModel::createCategoryEntity(int row, int column, Category cat) const
{
    return createIndex(row, column, entityTemplates[cat]);
}

QModelIndex ObjectTreeModel::createCategoryEntityFromChild(ObjectTreeModel::Category cat) const
{
    return createIndex(cat, 0, entityTemplates[cat]);
}

QModelIndex ObjectTreeModel::createEntity(int row, int column, objecttree::ObjectTreeEntity *entity) const
{
    return createIndex(row, column, entity);
}

bool ObjectTreeModel::isObject(QModelIndex index) const
{
    return (!isRootLevel(index) && !isCategoryLevel(index));
}

ObjectTreeModel::Category ObjectTreeModel::getCategory(QModelIndex index) const
{
    return entity(index)->getCategory();
}

objecttree::ObjectTreeEntity *ObjectTreeModel::entity(const QModelIndex &index) const
{
    return (objecttree::ObjectTreeEntity*)index.internalPointer();
}

void ObjectTreeModel::refresh()
{
    beginResetModel();
    endResetModel();
}
