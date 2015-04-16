#ifndef DTEDITORWINDOW_H
#define DTEDITORWINDOW_H

#include <QMainWindow>
#include <boost/shared_ptr.hpp>
#include <dtree/decisiontree.h>
#include <dtgraphicsscene.h>

namespace Ui {
class DtEditorWindow;
}

class DtEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DtEditorWindow(QWidget *parent = 0);
    ~DtEditorWindow();

protected:
    void save(QString filename);
    void open(QString filename);
    void updateTitleBar();
    void updateGui();
    bool checkForDTreeBeforeSaving();
    void createScene(boost::shared_ptr<dtree::DecisionTree> tree);

    void closeEvent(QCloseEvent *event);

private slots:
    void on_action_Add_Node_triggered();
    void evt_scene_node_added(GraphNodeItem *);
    void evt_scene_selection_changed();
    void on_nodepropVariable_currentIndexChanged(int index);
    void on_actionSave_as_triggered();
    void on_action_Open_triggered();
    void on_nodeValue_valueChanged(double value);
    void on_action_Save_triggered();
    void on_treeType_currentIndexChanged(int index);

    void on_action_Delete_Nodes_triggered();

private:
    Ui::DtEditorWindow *ui;

    boost::shared_ptr<dtree::DecisionTree> mTree;
    DtGraphicsScene *mScene;

    QString mFilename;
};

#endif // DTEDITORWINDOW_H
