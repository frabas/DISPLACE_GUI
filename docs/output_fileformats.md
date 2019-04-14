## Formats of output files written in /DISPLACE_outputs


Path: ~/ibm_vessels/DISPLACE_outputs/(basepath)/(basename)/


# vmslike_*.dat
tstep / name / tstep_dep / x / y / course / cum_fuel / state


# vmslikefpingsonly_*.dat
tstep / vessel name /  start trip tstep / lon / lat/nodeid / course / cumfuelcons/ pop / catches (i.e. landings+ discards, in weight) szgroup 0 /  szgroup 1 /... / 13


# popstats_*.dat
tstep / stock/ N at szgroup (in thousands, for 14 szgrp)/  W at szgroup / SSB at size group 
N are in thousands of individuals, W and SSB are in kg


# popdyn_annual_indic_*.dat
tstep / stock / oth mult / cpue mult / fbar / tot landings / tot discards / SSB / N at age / F at age /W at age / M at age


# benthosnodes_tot_biomasses_*.dat or benthosnodes_tot_numbers_*.dat
Func gr id/ tstep / node / long / lat / number this func group id /biomass this func group id/ mean weight this func group id / benthosbiomassoverK / benthosnumberoverK /benthos_tot_biomass_K this funcgr 


# popnodes_start_*.dat
tstep / node / long / lat / tot N sp0 / tot W sp0/ tot N sp1 / tot W sp1...
 

# popnodes_inc_*.dat
tstep / node / long / lat / tot N sp0 / tot W sp0/ tot N sp1 / tot W sp1...


# popnodes_impact_*.dat
pop / tstep / node_idx / long / lat / impact_on_pop


# popnodes_cumulcatches_per_pop_*.dat
pop / tstep / node_idx / long / lat / cumcatches (actually landings)


# popnodes_cumftime_*.dat
tstep /  node /  long /   lat /   cumftime


# popnodes_cumsweptarea_*.dat
tstep /  node /  long  /  lat  /  cumsweptarea / subsurfacecumsweptarea


# popnodes_cumcatches_*.dat
tstep / node_idx / long / lat / cumcatches (landings only; unless discard ban)


# popnodes_cumdiscards_*.dat
tstep / node_idx / long / lat / cumdiscards


# popnodes_cumcatches_with_threshold_*.dat
tstep / node_idx / long / lat / cumcatches / threshold in percent


# popnodes_tariffs_*.dat
tstep / node / long / lat /  tariffs


# tripcatchesperszgroup_*.dat
tstep / vessel name / departure tstep / popid / catches (landings only; unless discard ban) szgroup 0 /  szgroup 1 /... / 13


# export_individual_tac_*.dat
tstep / vesselid / pop / a remaining quota this pop / amount discarded if remaining is 0


# loglike_*.dat
0 tstep_dep
1 tstep         Current tstep
2 reason_to_go_back
3 cum_steam
4 loc->nodeidx (Harbour?)
5 idx
6 name
7 timeatsea
8 cumfulecons
9 travel_dist_this_trip
10xN    cumul sz per pop(1..N) (i.e. landings only!)
freq_metiers
revenue
revenue_from_av_prices
revenue_explicit_from_av_prices
fuelcost
gav
gav2 // gav handling energy cost only
sweptarea // from EU FP7 BENTHIS parameterisation
revenuepersweptarea
GVA // gva handling all costs (but need a deeper parameterisation)
GVAPerRevenue
LabourSurplus
GrossProfit
NetProfit
NetProfitMargin
GVAPerFTE
RoFTA
BER
CRBER
NetPresentValue
numTrips

e.g. adding headers in R to the loglike data table:
colnames (loglike) <- c('tstep_dep', 'tstep_arr', 'reason_back','cumsteaming', 'idx_node', 
                        'idx_vessel', 'VE_REF', 'timeatsea', 'fuelcons', 'traveled_dist', 
                         paste('pop.', 0:(general$nbpops-1), sep=''), 
                         "freq_metiers", "revenue", "rev_from_av_prices", "rev_explicit_from_av_prices", 
                         "fuelcost", "vpuf", "gav", "gradva", 
                         "sweptr", "revpersweptarea",
                         paste('disc.',  explicit_pops, sep=''), 
                         "GVA", "GVAPerRevenue", "LabourSurplus", "GrossProfit", "NetProfit", 
                         "NetProfitMargin", "GVAPerFTE", "RoFTA", "BER", "CRBER", "NetPresentValue", "numTrips")   


# fishfarmlogs_*.dat
tstep / node / long / lat / farmtype / farmid / meanw_kg / fish_harvested_kg / eggs_harvested_kg / fishfarm_annualprofit /


# shipslogs_*.dat
tstep / node / long / lat /shiptype / shipid / nb_units / fuel_use_h / NOx_emission_gperkW / SOx_emission_percentpertotalfuelmass / GHG_emission_gperkW / PME_emission_gperkW / fuel_use_litre / NOx_emission / SOx_emission / GHG_emissions / PME_emission


# windmillslogs_*.dat
 tstep / node / long / lat / windfarmtype / windfarmid / kWh / kW_production


# lst_loglike_weight_agg_sce*.RData (obtained with postprocessing R routines)
year.month / pop.0/ pop.1/ pop.2 /…/ disc.0/disc.1/disc.2/…/effort/cumsteaming/nbtrip/bwtrip/fuelcons/
revenue/rev_from_av_prices/sweptr/revpersweptarea/fuelcost/gav/gradva/totland/totland_explicit/
totland_implicit/rev_explicit_from_av_prices /av_effort/av_bwtrip/traveled_dist/
vpuf/vapuf/ disc_rate_0 disc_rate_1/disc_rate_2/…/av_vapuf_month 


# average_cumftime_layer.txt (obtained with postprocessing R routines)
	etc.


# (name of the app)_simu1_out.db 
A SQlite database gathers several tables (including the data in listed text files above) 
which can be read by DISPLACE itself, 
or scrutinized with a SQL Browser e.g. the "DB Browser for SQlite" software



