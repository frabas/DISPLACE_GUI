#ifndef CSVSPECSPAGE_H
#define CSVSPECSPAGE_H

#include <QWidget>
#include <memory>

namespace Ui {
class CsvSpecsPage;
}

class CsvTableModel;

QT_BEGIN_NAMESPACE
class QSortFilterProxyModel;
class QProcess;
QT_END_NAMESPACE

class CsvSpecsPage : public QWidget
{
    Q_OBJECT

public:
    explicit CsvSpecsPage(QWidget *parent = 0);
    ~CsvSpecsPage();

    void load();
    void save();

    void setFilename(QString n) {
        mFilename = n;
    }

private:
    Ui::CsvSpecsPage *ui;

    QString mFilename;

    std::shared_ptr<QList<QStringList>> mData;
    CsvTableModel *mModel;
    QSortFilterProxyModel *mVesselsSpecProxyModel;

};

#endif // CSVSPECSPAGE_H
