#include "editpalettedialog.h"
#include "ui_editpalettedialog.h"
#include "palettemanager.h"

#include <QAbstractItemModel>
#include <QColorDialog>
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>

class PaletteModel : public QAbstractItemModel
{
public:
    explicit PaletteModel (Palette *palette, QObject * parent = 0)
        : QAbstractItemModel(parent),
          values(), colors()
    {
        Palette::Iterator it = palette->begin();
        while (it != palette->end()) {
            values.push_back(it.key());
            colors.push_back(*it);

            ++it;
        }
    }

    QModelIndex index(int row, int column, const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return createIndex(row, column);
    }

    QModelIndex parent(const QModelIndex &child) const {
        Q_UNUSED(child);
        return QModelIndex();
    }

    int rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return values.size();
    }
    int columnCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return 2;
    }

    QVariant data(const QModelIndex &index, int role) const {
        switch (index.column()) {
        case 0:
            if (role != Qt::DisplayRole) return QVariant::Invalid;
            return values[index.row()];
        case 1:
            if (role != Qt::BackgroundColorRole && role != Qt::ForegroundRole) {
                return QVariant::Invalid;
            }
            return QVariant (colors[index.row()]);
        }
        return QVariant::Invalid;
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role) {
        bool ok;
        double v;
        switch (index.column()) {
        case 0:
            if (role != Qt::EditRole) return false;
            v = value.toDouble(&ok);
            if (!ok)
                return false;

            if (index.row() < values.size()-1 && v >= values[index.row() +1])
                return false;
            if (index.row() > 0 && v <= values[index.row() -1])
                return false;

            values[index.row()] = v;
            emit dataChanged(index,index);
            return true;
        case 1:
            if (role != Qt::BackgroundColorRole && role != Qt::ForegroundRole) {
                return false;
            }
            colors[index.row()] = value.value<QColor>();
            emit dataChanged(index,index);
            return true;
        }
        return false;
    }

    bool insertRow(int row, const QModelIndex &parent) {
        Q_UNUSED(parent);
        double val = ((row > 0 ? values[row-1] : 0.0) +
                      (row < values.size() ? values[row] : (row > 0 ? 2*values[row-1] : 0.0))) /2;

        values.insert(row,val);
        colors.insert(row, QColor());

        return true;
    }

    bool removeRow(int row, const QModelIndex &parent) {
        Q_UNUSED(parent);
        values.removeAt(row);
        colors.removeAt(row);
        return true;
    }

    Qt::ItemFlags flags(const QModelIndex &index) const {
        if (index.column() == 0)
            return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
        return QAbstractItemModel::flags(index);
    }

    void update(Palette *palette) {
        palette->clear();
        for (int i = 0; i < values.size(); ++i) {
            palette->addColor(values[i],colors[i]);
        }
    }

    void refresh() {
        beginResetModel();
        endResetModel();
    }

private:
    QList<double> values;
    QList<QColor> colors;
};


class SpecialPaletteModel : public QAbstractItemModel
{
public:
    SpecialPaletteModel (Palette *palette)
        : mPalette(palette) {
    }

    QModelIndex index(int row, int column, const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return createIndex(row, column);
    }

    QModelIndex parent(const QModelIndex &child) const {
        Q_UNUSED(child);
        return QModelIndex();
    }

    int rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return mPalette->specialColorCount();
    }
    int columnCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return 1;
    }

    QVariant data(const QModelIndex &index, int role) const {
        if (index.row() == 0) {
            switch (role) {
            case Qt::BackgroundColorRole:
                return QVariant (mPalette->specialColor(index.column()));
            case Qt::ForegroundRole: {
                QColor col = mPalette->specialColor(index.column());
                col.setRed(255-col.red());
                col.setGreen(255 - col.green());
                col.setBlue(255 - col.blue());
                return QVariant (col);
                }
            case Qt::DisplayRole:
                return QVariant (index.column());
            }

            return QVariant::Invalid;
        }
        return QVariant::Invalid;
    }

    void refresh() {
        beginResetModel();
        endResetModel();
    }

private:
    Palette *mPalette;
};

QString EditPaletteDialog::mFileFilter = QT_TR_NOOP("Palette files (*.p2c);;All files (*.*)");


EditPaletteDialog::EditPaletteDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::EditPaletteDialog),
      mPalette(0), mModel(0),
      mLock(false)
{
    ui->setupUi(this);
}

EditPaletteDialog::~EditPaletteDialog()
{
    delete ui;
}

