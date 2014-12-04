#ifndef MOUSEMODE_H
#define MOUSEMODE_H

#include <QObject>
#include <QPointF>
#include <QString>

class MouseModeInterface {
public:
    virtual void showMessage(const QString &) = 0;
};

/**
 * @brief The MouseMode class
 *
 * @note this class can be decorated (and it is), so follow these simple rules:
 * - Data members are duplicated by decorators. So getter and setter must be overloaded
 * to allow pass the decorated version of the object
 * - for this purpose, getter and setter must be declared virtual.
 */
class MouseMode : public QObject
{
    Q_OBJECT
public:
    MouseMode();
    virtual ~MouseMode();

    /** \brief signals a mouse press event.
     * \return false if the mouse mode must be aborted, true otherwise
     * */
    virtual bool pressEvent(const QPointF &point);

    /** \brief signals a mouse release event.
     * \return false if the mouse mode must be aborted, true otherwise
     * */
    virtual bool releaseEvent(const QPointF &point);

    /** \brief signals a mouse move event.
     * \return false if the mouse mode must be aborted, true otherwise
     * */
    virtual bool moveEvent(const QPointF &point);

    /** \brief Starts the mouse mode.
     * \return false if the mouse mode must be aborted, true otherwise
     * */
    virtual bool beginMode();

    /** \brief Completes the mouse mode
     * \return false if the mouse mode must be aborted, true otherwise
     * */
    virtual bool endMode(bool success) = 0;

    virtual QString getModeDescription() const = 0;

    virtual void setMouseModeInterface (MouseModeInterface *);

protected:
    void showMessage(QString msg);

private:
    MouseModeInterface *mGui;
};

#endif // MOUSEMODE_H
