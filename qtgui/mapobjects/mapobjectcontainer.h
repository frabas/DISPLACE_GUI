#ifndef MAPOBJECTCONTAINER_H
#define MAPOBJECTCONTAINER_H

#include <QList>
#include <QMap>
#include <idtypes.h>
#include <memory>

/** \brief A generic Container class form Map Objects
 *
 * */
template <typename Obj, typename Idx = int>
class MapObjectContainer
{
    class ObjC {
    public:
        QList<std::shared_ptr<Obj>> mList;
    };

    QMap<Idx, std::shared_ptr<ObjC>> mMap;

public:
    MapObjectContainer()
        : mMap() {
    }

    ~MapObjectContainer() {
    }

    /** @brief add an object with the specified id and role
     *
     * */
    Idx add (Idx id, std::shared_ptr<Obj> object, int role) {
        if (types::isIdInvalid(id))
            id = Idx(mMap.size());

        typename QMap<Idx, std::shared_ptr<ObjC>>::iterator it = mMap.find(id);
        std::shared_ptr<ObjC> cont;
        if (it == mMap.end()) {
            // not found: add it
            cont = std::make_shared<ObjC>();
            mMap.insert(id, cont);
        } else {
            cont = *it;
        }

        while (cont->mList.size() <= role)
            cont->mList.push_back(nullptr);
        cont->mList[role] = object;

        return id;
    }

    std::shared_ptr<Obj> get(Idx id, int role) {
        auto it = mMap.find(id);
        if (it == mMap.end()) {
            return nullptr;
        }

        std::shared_ptr<ObjC> obj = *it;
        if (obj->mList.size() <= role)
            return nullptr;
        return obj->mList.at(role);
    }

    void clear() {
        mMap.clear();
    }

    typedef typename QMap<Idx, std::shared_ptr<ObjC>>::const_iterator Iterator;

    Iterator begin() {
        return mMap.constBegin();
    }

    bool atEnd(Iterator it) {
        return it == mMap.constEnd();
    }

    std::shared_ptr<Obj> get (Iterator it, int role) {
        if (atEnd(it))
            return nullptr;

        std::shared_ptr<ObjC> obj = *it;
        if (obj->mList.size() <= role)
            return nullptr;

        return obj->mList.at(role);
    }

    /** @brief remove the object specified by the id and role.
     * The collection identified by id is not removed. Use remove(int) to remove also the list of roles
     * @return The shared_ptr to the object removed.
     * */
    std::shared_ptr<Obj> remove (Idx id, int role) {
        auto it = mMap.find(id);
        if (it == mMap.end()) {
            return nullptr;
        }

        std::shared_ptr<Obj> r = nullptr;
        std::shared_ptr<ObjC> obj = *it;
        std::swap(obj->mList[role], r);

        return r;
    }

    /** @brief Remove all the objects identified by the id, with all roles.
     * The collection identified by id is removed.
     * @return A List of shared_ptr to the objects removed.
     * */
    QList<std::shared_ptr<Obj>> remove (Idx id) {
        auto it = mMap.find(id);
        if (it == mMap.end()) {
            return QList<std::shared_ptr<Obj>>();
        }

        std::shared_ptr<ObjC> obj = *it;
        QList<std::shared_ptr<Obj>> r = obj->mList;
        mMap.remove(id);

        return r;
    }
};

#endif // MAPOBJECTCONTAINER_H
