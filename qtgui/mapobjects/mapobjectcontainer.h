#ifndef MAPOBJECTCONTAINER_H
#define MAPOBJECTCONTAINER_H

#include <QList>
#include <QMap>
#include <idtypes.h>

/** \brief A generic Container class form Map Objects
 *
 * */
template <typename Obj, typename Idx = int>
class MapObjectContainer
{
    class ObjC {
    public:
        QList<Obj *> mList;
    };

    QMap<Idx, ObjC *> mMap;

public:
    MapObjectContainer()
        : mMap() {
    }

    ~MapObjectContainer() {
    }

    /** @brief add an object with the specified id and role
     *
     * */
    Idx add (Idx id, Obj *object, int role) {
        if (types::isIdInvalid(id))
            id = Idx(mMap.size());

        typename QMap<Idx,ObjC *>::iterator it = mMap.find(id);
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

        return id;
    }

    Obj *get(Idx id, int role) {
        auto it = mMap.find(id);
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

    typedef typename QMap<Idx,ObjC *>::const_iterator Iterator;

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

    /** @brief remove the object specified by the id and role.
     * The collection identified by id is not removed. Use remove(int) to remove also the list of roles
     * it is responsibility of the caller to de-allocate the pointer returned
     * @return The Pointer to the object removed.
     * */
    Obj *remove (Idx id, int role) {
        auto it = mMap.find(id);
        if (it == mMap.end()) {
            return nullptr;
        }

        Obj *r = 0;
        ObjC *obj = *it;
        swap (obj->mList[role], r);

        return r;
    }

    /** @brief Remove all the objects identified by the id, with all roles.
     * The collection identified by id is removed.
     * It is responsibility of the caller to de-allocate the objects returned with the pointer list.
     * @return A List od pointers to the objects removed.
     * */
    QList<Obj *> remove (Idx id) {
        auto it = mMap.find(id);
        if (it == mMap.end()) {
            return QList<Obj *>();
        }

        ObjC *obj = *it;
        QList<Obj *> r = obj->mList;
        mMap.remove(id);

        return r;
    }
};

#endif // MAPOBJECTCONTAINER_H
