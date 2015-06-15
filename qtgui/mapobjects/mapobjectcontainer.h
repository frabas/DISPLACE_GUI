#ifndef MAPOBJECTCONTAINER_H
#define MAPOBJECTCONTAINER_H

#include <QList>
#include <QMap>

template <typename Obj>
class MapObjectContainer
{
    class ObjC {
    public:
        QList<Obj *> mList;
    };

    QMap<int, ObjC *> mMap;

public:
    MapObjectContainer()
        : mMap() {
    }

    ~MapObjectContainer() {
    }

    void add (int id, Obj *object, int role) {
        typename QMap<int,ObjC *>::iterator it = mMap.find(id);
        ObjC *cont;
        if (it == mMap.end()) {
            // not found: add it
            cont = new ObjC;
            mMap.insert(id, cont);
        } else {
            cont = *it;
        }

        while (cont->mList.size() <= role)
            cont->mList.push_back(0);
        cont->mList[role] = object;
    }

    Obj *get(int id, int role) {
        typename QMap<int,ObjC *>::iterator it = mMap.find(id);
        if (it == mMap.end()) {
            return nullptr;
        }

        ObjC *obj = *it;
        if (obj->mList.size() <= role)
            return nullptr;
        return obj->mList.at(role);
    }

    void clear() {
        mMap.clear();
    }

    typedef typename QMap<int,ObjC *>::const_iterator Iterator;

    Iterator begin() {
        return mMap.constBegin();
    }

    bool atEnd(Iterator it) {
        return it == mMap.constEnd();
    }

    Obj *get (Iterator it, int role) {
        if (atEnd(it))
            return nullptr;

        ObjC *obj = *it;
        if (obj->mList.size() <= role)
            return nullptr;

        return obj->mList.at(role);
    }
};

#endif // MAPOBJECTCONTAINER_H
