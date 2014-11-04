#ifndef NODEDETAILSWIDGET_H
#define NODEDETAILSWIDGET_H

#include <QWidget>

namespace Ui {
class NodeDetailsWidget;
}

class NodeDetailsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NodeDetailsWidget(QWidget *parent = 0);
    ~NodeDetailsWidget();

    void setText(QString text);
private:
    Ui::NodeDetailsWidget *ui;
};

#endif // NODEDETAILSWIDGET_H
