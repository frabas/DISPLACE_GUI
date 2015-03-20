#include "dteditorwindow.h"
#include "ui_dteditorwindow.h"
#include <dtree/node.h>
#include <graphnodeextra.h>

#include <QCloseEvent>
#include <QSettings>

DtEditorWindow::DtEditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DtEditorWindow)
{
    ui->setupUi(this);

    mScene = new DtGraphicsScene;
    mScene->setSceneRect(QRectF(0, 0, 5000, 5000));

    mTree = boost::shared_ptr<dtree::DecisionTree>(new dtree::DecisionTree);

    connect (mScene, SIGNAL(nodeAddCompleted(QPointF)), this, SLOT(evt_scene_node_added(QPointF)));

    ui->treeView->setScene(mScene);

    QSettings set;
    restoreGeometry(set.value("mainGeometry").toByteArray());
    restoreState(set.value("mainState").toByteArray());
}

DtEditorWindow::~DtEditorWindow()
{
    delete ui;
}

void DtEditorWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

    QSettings sets;
    sets.setValue("mainGeometry", saveGeometry());
    sets.setValue("mainState", saveState());

    qApp->closeAllWindows();
}

void DtEditorWindow::on_action_Add_Node_triggered()
{
    mScene->startAddNode();
}

void DtEditorWindow::evt_scene_node_added(QPointF pt)
{
    boost::shared_ptr<dtree::Node> newnode = mTree->createNode();
    boost::shared_ptr<dtree::GraphNodeExtra> newextra (new dtree::GraphNodeExtra);

    newnode->setExtra(newextra);

    mScene->addUnconnectedNode(newnode, pt);
}
