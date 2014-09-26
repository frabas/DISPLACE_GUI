#include "outputfileparser.h"

#include <displacemodel.h>

#include <QFile>
#include <QFileInfo>
#include <QDebug>

#include <QMessageBox>

OutputFileParser::OutputFileParser(MainWindow *win)
    : mOwner(win)
{
}

void OutputFileParser::parse(QString path, DisplaceModel *model)
{
    QFile file (path);
    QFileInfo info (file);

    QString name = info.fileName();
    if (name.startsWith("vmslike_")) {
        qDebug() << "Parsing " << path << " as population node start";

        parsePopStart(file, model);
    }
}


/*
 * PopStart file is a list of space-separated lines containing:
 * nodeid x y population1 population2 ...
 * see Node::export_popnodes for details
 */
void OutputFileParser::parsePopStart(QFile file, DisplaceModel *model)
{
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(mOwner, tr("Error loading output file"),
                             QString(tr("The file %1 cannot be read: %2"))
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return;
    }
    QTextStream strm (&file);

    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ");
        int id = fields[0].toInt();


        //TODO: check if this is ok.
        for (int i = 3 ; i < fields.size(); ++i)
            model->getNodesList()[id]->set_benthos_tot_biomass(i, fields[i].toDouble());
    }
}
