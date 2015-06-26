                              

  general <- list()
  general$main.path      <- file.path("C:","Users","fbas","Documents","GitHub","DISPLACE_R_inputs")
  general$main.path.data <- file.path("C:","Users","fbas","Documents","GitHub","DISPLACE_input")
  general$main.path.data2 <- file.path("C:","Users","fbas","Documents","GitHub","DISPLACE_input_raw")
  
  general$igraph <- 11 # for the balticonly
  general$case_study <- "balticonly"
  general$case_study2 <- "baltic_only"
  general$a.year <- "2012"
  general$a.country <- "DEN"


##------------------------------------------------------------------------------##
##------------------------------------------------------------------------------##
##---------------------WIND SPEED-----------------------------------------------##
##------------------------------------------------------------------------------##
##------------------------------------------------------------------------------##


# 1. load wind speed from  hhttp://www.dmi.dk/laer-om/generelt/dmi-publikationer/2013/
 windspeeds             <- read.table(file=file.path('C:','Users','fbas','Documents','GitHub','DISPLACE_input_raw',
                            paste("daily_20x20km_wind speed_2010.txt",sep='')), header=TRUE, sep=";")
 windspeeds$day         <- sprintf("%02d",  as.numeric(as.character(windspeeds$day)))
 windspeeds$month       <- sprintf("%02d",  as.numeric(as.character(windspeeds$month)))
 windspeeds$the_day     <-   strptime(  paste( windspeeds$year,"-",  windspeeds$month, "-", windspeeds$day, sep="") , tz='GMT',  "%Y-%m-%d" )    
 windspeeds$day2        <-   format(strptime(  paste( windspeeds$the_day) , tz='GMT',  "%Y-%m-%d" ), "%j")


 rg_wspeed <- matrix(0, ncol=5, nrow=12)
 months    <- c('01', '02', '03', '04', '05', '06', '07', '08', '09', '10', '11', '12')
 for(m in 1 : length(months)) {
   rg_wspeed[m,]                  <- quantile(windspeeds[windspeeds$month==months[m], "X20_20km_Mean_wind_speed_m_s"],
                                             probs=c(0,0.25,0.50,0.75,1))
 }

 #or....
 #rg_wspeed <- matrix(c(3.000, 1.200, 0.800, 3.200, 1.700, 1.400, 2.400, 2.500, 2.300, 1.800, 1.700, 1.500,
 #5.875, 5.400, 5.300, 5.000, 3.800, 3.500, 3.800, 3.600, 4.200, 6.500, 4.200, 6.100,
 #7.300, 6.600, 6.200, 6.200, 5.800, 4.600, 4.600, 5.200, 7.700, 8.050, 7.000, 7.900,
 #9.200, 8.200, 7.100, 7.000, 8.300, 6.200, 5.500, 6.500, 9.900, 9.600, 9.700, 9.100,
 #17.500, 12.700, 11.600, 9.000, 12.600, 12.500, 10.200, 14.000, 13.300, 11.400, 14.200, 13.300),
 #ncol=5, nrow=12)


##------------------------------------------------------------------------------##
##------------------------------------------------------------------------------##
##----------------------FISH PRICE----------------------------------------------##
##------------------------------------------------------------------------------##
##------------------------------------------------------------------------------##

