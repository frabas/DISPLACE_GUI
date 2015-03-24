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
    void closeEvent(QCloseEvent *event);

private slots:
    void on_action_Add_Node_triggered();
    void evt_scene_node_added(QPointF);
    void evt_scene_selection_changed();

    void on_nodepropVariable_currentIndexChanged(int index);

private:
    Ui::DtEditorWindow *ui;

    boost::shared_ptr<dtree::DecisionTree> mTree;
    DtGraphicsScene *mScene;
};

#endif // DTEDITORWINDOW_H
