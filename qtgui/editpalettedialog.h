#ifndef EDITPALETTEDIALOG_H
#define EDITPALETTEDIALOG_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
    class EditPaletteDialog;
}

class Palette;
class PaletteModel;
class SpecialPaletteModel;

class EditPaletteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditPaletteDialog(QWidget *parent = 0);
    ~EditPaletteDialog();

    void linkPalette (Palette *palette);

signals:
    void paletteChanged();

private slots:
    void on_palette_doubleClicked(const QModelIndex &index);
    void on_minVal_valueChanged(double arg1);
    void on_maxVal_valueChanged(double arg1);
    void on_nrCol_valueChanged(int arg1);
    void on_step_valueChanged(double arg1);
    void on_specialPalette_doubleClicked(const QModelIndex &index);
    void on_btLoad_clicked();
    void on_btSave_clicked();

    void on_name_textChanged(const QString &arg1);

protected:
    void updateControlValues();

private:
    Ui::EditPaletteDialog *ui;
    Palette *mPalette;
    PaletteModel *mModel;
    SpecialPaletteModel *mSpecialModel;
    bool mLock;

    static QString mFileFilter;
};

#endif // EDITPALETTEDIALOG_H
