#ifndef ADDNODEMOUSEMODE_H
#define ADDNODEMOUSEMODE_H

#include <mousemode.h>

#include <QObject>
#include <QPointF>
#include <QList>
#include <QDialog>

/**
 * @brief The SingleClickMouseMode class implements a mouse mode that completes once a point on the map is
 * clicked, by emitting the modeCompleted(QPointF) signal.
 * The class can be decorated using the MoveVilteringMouseModeDecorator.
 */
class SingleClickMouseMode : public MouseMode
{
    Q_OBJECT
public:
    explicit SingleClickMouseMode(QString description);

    virtual ~SingleClickMouseMode();

    /** \brief signals a mouse release event.
     * \return false if the mouse mode must be aborted, true otherwise
     * */
    virtual bool releaseEvent(const QPointF &point);

    /** \brief Completes the mouse mode
     * \return false if the mouse mode must be aborted, true otherwise
     * */
    virtual bool endMode(bool success);

    QString getModeDescription() const;
signals:
    void modeCompleted(QPointF);

protected:
    QString mDescription;
};

#endif // ADDNODEMOUSEMODE_H
