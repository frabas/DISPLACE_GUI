#ifndef NODEMAPPINGSDIALOG_H
#define NODEMAPPINGSDIALOG_H

#include <QDialog>
#include <QVector>
#include <QComboBox>
#include <QGridLayout>

#include <boost/shared_ptr.hpp>

namespace Ui {
class NodeMappingsDialog;
}

namespace dtree {
class Node;
}

class NodeMappingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NodeMappingsDialog(boost::shared_ptr<dtree::Node> node, QWidget *parent = 0);
    ~NodeMappingsDialog();

    int getMappingForIndex(int idx);

private:
    Ui::NodeMappingsDialog *ui;
    QGridLayout *mGroupLayout;
    boost::shared_ptr<dtree::Node> mNode;
    QVector<QComboBox *> mCombos;

    void setupMappings();
};

#endif // NODEMAPPINGSDIALOG_H