# M:\JSV\FBA_price_data.
# It is by size category.
# I have calculated a price by sales slip, and then calculated the mean price by sales harbor, date species and size sorting.
# I have summarized the landings (kg) and value (dkk) by the same categories, so it should be possible for you to do the weighting.
# I have included the UK species names and the FAO code, and the harbor names, position, eucode and nation.


 # read data
 a.year       <- "2012"
 prices       <- read.table(file=file.path('C:','Users','fbas','Documents','GitHub','DISPLACE_input_raw',
                            paste("price_data2.csv",sep='')), header=TRUE, sep=",")

 # the time dimension
 prices$date2       <- strptime(  paste(prices$date) ,  "%e/%m/%Y" )
 prices$day         <-   format(strptime(  paste( prices$date2) , tz='GMT',  "%Y-%m-%d" ), "%d")
 prices$day2        <-   format(strptime(  paste( prices$date2) , tz='GMT',  "%Y-%m-%d" ), "%j")
 prices$month       <-   format(strptime(  paste( prices$date2) , tz='GMT',  "%Y-%m-%d" ), "%m")
 prices$year        <-   format(strptime(  paste( prices$date2) , tz='GMT',  "%Y-%m-%d" ), "%Y")
 prices$quarter     <- quarters(prices$date2)


 # subset for 2012
 prices <- prices[prices$year==a.year,]

 # get rid of NA
 prices <- prices[!is.na(prices$harbor_lon),]
 prices <- prices[!is.na(prices$harbor_lat),]

 # find out areas
 source(file=file.path(general$main.path,"IBM_param_utils_longlat_to_ICESareas.r"))
 coor <- data.frame(
                    x= as.numeric(as.character(prices[,'harbor_lon'])),
                    y= as.numeric(as.character(prices[,'harbor_lat']))
                    )
 coor$area   <- longlat_to_ICESareas(coor)
 coor[is.na(coor$area) | !(coor$area %in% c('22', '23', '24', '25', '26', '27', '28-1', '28-2', '29', '30', '31', '32',
                         'IIIan', 'IIIas') ), 'area'] <- 'nsea' # if out of range, assign all these nodes to North Sea stocks...
 coor[(coor$area %in% c('22', '23', '24')), 'area'] <- '2224'
 coor[(coor$area %in% c('25', '26', '27', '28-1', '28-2', '29','30', '31', '32')), 'area'] <- '2532'

 prices$stock <- paste( prices$species_uk_code, ".", coor$area, sep='')

 # correct names for special cases (those across management areas)
 prices[prices$stock %in% c("COD.IIIan"), "stock"] <- 'COD.nsea'
 prices[prices$stock %in% c("COD.IIIas"), "stock"] <- 'COD.kat'
 prices[prices$stock %in% c("HAD.IIIan"), "stock"] <- 'HAD.nsea'
 prices[prices$stock %in% c("HER.IIIan", "HER.IIIas", "HER.2224"), "stock"] <- 'HER.3a22'
 prices[prices$stock %in% c("SPR.2224", "SPR.2532"), "stock"] <- 'SPR.2232'
 prices[prices$stock %in% c("PLE.2224", "PLE.2532"), "stock"] <- 'PLE.2232'
 prices[prices$stock %in% c("FLE.2224", "FLE.2532"), "stock"] <- 'FLE.2232'
 prices[prices$stock %in% c("TUR.2224", "TUR.2532"), "stock"] <- 'TUR.2232'
 prices[prices$stock %in% c("DAB.2224", "DAB.2532"), "stock"] <- 'DAB.2232'
 prices[grep('IIIa', prices$stock), "stock"] <- # for all other species, correct kask
    paste( prices[grep('IIIa', prices$stock),'species_uk_code'], ".kask", sep='')

 # subset for relevant populations
 pop_names                   <- read.table(file.path(general$main.path.data, "popsspe_balticonly", paste("pop_names_baltic_only.txt", sep='')))
 prices                 <- prices[prices$stock %in% pop_names[,1],] # keep simulated stocks only
 prices$stock           <- factor(prices$stock)
 levels(prices$stock )  <- pop_names[,2][ match(levels(prices$stock), as.character(pop_names[,1]))] # map the name to integer
 prices$mapped_stk_code <- as.numeric(as.character(prices$stock))
 library(doBy)
 prices                 <- orderBy(~stock, data=prices) # library(doBy) # order from 0 to nbstock


 # subset for the best size category only
 min_cats                <- unlist(lapply(split(prices, f= prices$stock), function(x) min(x[,'size_sorting'], na.rm=TRUE)))
 cats_to_keep            <- paste(names(min_cats),".",min_cats, sep="")
 prices_best_size_cat    <- prices[,c('harbor_name', 'month', 'stock', 'size_sorting', 'harbor_lon', 'harbor_lat', 'date2', 'price')]
 prices_best_size_cat$sp_size_sorting <-  paste(prices_best_size_cat[,'stock'],".",prices_best_size_cat[,'size_sorting'], sep="")
 prices_best_size_cat    <- prices_best_size_cat[prices_best_size_cat[,'sp_size_sorting'] %in% cats_to_keep,]

 
 rg_fishprice <- NULL
 months    <- c('01', '02', '03', '04', '05', '06', '07', '08', '09', '10', '11', '12')
 for(st in levels(prices_best_size_cat$stock)) {
    rg_fishprice[[st]] <- matrix(0, ncol=5, nrow=12)
    for(m in 1 : length(months)) {
       rg_fishprice[[st]][m,]                  <- quantile(prices_best_size_cat[prices_best_size_cat$month==months[m] & prices_best_size_cat$stock==st, "price"],
                                             probs=c(0,0.25,0.50,0.75,1))
    }
 }

 
 #or...
 #rg_fishprice <- matrix(c(3.000,  1.200,  0.800,  3.200,  1.700,  1.400,  2.400,  2.500,  2.300,  1.800,  1.700,  1.500,  
 #        5.875,  5.400,  5.300,  5.000,  3.800,  3.500,  3.800,  3.600,  4.200,  6.500,  4.200,  6.100, 
 #        7.300,  6.600,  6.200,  6.200,  5.800,  4.600,  4.600,  5.200,  7.700,  8.050,  7.000,  7.900,  
 #        9.200,  8.200,  7.100,  7.000,  8.300,  6.200,  5.500,  6.500,  9.900,  9.600,  9.700,  9.100,
 #        17.500, 12.700, 11.600,  9.000, 12.600, 12.500, 10.200, 14.000, 13.300, 11.400, 14.200, 13.300),
 #         ncol=5, nrow=12)


