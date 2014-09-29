#include "dbhelper.h"

#include <Vessel.h>

#include <QStringList>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

const QString DbHelper::TBL_VESSELS = "VesselsNames";
const QString DbHelper::TBL_VESSELS_POS = "VesselsPos";

DbHelper::DbHelper()
    : mDb(),
      mOngoingTransaction(false)
 //     mLastStep(-1)
{
    mInsertThread = new QThread();
}

bool DbHelper::attachDb(QString file)
{
    mDb = QSqlDatabase::addDatabase("QSQLITE");
    mDb.setDatabaseName(file);

    if (!mDb.open()) {
        return false;
    }

    // prepare helpers query
    // check tables
    checkVesselsPosTable();
    checkVesselsTable();

    mInserter = new VesselPositionInserter(this, &mDb);

    connect (this, SIGNAL(postVesselInsertion(int,int,double,double,double,int)), mInserter, SLOT(addVesselPosition(int,int,double,double,double,int)));

    mInserter->moveToThread(mInsertThread);
    mInsertThread->start();

    return true;
}

void DbHelper::addVesselPosition(int step, int idx, Vessel *vessel)
{
    emit postVesselInsertion(step, idx, vessel->get_x(), vessel->get_y(), vessel->get_fuelcons(), vessel->get_state());
}

void DbHelper::removeAllVesselsDetails()
{
    QSqlQuery q;
    bool res = q.exec("DELETE FROM " + TBL_VESSELS);
    Q_ASSERT(res);
}

void DbHelper::addVesselDetails(int idx, Vessel *vessel)
{
    bool res;

    mDetailsVesselInsertionQuery->addBindValue(idx);
    mDetailsVesselInsertionQuery->addBindValue(QString::fromUtf8(vessel->get_name().c_str()));
    res = mDetailsVesselInsertionQuery->exec();
    Q_ASSERT_X(res, __FUNCTION__, mDetailsVesselInsertionQuery->lastError().text().toStdString().c_str());
}

void DbHelper::beginTransaction()
{
    mDb.transaction();
    mOngoingTransaction = true;
}

void DbHelper::endTransaction()
{
    mDb.commit();
    mOngoingTransaction = false;
}

bool DbHelper::checkVesselsPosTable()
{
    if (!mDb.tables().contains(TBL_VESSELS_POS)) {
        QSqlQuery q;
        bool r =
        q.exec("CREATE TABLE " + TBL_VESSELS_POS + "("
               + "_id INTEGER AUTO_INCREMENT PRIMARY KEY,"
               + "vesselid INTEGER,"
               + "tstep INTEGER,"
               + "x REAL,"
               + "y REAL,"
               + "fuel REAL,"
               + "state INTEGER,"
               + "cumcatches REAL,"
               + "timeatsea REAL,"
               + "reason_to_go_back INTEGER"
               + ");"
               );

        Q_ASSERT_X(r, __FUNCTION__, q.lastError().text().toStdString().c_str());
    }

    return true;
}

bool DbHelper::checkVesselsTable()
{
    bool r;
    if (!mDb.tables().contains(TBL_VESSELS)) {
        QSqlQuery q;
        r =
        q.exec("CREATE TABLE " + TBL_VESSELS + "("
               + "_id INTEGER PRIMARY KEY,"
               + "name VARCHAR(16)"
               + ");"
               );

        Q_ASSERT_X(r, __FUNCTION__, q.lastError().text().toStdString().c_str());
    }

    mDetailsVesselInsertionQuery = new QSqlQuery;
    r = mDetailsVesselInsertionQuery->prepare("INSERT INTO " +TBL_VESSELS
                    + "(_id,name) VALUES(?,?);"
                    );
    Q_ASSERT_X(r, __FUNCTION__, mDetailsVesselInsertionQuery->lastError().text().toStdString().c_str());

    return true;

}


VesselPositionInserter::VesselPositionInserter(DbHelper *helper, QSqlDatabase *db)
    : QObject(),
      mHelper(helper),
      mDb(QSqlDatabase::cloneDatabase(*db, "inserter")),
      mVesselInsertionQuery(0)
{
    mVesselInsertionQuery = new QSqlQuery;
    mVesselInsertionQuery->prepare(
                "INSERT INTO " + DbHelper::TBL_VESSELS_POS
                + "(vesselid,tstep,x,y,fuel,state) VALUES (?,?,?,?,?,?)"
                );

    mDb.open();
}

void VesselPositionInserter::addVesselPosition(int step, int idx, double x, double y, double fuel, int state)
{
    /* if it's a new step, commits the insertion and restart */
    if (mLastStep != step) {
        mHelper->endTransaction();
        mHelper->beginTransaction();
    }

    /* if there's no transaction ongoing, start it */
    if (!mHelper->mOngoingTransaction) {
        mHelper->beginTransaction();
    }

    mLastStep = step;

    mVesselInsertionQuery->addBindValue(idx);
    mVesselInsertionQuery->addBindValue(step);
    mVesselInsertionQuery->addBindValue(x);
    mVesselInsertionQuery->addBindValue(y);
    mVesselInsertionQuery->addBindValue(fuel);
    mVesselInsertionQuery->addBindValue(state);

    if (!mVesselInsertionQuery->exec())
        qDebug() << mVesselInsertionQuery->lastError();
}
