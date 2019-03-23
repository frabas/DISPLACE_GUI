// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2019 Francois Bastardie <fba@aqua.dtu.dk>

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

/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
#ifndef BACKGROUDWORKER_H
#define BACKGROUDWORKER_H

#include <QObject>
#include <waitdialog.h>

class MainWindow;

class BackgroundWorker : public QObject
{
    Q_OBJECT
public:
    explicit BackgroundWorker(MainWindow *main);

    virtual void execute() = 0;

    QString getError() const { return mError; }
    virtual bool success() const { return mResult; }

signals:
    void workStarted();
    void workEnded();
    void progress(int n);
    void totalChanged(int tot);
    void warning(QString,QString);
    void completed (bool success);
    void mainMessage (QString);
    void progressMessage (QString);

public slots:
    void process();

protected:
    void setFail(QString msg) { mResult = false; mError = msg; }
    void setSuccess() { mResult = true; }

    MainWindow *mMain;
    bool mResult;
    QString mError;
};

class BackgroundWorkerWithWaitDialog : public BackgroundWorker, public AbortListener
{
    Q_OBJECT
public:
    explicit BackgroundWorkerWithWaitDialog(MainWindow *main, WaitDialog *dialog);

    WaitDialog *getWaitDialog() const { return mWaitDialog; }

    bool aborted() const { return mAborted; }

signals:
    void messageChanged(QString);
    void progressBarVisibilityChanged(bool, int);
    void abortButtonVisibilityChanged(bool);

public slots:
    virtual void abortIssued();

protected:
    void setProgressMax(int);
    void setProgress(int);
    void setText(QString);
    void setAbortEnabled(bool);

    WaitDialog *mWaitDialog;
    bool mAborted;
};

#endif // BACKGROUDWORKER_H
