#ifndef DRAWPENALTYPOLYGON_H
#define DRAWPENALTYPOLYGON_H

#include <mousemode.h>
#include <editorlayerinterface.h>

#include <QPointF>
#include <QList>
#include <QDialog>

class MainWindow;

class DrawPenaltyPolygon : public MouseMode
{
public:
    explicit DrawPenaltyPolygon(MainWindow *win, EditorLayerInterface *ifc);

    ~DrawPenaltyPolygon();

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

    QString getModeDescription() const;
private:
    MainWindow *mMainWindow;
    EditorLayerInterface *mLayerInterface;
    QDialog *mActionDialog;

    bool mPressed;
    QList<QPointF> mPoints;
};

#endif // DRAWPENALTYPOLYGON_H
