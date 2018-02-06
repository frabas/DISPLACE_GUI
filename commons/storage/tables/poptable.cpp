#include "poptable.h"

#include "Node.h"
#include "sqlitestatement.h"
#include "sqlitestatementformatters.h"
#include "sqlitefieldsop.h"

struct PopTable::Impl {
    std::mutex mutex;
    bool init = false;

    PreparedInsert<FieldDef<FieldType::Integer>,FieldDef<FieldType::Integer>,FieldDef<FieldType::Integer>,
        FieldDef<FieldType::Real>,FieldDef<FieldType::Real>> statement;
};

PopTable::PopTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db,name), p(std::make_unique<Impl>())
{
}

PopTable::~PopTable() noexcept = default;

void PopTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());

    create(std::make_tuple(fldNodeId,
                           fldTStep,
                           fldPopId,
                           fldTotNId,
                           fldTotWId
                           ));
}

void PopTable::insert(int tstep, Node *node, const std::multimap<int, double> &weight_at_szgroup)
{
    std::unique_lock<std::mutex> m(p->mutex);
    if (!p->init) {
        p->init = true;
        p->statement = prepareInsert(std::make_tuple(fldNodeId,
                                                     fldTStep,
                                                     fldPopId,
                                                     fldTotNId,
                                                     fldTotWId));
    }

    double totN_this_pop, totW_this_pop;

    for(unsigned int name_pop = 0; name_pop < node->get_Ns_pops_at_szgroup().size(); name_pop++) {
        totN_this_pop=0;		 // re-init
        totW_this_pop= 0.0;
        auto lower_init_we = weight_at_szgroup.lower_bound(name_pop);
        auto upper_init_we = weight_at_szgroup.upper_bound(name_pop);
        vector<double> w;
        for (auto pos=lower_init_we; pos != upper_init_we; pos++)
            w.push_back(pos->second);

        auto const &ns =node->get_Ns_pops_at_szgroup(name_pop);
        for(unsigned int sz = 0; sz < ns.size(); sz++)
        {
            totN_this_pop+= ns[sz];
            totW_this_pop+= ns[sz] * w.at(sz);
        }

        SQLiteTable::insert (p->statement, std::make_tuple(
                    (int)node->get_idx_node().toIndex(),
                    tstep,
                    (int)name_pop,
                    totN_this_pop,
                    totW_this_pop)
                    );
    }

}

size_t PopTable::getNbPops()
{
    sqlite::statements::Select select(name(), sqlite::op::count(sqlite::op::distinct(fldPopId)));
    sqlite::SQLiteStatement stmt(db(),select);

    size_t v;
    stmt.execute([&stmt, &v](){
        v = stmt.getIntValue(0);
        return true;
    });

    return v;
}
