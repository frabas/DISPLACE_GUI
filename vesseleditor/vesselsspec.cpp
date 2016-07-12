#include "vesselsspec.h"

#include <utils/MultifieldReader.h>

#include <QMessageBox>
#include <QObject>
#include <QDebug>

VesselsSpec::VesselsSpec()
{

}

bool VesselsSpec::loadFromSpecFile(std::istream &is)
{
    displace::formats::helpers::MultifieldReader reader;

    bool res = false;

    try {
        // skip the first line
        std::string empty;
        std::getline(is,empty);

        res = reader.importFromStream<RecordTuple>(is, ",", [this](RecordTuple t) {
        Record rec;
        RecordTupleRef r = recordToTuple(rec);
        r = t;
        mVesselsSpec.push_back(rec);
    },1);
    } catch (std::exception &x) {
        qWarning() << "Cannot read spec file: " << x.what();
    }

    if (!res) {
        QMessageBox::warning(nullptr, QObject::tr("Failed loading vessels spec"),
                             QObject::tr("An error occurred loading the vessels spec file."));
        return false;
    }

    update();

    return true;
}

void VesselsSpec::update()
{
    auto it = mUpdateList.begin();
    while (it != mUpdateList.end()) {
        if (std::get<0>(*it).expired()) {
            it = mUpdateList.erase(it);
        } else {
            std::get<1>(*it)();
        }
    }
}
