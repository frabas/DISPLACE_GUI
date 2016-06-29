#ifndef GRAPHEXPORTPROPERTIES_H
#define GRAPHEXPORTPROPERTIES_H

#include <QDialog>

namespace Ui {
class GraphExportProperties;
}

struct GraphProperties {
    unsigned int width;
    unsigned int height;
    QString format;
};

class GraphExportProperties : public QDialog
{
    Q_OBJECT

public:
    explicit GraphExportProperties(QWidget *parent = 0);
    ~GraphExportProperties();

    GraphProperties getOptions() const;

private slots:
    void on_width_valueChanged(int arg1);

    void on_height_valueChanged(int arg1);

    void on_aspect_valueChanged(double arg1);

private:
    Ui::GraphExportProperties *ui;

    // QDialog interface
public slots:
    void accept() override;
};

#endif // GRAPHEXPORTPROPERTIES_H