void EditPaletteDialog::showSpecials(bool visible)
{
    ui->specialPalette->setVisible(visible);
}

void EditPaletteDialog::linkPalette(Palette *palette)
{
    mPalette = palette;

    ui->palette->setModel(mModel = new PaletteModel(mPalette));
    ui->specialPalette->setModel(mSpecialModel = new SpecialPaletteModel(mPalette));

    updateControlValues();
}

void EditPaletteDialog::updateControlValues()
{
    mLock = true;
//    ui->minVal->setValue(mPalette->getMin());
//    ui->maxVal->setValue(mPalette->getMax());
//    ui->step->setValue(mPalette->getStep());
//    ui->nrCol->setValue(mPalette->colorCount());
    ui->name->setText(mPalette->name());
    mLock = false;
}

void EditPaletteDialog::on_palette_doubleClicked(const QModelIndex &index)
{
    if (index.column() != 1) return;

    QString s1 = ui->palette->model()->data(ui->palette->model()->index(index.row(), 0)).toString();

    QString tit = QString(tr("Select color for values under '%1'"))
            .arg(s1);
    QColor col = ui->palette->model()->data(index, Qt::BackgroundColorRole).value<QColor>();
    col = QColorDialog::getColor(col, this, tit);

    if (col.isValid()) {
        ui->palette->model()->setData(index, col, Qt::BackgroundColorRole);

        emit paletteChanged();
    }
}

void EditPaletteDialog::on_specialPalette_doubleClicked(const QModelIndex &index)
{
    if (index.row() != 0) return;

    QString tit = QString(tr("Select color for special value %1"))
            .arg(index.column());
    QColor col = mSpecialModel->data(index, Qt::BackgroundColorRole).value<QColor>();
    col = QColorDialog::getColor(col, this, tit);

    if (col.isValid()) {
        mPalette->setSpecialColor(index.column(), col);
        mSpecialModel->refresh();

        emit paletteChanged();
    }
}

void EditPaletteDialog::on_btLoad_clicked()
{
    QSettings sets;
    QString startpath = sets.value("palettedir").toString();

    QString fn = QFileDialog::getOpenFileName(this, tr("Select a file to load the palette from"),
                                              startpath, mFileFilter);
    if (!fn.isEmpty()) {
        QFile f(fn);

        if (!f.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, tr("Error loading file"),
                                 QString(tr("Can't load the selected file: %1")).arg(f.errorString()));
            return ;
        }

        if (!mPalette->loadFromFile(&f)) {
            QMessageBox::warning(this, tr("Error loading file"),
                                 tr("Error loading the file. Maybe the file isn't a Prelude 2 Palette file, or it's corrupted."));
            return;
        }

        mModel->refresh();
        mSpecialModel->refresh();
        updateControlValues();

        QFileInfo info(fn);
        sets.setValue("palettedir", info.absolutePath());
    }
}

void EditPaletteDialog::on_btSave_clicked()
{
    QSettings sets;
    QString startpath = sets.value("palettedir").toString();

    QString fn = QFileDialog::getSaveFileName(this, tr("Select a file to save the palette to"),
                                              startpath, mFileFilter);

    if (!fn.isEmpty()) {
        QFile f(fn);

        if (!f.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, tr("Error saving file"),
                                 QString(tr("Can't save the selected file: %1")).arg(f.errorString()));
            return ;
        }

        if (!mPalette->saveToFile(&f)) {
            QMessageBox::warning(this, tr("Error loading file"),
                                 QString(tr("Operation failed when saving the file.")));
            return ;
        }

        QFileInfo info(fn);
        sets.setValue("palettedir", info.absolutePath());
    }
}

void EditPaletteDialog::on_name_textChanged(const QString &arg1)
{
    mPalette->setName(arg1);
}

void EditPaletteDialog::on_add_clicked()
{
    QModelIndex i = ui->palette->currentIndex();
    int p = (i.isValid() ? i.row() : mModel->rowCount(QModelIndex()));
    mModel->insertRow(p, QModelIndex());
    mModel->refresh();
}

void EditPaletteDialog::on_remove_clicked()
{
    QModelIndex i = ui->palette->currentIndex();
    if (!i.isValid())
        return;

    mModel->removeRow(i.row(), QModelIndex());
    mModel->refresh();
}

void EditPaletteDialog::on_pushButton_clicked()
{
    mModel->update(mPalette);
    QDialog::accept();
}
