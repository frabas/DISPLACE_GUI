#ifndef MOVEFILTERINGMOUSEMODEDECORATOR_H
#define MOVEFILTERINGMOUSEMODEDECORATOR_H

#include <mousemode.h>

class MoveFilteringMouseModeDecorator : public MouseMode
{
public:
    MoveFilteringMouseModeDecorator(MouseMode *mode);

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
    virtual bool endMode(bool success);

private:
    MouseMode *mMode;
    bool mHasMoved;
    bool mPressed;
};

#endif // MOVEFILTERINGMOUSEMODEDECORATOR_H
