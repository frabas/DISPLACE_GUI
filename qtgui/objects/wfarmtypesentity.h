#ifndef WFARMTYPESENTITY_H
#define WFARMTYPESENTITY_H

#include <objects/objecttreeentity.h>
#include <utils/interestinglistwithspecialvalues.h>
#include <memory>

        namespace objecttree {

        class WFarmTypesEntity : public ObjectTreeEntity
        {
            int mGroupIndex;
            QString mLabel;
            std::shared_ptr<InterestingListWithSpecialValues<int>> mList;

        public:
            explicit WFarmTypesEntity(ObjectTreeModel *_model, std::shared_ptr<InterestingListWithSpecialValues<int>> list, int idx = -1);

            virtual ObjectTreeModel::Category getCategory() const override {
                return ObjectTreeModel::WindfarmsTypes;
            }

            void setLabel(QString label) { mLabel = label; }
            void setInterestingList(std::shared_ptr<InterestingListWithSpecialValues<int>> list);

            virtual QModelIndex parent (const QModelIndex &parent) const override;
            virtual QModelIndex index (int row, int column, const QModelIndex &parent) const override;
            virtual int rowCount() const override;
            virtual int columnCount() const override;
            virtual QVariant data(const QModelIndex &index, int role) const override;
            virtual Qt::ItemFlags flags(Qt::ItemFlags defflags, const QModelIndex &index) const override;
            virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;

            void modelChanged(int idx) override;
        };

        } // ns

#endif // WFARMTYPESENTITY_H
