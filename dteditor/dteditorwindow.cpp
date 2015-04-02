#include "dteditorwindow.h"
#include "ui_dteditorwindow.h"
#include <dtree/dtnode.h>
#include <graphnodeextra.h>
#include <graphnodeitem.h>
#include <dtcsvwriter.h>
#include <dtcsvreader.h>

#include <QCloseEvent>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>

DtEditorWindow::DtEditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DtEditorWindow),
    mTree(),
    mScene(0)
{
    ui->setupUi(this);

    // fill the combo
    for (int i = 0; i < dtree::VarLast; ++i) {
        ui->nodepropVariable->addItem(dtree::VariableNames::variableName(static_cast<dtree::Variable>(i)));
    }
    ui->nodepropVariable->setCurrentIndex(-1);

    mTree = boost::shared_ptr<dtree::DecisionTree>(new dtree::DecisionTree);

    mScene = new DtGraphicsScene(mTree);
    mScene->setSceneRect(QRectF(0, 0, 5000, 5000));

    connect (mScene, SIGNAL(nodeAddCompleted(QPointF)), this, SLOT(evt_scene_node_added(QPointF)));
    connect (mScene, SIGNAL(selectionChanged()), this, SLOT(evt_scene_selection_changed()));

    ui->treeView->setScene(mScene);

    QSettings set;
    restoreGeometry(set.value("mainGeometry").toByteArray());
    restoreState(set.value("mainState").toByteArray());

    evt_scene_selection_changed();
}

DtEditorWindow::~DtEditorWindow()
{
    delete ui;
}

void DtEditorWindow::save(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::warning(this, tr("Save failed"),
                             QString(tr("Error writing to file: %1")).arg(file.errorString()));
        return;
    }

    QTextStream strm(&file);
    DtCsvWriter exporter;

    if (!exporter.exportTree(strm, mTree.get(), mScene)) {
        QMessageBox::warning(this, tr("Save failed"),
                             QString(tr("Cannot export to csv file.")));
    }
}

void DtEditorWindow::open(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Open failed"),
                             QString(tr("Error Opening file: %1")).arg(file.errorString()));
        return;
    }

    QTextStream strm(&file);
    DtCsvReader reader;

    boost::shared_ptr<dtree::DecisionTree> tree;
    if (!reader.readTree(strm, &tree, mScene)) {
        QMessageBox::warning(this, tr("Load failed"),
                             QString(tr("Cannot export to csv file.")));
        return;
    }
    mTree = tree;


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
    boost::shared_ptr<dtree::Node> newnode = mTree->createNode();
    boost::shared_ptr<dtree::GraphNodeExtra> newextra (new dtree::GraphNodeExtra);

    newnode->setExtra(newextra);
    mScene->startAddNode(newnode);
}

void DtEditorWindow::evt_scene_node_added(QPointF pt)
{
}

void DtEditorWindow::evt_scene_selection_changed()
{
    QList<QGraphicsItem *> selection = mScene->selectedItems();

    if (selection.size() != 1) {
        // hide properties and disable controls
        ui->nodepropVariable->setEnabled(false);
        ui->nodepropVariable->setCurrentIndex(-1);
        //        ui->nodepropDetailsContainer->hide();
    } else {
        ui->nodepropVariable->setEnabled(true);
        GraphNodeItem *item = dynamic_cast<GraphNodeItem *>(selection[0]);

        // don't like this - TODO: fix it without using downcasting
        if (item) {
            boost::shared_ptr<dtree::Node> node = item->getNode();
            if (node.get() != 0) {
                dtree::Variable var = node->variable();
                ui->nodepropVariable->setCurrentIndex(var);

                boost::shared_ptr<dtree::NodeExtra> extra = node->extra();
                if (extra.get() != 0) {

                }
            }
        }
    }
}

void DtEditorWindow::on_nodepropVariable_currentIndexChanged(int index)
{
    if (!mScene) return;

    QList<QGraphicsItem *> selection = mScene->selectedItems();

    foreach (QGraphicsItem *i, selection) {
        GraphNodeItem *item = dynamic_cast<GraphNodeItem *>(i);

        // don't like this - TODO: fix it without using downcasting
        if (item) {
            boost::shared_ptr<dtree::Node> node = item->getNode();
            if (node.get() != 0) {
                node->setVariable(static_cast<dtree::Variable>(index));
            }
            item->update();
        }
    }
}

void DtEditorWindow::on_actionSave_as_triggered()
{
    QSettings s;
    QString last = s.value("last_tree").toString();
    QFileDialog dlg(this, tr("Exporting to CSV file"), last, tr("Tree files (*.dtcsv);;All files (*.*)"));
    dlg.setDefaultSuffix("dtcsv");
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setAcceptMode(QFileDialog::AcceptSave);

    if (dlg.exec() == QDialog::Accepted) {
        QStringList file = dlg.selectedFiles();
        if (file.size() != 1) {
            return;
        }
        save(file[0]);
        QFileInfo info(file[0]);
        s.setValue("last_tree", info.path());
    }
}

void DtEditorWindow::on_action_Open_triggered()
{
    QSettings s;
    QString last = s.value("last_tree").toString();
    QString file = QFileDialog::getOpenFileName(this, tr("Exporting to CSV file"), last, tr("Tree files (*.dtcsv);;All files (*.*)"));

    if (!file.isEmpty()) {
        try {
            open(file);
            QFileInfo info(file);
            s.setValue("last_tree", info.path());
        } catch (std::exception &x) {
            QMessageBox::warning(this, tr("Cannot load tree"), QString::fromStdString(x.what()));
            return;
        }
    }
}
