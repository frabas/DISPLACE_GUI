#ifndef BFUNCTIONALGROUPSENTITY_H
#define BFUNCTIONALGROUPSENTITY_H

#include <objects/objecttreeentity.h>
#include <utils/interestinglistwithspecialvalues.h>
#include <memory>

namespace objecttree {

class BFunctionalGroupsEntity : public ObjectTreeEntity
{
    int mGroupIndex;
    QString mLabel;
    std::shared_ptr<InterestingListWithSpecialValues<int>> mList;

public:
    explicit BFunctionalGroupsEntity(ObjectTreeModel *_model, std::shared_ptr<InterestingListWithSpecialValues<int>> list, int idx = -1);

    virtual ObjectTreeModel::Category getCategory() const {
        return ObjectTreeModel::BenthosFunctionalGroups;
    }

    void setLabel(QString label) { mLabel = label; }
    void setInterestingList(std::shared_ptr<InterestingListWithSpecialValues<int>> list);

    virtual QModelIndex parent (const QModelIndex &parent) const;
    virtual QModelIndex index (int row, int column, const QModelIndex &parent) const;
    virtual int rowCount() const;
    virtual int columnCount() const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(Qt::ItemFlags defflags, const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

    void modelChanged(int idx) override;
};

} // ns

#endif // BFUNCTIONALGROUPSENTITY_H
