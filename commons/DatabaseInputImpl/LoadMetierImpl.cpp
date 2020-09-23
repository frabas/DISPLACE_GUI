

#include "DatabaseModelLoader.h"
#include "Impl.h"
#include "SimModel.h"

#include "Metier.h"
#include "MetiersLoader.h"

#include <msqlitecpp/v2/selectstatement.h>
#include <msqlitecpp/v2/fields.h>

namespace db = msqlitecpp::v2;


void DatabaseModelLoader::loadMetiers(int year, int month, int quarter, int semester)
{
    MetiersLoader loader(*p->db);

    auto allMetiers = loader.getListOfAllMetiers();

    std::vector<Metier*> metiersToAdd;
    double idx_metier = -1;

    for (auto& metier : allMetiers) {
        // TODO: in the DB we just have opt1 as "period". We select month here. FIXME.
        auto metierData = loader.getMetierData(metier, month);

        // TODO this is not correct.
        int metier_name = 0;

        auto m = new Metier(
                metier_name,
                metierData->type,
                metierData->percent_revenue_completeness,
                metierData->selectivity_per_stock,
                metierData->betas_per_pop,
                metierData->discardratio_limits,
                metierData->is_avoided_stocks,
                metierData->mls_cat_per_pop,
                metierData->fspeed,
                metierData->gear_width_a,
                metierData->gear_width_b,
         metierData->gear_width_model,
         metierData->loss_after_1_passage,
         metierData->metier_target_stocks,
         metierData->metier_suitable_seabottomtypes
        );
        metiersToAdd.push_back(m);
    }

    model().setMetiers(metiersToAdd);
}
