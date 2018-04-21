#ifndef CACHEDDATASTORAGE_H
#define CACHEDDATASTORAGE_H

#include <dbtypes.h>
#include <modeltypes.h>

template <typename Data>
class CachedDataStorage
{
protected:
    struct Record {
        bool dirty = true;
        types::tstep_t cachedTstep = types::tstep_t(-1);
        std::shared_ptr<Data> data = nullptr;
    };

    std::vector<Record> cachedData;

    Record &getRecord(types::NodeId nodeId) {
        while (cachedData.size() <= nodeId.toIndex())
            cachedData.push_back(Record());
        return cachedData.at(nodeId.toIndex());
    }

    void updateData (types::tstep_t step, Data n) {
        auto &d = getRecord(n.nodeId);
        d.dirty = false;
        d.cachedTstep = step;
        d.data = std::make_shared<Data>(std::move(n));
    }

public:
    CachedDataStorage() {}
    virtual ~CachedDataStorage() noexcept = default;

    void invalidateAllCache() {
        for(auto &e : cachedData) {
            e.data.reset();
            e.dirty = false;
            e.cachedTstep = types::tstep_t(-1);
        }
    }

    virtual void queryAllData(types::tstep_t step) = 0;

    void refreshAllData (types::tstep_t step) {
        // Update all the cache with valid data, but null.
        for(auto &e : cachedData) {
            e.data.reset();
            e.dirty = false;
            e.cachedTstep = step;
        }

        queryAllData(step);
    }

    std::shared_ptr<Data> getData(types::NodeId nodeId, types::tstep_t tstep) {
        auto &d = getRecord(nodeId);

        // it's clean. pass.
        if (!d.dirty && d.cachedTstep == tstep)
            return d.data;

        refreshAllData(tstep);
        // the following code would be better but it's not optimal.
        //p->refreshDataForNode(nodeId, tstep);

        return getRecord(nodeId).data;
    }
};

#endif // CACHEDDATASTORAGE_H
