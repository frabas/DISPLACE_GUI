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

#ifndef DTEDITORWINDOW_H
#define DTEDITORWINDOW_H

#include <QMainWindow>
#include <memory>
#include <dtree/decisiontree.h>
#include <dtgraphicsscene.h>
#include <commands/command.h>
#include <memory>
#include <QStack>

namespace Ui {
class DtEditorWindow;
}

class DtEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DtEditorWindow(QWidget *parent = 0);
    ~DtEditorWindow();

    void updateGui();

protected:
    void save(QString filename);
    void open(QString filename);
    void updateTitleBar();
    bool checkForDTreeBeforeSaving();
    void createScene(std::shared_ptr<dtree::DecisionTree> tree);

    void undo();
    void redo();
    void execute(std::shared_ptr<Command> command);
    void clearUndoRedoStacks();
    void updateUndoGuiStatus();

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
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();

    void on_actionQuit_triggered();

    void on_actionMappings_triggered();

private:
    Ui::DtEditorWindow *ui;

    std::shared_ptr<dtree::DecisionTree> mTree;
    DtGraphicsScene *mScene;

    QStack<std::shared_ptr<Command>> mUndoList, mRedoList;
    QString mFilename;
};

#endif // DTEDITORWINDOW_H
