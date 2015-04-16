#include "dteditorwindow.h"
#include "ui_dteditorwindow.h"
#include <dtree/dtnode.h>
#include <dtree/decisiontreemanager.h>
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

    for (int i = 0; i < dtree::DecisionTreeManager::SIZE; ++i) {
        ui->treeType->addItem(QString::fromStdString(dtree::DecisionTreeManager::manager()->treeTypeCode(static_cast<dtree::DecisionTreeManager::TreeType>(i))));
    }
    ui->treeType->setCurrentIndex(-1);

    createScene(boost::shared_ptr<dtree::DecisionTree>(new dtree::DecisionTree));

    QSettings set;
    restoreGeometry(set.value("mainGeometry").toByteArray());
    restoreState(set.value("mainState").toByteArray());

    updateUndoGuiStatus();
    evt_scene_selection_changed();
}

DtEditorWindow::~DtEditorWindow()
{
    delete ui;
}

void DtEditorWindow::createScene(boost::shared_ptr<dtree::DecisionTree> tree)
{
    mTree = tree;
    mScene = new DtGraphicsScene(mTree);
    mScene->setSceneRect(QRectF(0, 0, 5000, 5000));

    connect (mScene, SIGNAL(nodeAdded(GraphNodeItem*)), this, SLOT(evt_scene_node_added(GraphNodeItem*)));
    connect (mScene, SIGNAL(selectionChanged()), this, SLOT(evt_scene_selection_changed()));

    ui->treeView->setScene(mScene);
}

void DtEditorWindow::undo()
{
    if (!mUndoList.empty()) {
        boost::shared_ptr<Command> command = mUndoList.pop();
        if (command->undo()) {
            mRedoList.push(command);
        }
    }
    updateUndoGuiStatus();
}

void DtEditorWindow::redo()
{
    if (!mRedoList.empty()) {
        boost::shared_ptr<Command> command = mRedoList.pop();
        if (command->redo()) {
            mUndoList.push(command);
        }
    }
    updateUndoGuiStatus();
}

void DtEditorWindow::execute(boost::shared_ptr<Command> command)
{
    if (command->execute()) {
        mUndoList.push(command);
        mRedoList.clear();
    }
    updateUndoGuiStatus();
}

void DtEditorWindow::clearUndoRedoStacks()
{
    mUndoList.clear();
    mRedoList.clear();
    updateUndoGuiStatus();
}

void DtEditorWindow::updateUndoGuiStatus()
{
    ui->actionUndo->setEnabled(mUndoList.size() > 0);
    ui->actionRedo->setEnabled(mRedoList.size() > 0);
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

    clearUndoRedoStacks();

    QTextStream strm(&file);
    DtCsvReader reader;

    mScene->clear();
    mTree->clear();
    if (!reader.readTree(strm, mTree, mScene)) {
        QMessageBox::warning(this, tr("Load failed"),
                             QString(tr("Cannot export to csv file.")));
        return;
    }

    updateGui();
}

void DtEditorWindow::updateTitleBar()
{
    if (mFilename.isEmpty()) {
        setWindowTitle(tr("Decision Tree Editor"));
    } else {
        QFileInfo info (mFilename);
        setWindowTitle(QString(tr("Decision Tree Editor - %1")).arg(info.fileName()));
    }
}

