#include "wrapperwidget.h"

#include <mapcontrol.h>
#include <QResizeEvent>

WrapperWidget::WrapperWidget(QWidget *parent) :
    QWidget(parent), widget (0)
{
}

void WrapperWidget::resizeEvent(QResizeEvent *event)
{
    if (widget) {
        widget->resize(event->size());
    }
}
