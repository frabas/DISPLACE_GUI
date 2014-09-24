#ifndef WRAPPERWIDGET_H
#define WRAPPERWIDGET_H

#include <QWidget>

namespace qmapcontrol {
class QMapControl;
}

class WrapperWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WrapperWidget(QWidget *parent = 0);

    void setWidget(qmapcontrol::QMapControl *w) {
        widget = w;
    }

signals:

public slots:

protected:
    void resizeEvent(QResizeEvent *event);

private:
    qmapcontrol::QMapControl *widget;
};

#endif // WRAPPERWIDGET_H
