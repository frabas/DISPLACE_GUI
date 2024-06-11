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

#include "dteditorwindow.h"
#include "ui_dteditorwindow.h"
#include <dtree/dtnode.h>
#include <dtree/decisiontreemanager.h>
#include <graphnodeextra.h>
#include <graphnodeitem.h>
#include <dtcsvwriter.h>
#include <dtcsvreader.h>

#include <nodemappingsdialog.h>

#include <commands/settreetypecommand.h>
#include <commands/setnodevaluecommand.h>

#include <QCloseEvent>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <qsignalblocker.h>

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

    createScene(std::shared_ptr<dtree::DecisionTree>(new dtree::DecisionTree));

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

void DtEditorWindow::createScene(std::shared_ptr<dtree::DecisionTree> tree)
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
        std::shared_ptr<Command> command = mUndoList.pop();
        if (command->undo()) {
            mRedoList.push(command);
        }
    }
    updateUndoGuiStatus();
}

void DtEditorWindow::redo()
{
    if (!mRedoList.empty()) {
        std::shared_ptr<Command> command = mRedoList.pop();
        if (command->redo()) {
            mUndoList.push(command);
        }
    }
    updateUndoGuiStatus();
}

void DtEditorWindow::execute(std::shared_ptr<Command> command)
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

    bool retv = false;
    QString errmsg;
    try {
        retv = reader.readTree(strm, mTree, mScene);
    } catch (std::invalid_argument x) {
        errmsg = x.what();
    }

    if (!retv) {
        QString msg;
        if (errmsg.isEmpty()) {
            msg = QString(tr("Cannot load tree file."));
        } else {
            msg = QString(tr("Cannot load tree file: %1")).arg(errmsg);
        }
        QMessageBox::warning(this, tr("Load failed"),msg);
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
        const QSignalBlocker block(ui->treeType);
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
    std::shared_ptr<dtree::Node> newnode = mTree->createNode();
    std::shared_ptr<dtree::GraphNodeExtra> newextra (new dtree::GraphNodeExtra);

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
    ui->actionMappings->setEnabled(selection.size() == 1);

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
            std::shared_ptr<dtree::Node> node = item->getNode();
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

                std::shared_ptr<dtree::NodeExtra> extra = node->extra();
                if (extra.get() != 0) {

                }
            } else {
                qWarning() << "Selected graph " << item << " has null node!";
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
            std::shared_ptr<dtree::Node> node = item->getNode();
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

    std::shared_ptr<SetNodeValueCommand> command(new SetNodeValueCommand(selection, value));
    execute(command);
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
    if (mTree) {
        std::shared_ptr<Command> command (new SetTreeTypeCommand(this, mTree, static_cast<dtree::DecisionTreeManager::TreeType>(index)));
        execute(command);
    }
}

void DtEditorWindow::on_action_Delete_Nodes_triggered()
{
    QList<QGraphicsItem *> selection = mScene->selectedItems();

    if (selection.size() > 0) {
        if (QMessageBox::information(this, tr("Confirm nodes removal"),
                                     tr("You're about to remove the selected nodes and all their descendents. Do you want to proceed?"),
                                     QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes) {

            QList<GraphNodeItem *>nodeitems;
            std::list<std::shared_ptr<dtree::Node> > nodes;
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

void DtEditorWindow::on_actionQuit_triggered()
{
    int r = QMessageBox::question(this, tr("Quit"), tr("Really quit?"),
                                  QMessageBox::No, QMessageBox::Yes);
    if (r == QMessageBox::No)
        return;

    close();
}

void DtEditorWindow::on_actionMappings_triggered()
{
    QList<QGraphicsItem *> selection = mScene->selectedItems();

    if (selection.size() != 1) {
        return;
    }

    GraphNodeItem *item = dynamic_cast<GraphNodeItem *>(selection.at(0));

    if (!item)
        return;

    std::shared_ptr<dtree::Node> node = item->getNode();
    NodeMappingsDialog dlg(node, this);

    if (dlg.exec() == QDialog::Accepted) {
        int n = node->getChildrenCount();

        for (int i = 0; i < n; ++i) {
            node->setMapping(i, dlg.getMappingForIndex(i));
        }

        item->setVariable(node->variable());
    }
}