void DtEditorWindow::updateGui()
{
    if (mTree) {
        ui->treeType->setCurrentIndex(static_cast<int>(mTree->type()));
    }
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

void DtEditorWindow::evt_scene_node_added(GraphNodeItem *node)
{
    mScene->clearSelection();
    node->setSelected(true);
}

void DtEditorWindow::evt_scene_selection_changed()
{
    QList<QGraphicsItem *> selection = mScene->selectedItems();

    ui->action_Delete_Nodes->setEnabled(selection.size() > 0);

    if (selection.size() == 0) {
        // hide properties and disable controls
        ui->nodepropVariable->setEnabled(false);
        ui->nodepropVariable->setCurrentIndex(-1);
        ui->nodeValue->setEnabled(false);
        ui->nodeValue->setValue(0);
    } else {
        ui->nodepropVariable->setEnabled(true);
        GraphNodeItem *item = dynamic_cast<GraphNodeItem *>(selection[0]);

        // don't like this - TODO: fix it without using downcasting
        if (item) {
            boost::shared_ptr<dtree::Node> node = item->getNode();
            if (node.get() != 0) {
                dtree::Variable var = node->variable();
                ui->nodepropVariable->setCurrentIndex(var);

                if (var == dtree::Variable::VarLeaf) {
                    ui->nodeValue->setEnabled(true);
                    ui->nodeValue->setValue(node->value());
                } else {
                    ui->nodeValue->setEnabled(false);
                    ui->nodeValue->setValue(0);
                }

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
                dtree::Variable var = static_cast<dtree::Variable>(index);
                node->setVariable(var);
                item->setVariable(var);
                if (var == dtree::Variable::VarLeaf) {
                    ui->nodeValue->setEnabled(true);
                    ui->nodeValue->setValue(node->value());
                } else {
                    ui->nodeValue->setEnabled(false);
                    ui->nodeValue->setValue(0);
                }
            }
            item->update();
        }
    }
}

void DtEditorWindow::on_nodeValue_valueChanged(double value)
{
    if (!mScene) return;

    QList<QGraphicsItem *> selection = mScene->selectedItems();

    foreach (QGraphicsItem *i, selection) {
        GraphNodeItem *item = dynamic_cast<GraphNodeItem *>(i);

        // don't like this - TODO: fix it without using downcasting
        if (item) {
            boost::shared_ptr<dtree::Node> node = item->getNode();
            if (node.get() != 0) {
                node->setValue(value);
            }
            item->update();
        }
    }
}

void DtEditorWindow::on_actionSave_as_triggered()
{
    if (!checkForDTreeBeforeSaving())
        return;

    QSettings s;
    QString last = s.value("last_tree").toString();
    QFileDialog dlg(this, tr("Exporting to CSV file"), last, tr("Tree files (*.dt.csv);;All files (*.*)"));
    dlg.setDefaultSuffix("dt.csv");
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setAcceptMode(QFileDialog::AcceptSave);

    if (dlg.exec() == QDialog::Accepted) {
        QStringList file = dlg.selectedFiles();
        if (file.size() != 1) {
            return;
        }
        save(file[0]);
        mFilename = file[0];
        QFileInfo info(file[0]);
        s.setValue("last_tree", info.path());

        updateTitleBar();
    }
}

void DtEditorWindow::on_action_Open_triggered()
{
    QSettings s;
    QString last = s.value("last_tree").toString();
    QString file = QFileDialog::getOpenFileName(this, tr("Exporting to CSV file"), last, tr("Tree files (*.dt.csv);;All files (*.*)"));

    if (!file.isEmpty()) {
        try {
            open(file);
            QFileInfo info(file);
            s.setValue("last_tree", info.path());
            mFilename = file;
            updateTitleBar();
        } catch (std::exception &x) {
            QMessageBox::warning(this, tr("Cannot load tree"), QString::fromStdString(x.what()));
            return;
        }
    }
}

void DtEditorWindow::on_action_Save_triggered()
{
    if (!checkForDTreeBeforeSaving())
        return;

    if (mFilename.isEmpty()) {
        on_actionSave_as_triggered();
    } else {
        save(mFilename);
        updateTitleBar();
    }
}

bool DtEditorWindow::checkForDTreeBeforeSaving()
{
    if (mTree->type() == dtree::DecisionTreeManager::InvalidTreeType) {
        QMessageBox::warning(this, tr("Invalid DTree"),
                             tr("Cannot save Decision Tree: first assign a type to the tree."));
        return false;
    }
    return true;
}

void DtEditorWindow::on_treeType_currentIndexChanged(int index)
{
    if (mTree)
        mTree->setType(static_cast<dtree::DecisionTreeManager::TreeType>(index));
}

void DtEditorWindow::on_action_Delete_Nodes_triggered()
{
    QList<QGraphicsItem *> selection = mScene->selectedItems();

    if (selection.size() > 0) {
        if (QMessageBox::information(this, tr("Confirm nodes removal"),
                                     tr("You're about to remove the selected nodes and all their descendents. Do you want to proceed?"),
                                     QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes) {

            QList<GraphNodeItem *>nodeitems;
            std::list<boost::shared_ptr<dtree::Node> > nodes;
            foreach (QGraphicsItem * item, selection) {
                GraphNodeItem *nodeitem = dynamic_cast<GraphNodeItem*>(item);
                if (nodeitem) {
                    nodeitems.push_back(nodeitem);
                    nodes.push_back(nodeitem->getNode());
                }
            }
            mScene->removeNodes(nodeitems);
            mTree->removeNodes(nodes);
        }
    }
}

void DtEditorWindow::on_actionUndo_triggered()
{
    undo();
}

void DtEditorWindow::on_actionRedo_triggered()
{
    redo();
}