##------------------------------------------------------------------------------##
##------------------------------------------------------------------------------##
##-------------------------FUEL PRICE-------------------------------------------##
##------------------------------------------------------------------------------##
##------------------------------------------------------------------------------##

 fakefuelprice <- expand.grid(month=months, fuelprice=c(0.45,0.50,0.53,0.53,0.53,0.55,0.60))
 

 rg_fuelprice <- matrix(0, ncol=5, nrow=12)
 months    <- c('01', '02', '03', '04', '05', '06', '07', '08', '09', '10', '11', '12')
 for(m in 1 : length(months)) {
   rg_fuelprice[m,]                  <- quantile(fakefuelprice[fakefuelprice$month==months[m], "fuelprice"],
                                             probs=c(0,0.25,0.50,0.75,1))
 }

#or ...

 #rg_fuelprice <- matrix(c(3.000, 1.200, 0.800, 3.200, 1.700, 1.400, 2.400, 2.500, 2.300, 1.800, 1.700, 1.500,
 #5.875, 5.400, 5.300, 5.000, 3.800, 3.500, 3.800, 3.600, 4.200, 6.500, 4.200, 6.100,
 #7.300, 6.600, 6.200, 6.200, 5.800, 4.600, 4.600, 5.200, 7.700, 8.050, 7.000, 7.900,
 #9.200, 8.200, 7.100, 7.000, 8.300, 6.200, 5.500, 6.500, 9.900, 9.600, 9.700, 9.100,
 #17.500, 12.700, 11.600, 9.000, 12.600, 12.500, 10.200, 14.000, 13.300, 11.400, 14.200, 13.300),
 #ncol=5, nrow=12)


##------------------------------------------------------------------------------##
##------------------------------------------------------------------------------##
##--------------------------PARAM_TS--------------------------------------------##
##------------------------------------------------------------------------------##
##------------------------------------------------------------------------------##

param_ts <- rbind.data.frame(
cbind.data.frame (value=as.vector(t(rg_wspeed)), limit=rep(c("0", "25", "50", "75", "max"), length=length(as.vector(t(rg_wspeed)))), month=rep(months,each=5), area="all_area",variable="wspeed", a_dim=0, a_min=0, a_max=18, a_res=0.1),
cbind.data.frame (value=as.vector(t(rg_fuelprice)), limit=rep(c("0", "25", "50", "75", "max"), length=length(as.vector(t(rg_fuelprice)))), month=rep(months,each=5), area="all_area",variable="fuelprice", a_dim=0, a_min=0.4, a_max=1, a_res=0.05)
)

