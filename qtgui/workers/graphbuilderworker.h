#ifndef GRAPHBUILDERWORKER_H
#define GRAPHBUILDERWORKER_H

#include <backgroundworker.h>
#include <graphbuilder_shp.h>
#include <mainwindow.h>

class GraphBuilderWorker : public BackgroundWorker, public GraphBuilder::Feedback {
    GraphBuilder *builder;
    WaitDialog *waitDialog;
    QList<GraphBuilder::Node> result;

public:
    GraphBuilderWorker (MainWindow *win, GraphBuilder *b, WaitDialog *dlg);

    virtual void execute() override;
    void setMax (int m) override;
    void setStep(int step) override;

    void setMainMessage(QString) override;
    void setPartMessage(QString) override;
};

#endif // GRAPHBUILDERWORKER_H
