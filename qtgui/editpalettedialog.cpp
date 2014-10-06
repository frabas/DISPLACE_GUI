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
    PaletteModel (Palette *palette)
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
        return mPalette->colorCount();
    }
    int columnCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return 3;
    }

    QVariant data(const QModelIndex &index, int role) const {
        switch (index.column()) {
        case 0:     // From
            if (role != Qt::DisplayRole) return QVariant::Invalid;
            if (index.row() == 0)       // first row
                return QString(tr("Less than"));
            return QString::number(mPalette->getMin() + (index.row()-1) * mPalette->getStep());
        case 1:
            if (role != Qt::DisplayRole) return QVariant::Invalid;
            if (index.row() == mPalette->colorCount()-1)
                return QString(tr("More than"));
            return QString::number(mPalette->getMin() + index.row() * mPalette->getStep());
        case 2:
            if (role != Qt::BackgroundColorRole && role != Qt::ForegroundRole) {
                return QVariant::Invalid;
            }
            return QVariant (mPalette->color(index.row()));
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
    ui->minVal->setValue(mPalette->getMin());
    ui->maxVal->setValue(mPalette->getMax());
    ui->step->setValue(mPalette->getStep());
    ui->nrCol->setValue(mPalette->colorCount());
    ui->name->setText(mPalette->name());
    mLock = false;
}

void EditPaletteDialog::on_palette_doubleClicked(const QModelIndex &index)
{
    if (index.column() != 2) return;

    QString s1 = ui->palette->model()->data(ui->palette->model()->index(index.row(), 0)).toString();
    QString s2 = ui->palette->model()->data(ui->palette->model()->index(index.row(), 1)).toString();

    QString tit = QString(tr("Select color for values '%1' and '%2'"))
            .arg(s1).arg(s2);
    QColor col = ui->palette->model()->data(index, Qt::BackgroundColorRole).value<QColor>();
    col = QColorDialog::getColor(col, this, tit);

    if (col.isValid()) {
        mPalette->setColor(index.row(), col);
        mModel->refresh();

        emit paletteChanged();
    }
}

void EditPaletteDialog::on_minVal_valueChanged(double arg1)
{
    if (mLock) return;
    mLock = true;

    mPalette->setMin(arg1);
    ui->step->setValue(mPalette->getStep());
    ui->nrCol->setValue(mPalette->colorCount());
    mModel->refresh();
    emit paletteChanged();
    mLock = false;
}

void EditPaletteDialog::on_maxVal_valueChanged(double arg1)
{
    if (mLock) return;
    mLock = true;

    mPalette->setMax(arg1);
    ui->step->setValue(mPalette->getStep());
    ui->nrCol->setValue(mPalette->colorCount());
    mModel->refresh();
    emit paletteChanged();

    mLock = false;
}

void EditPaletteDialog::on_nrCol_valueChanged(int arg1)
{
    if (mLock) return;
    mLock = true;

    mPalette->setNumColor(arg1);
    ui->step->setValue(mPalette->getStep());
    mModel->refresh();
    emit paletteChanged();
    mLock = false;
}

void EditPaletteDialog::on_step_valueChanged(double arg1)
{
    if (mLock) return;
    mLock = true;

    mPalette->setStep(arg1);
    ui->nrCol->setValue(mPalette->colorCount());
    mModel->refresh();
    emit paletteChanged();
    mLock = false;
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