for(st in 1: length(rg_fishprice)){
a_stock <- names(rg_fishprice)[st]
param_ts <- rbind.data.frame( param_ts,
  cbind.data.frame (value=as.vector(t(rg_fishprice[[a_stock]])), limit=rep(c("0", "25", "50", "75", "max"), length=length(as.vector(t(rg_fishprice[[a_stock]])))),  month=rep(months,each=5), area="all_area",variable="fishprice", a_dim=a_stock, a_min=min(rg_fishprice[[a_stock]]), a_max=max(rg_fishprice[[a_stock]]), a_res=0.1)
 )
}

write.table(param_ts, file=file.path('C:','Users','fbas','Documents','GitHub','DISPLACE_input', 'externalforcing_balticonly', 'param_timeseries.dat'),
                 row.names=FALSE, col.names=TRUE, sep= " ", quote=FALSE)


##------------------------------------------------------------------------------##
##------------------------------------------------------------------------------##
##--------------------------EXAMPLE OF USE--------------------------------------##
##------------------------------------------------------------------------------##
##------------------------------------------------------------------------------##


param_ts <- read.table(file=file.path('C:','Users','fbas','Documents','GitHub','DISPLACE_input', 'externalforcing_balticonly', 'param_timeseries.dat'),
                 header=TRUE, sep= " ")


daily_ts_generator <- function (param_ts, variable="wspeed", area="all_area", a_dim=0){

   relevant_param <- param_ts[param_ts$variable==variable & param_ts$area==area & param_ts$a_dim==a_dim,]
   some_quantiles <- matrix( 
                  relevant_param [, "value"]
                  ,
                  ncol=5, nrow=12, byrow=TRUE)

    a_min   <- relevant_param [1, "a_min"]
    a_max   <- relevant_param [1, "a_max"]
    a_res   <- relevant_param [1, "a_res"]
    variable <- seq(a_min,a_max, by=a_res)
  
   ts_for_a_variable <- NULL
   nbdays <- c(31, 28,31, 30, 31, 30, 31, 31, 30, 31, 30, 31)
   months <- c('01', '02', '03', '04', '05', '06', '07', '08', '09', '10', '11', '12')
   for(m in 1 : length(months)) {
   lg1 <- length(which(variable <= some_quantiles[m,2])==TRUE)
   lg2 <- length(which(variable <= some_quantiles[m,3] & variable > some_quantiles[m,2])==TRUE)
   lg3 <- length(which(variable <= some_quantiles[m,4] & variable > some_quantiles[m,3])==TRUE)
   lg4 <- length(which(variable <= some_quantiles[m,5] & variable > some_quantiles[m,4])==TRUE)
   lg5 <- length(which(variable > some_quantiles[m,5] )==TRUE)
   prob_quartile1 <- rep(0.25 /  lg1, each=lg1)
   prob_quartile2 <- rep(0.25 /  lg2, each=lg2)
   prob_quartile3 <- rep(0.25 /  lg3, each=lg3)
   prob_quartile4 <- rep(0.25 /  lg4, each=lg4)
   prob_quartile5 <- rep(0, each=lg5)
   ts_for_a_variable <- c(
          ts_for_a_variable, sample(variable, size=nbdays[m], replace=TRUE, 
                        prob=c(prob_quartile1, prob_quartile2, prob_quartile3, prob_quartile4, prob_quartile5))
          )
   }
   return(ts_for_a_variable)
}

ts_for_a_variable <- daily_ts_generator(param_ts, variable="wspeed", area="all_area", a_dim=0)
plot(ts_for_a_variable, type="l")
abline(h=5, col=2)
abline(h=10, col=2)

 ts_for_a_variable <- daily_ts_generator(param_ts, variable="fuelprice", area="all_area", a_dim=0)
plot(ts_for_a_variable, type="l")
abline(h=5, col=2)
abline(h=10, col=2)

ts_for_a_variable <- daily_ts_generator(param_ts, variable="fishprice", area="all_area", a_dim=10)
plot(ts_for_a_variable, type="l")
abline(h=5, col=2)
abline(h=10, col=2)
