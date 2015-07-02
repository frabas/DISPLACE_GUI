# Generate Time Series, starting from the param file passed as argument
#
# Usage:
# Rscript gen_ts.R -- dest_file variable area a_dim param_file

args <- commandArgs(trailingOnly = TRUE)

dest_file <- args[1]
variable <- args[2]
area <- args[3]
a_dim <- args[4]
a_dim_as_int <- as.numeric(a_dim)
param_file <- args[5]

param_ts <- read.table(file=param_file, header=TRUE, sep= " ")

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
 
  # ts_for_a_variable <- c(relevant_param[1,c("threshold1", "threshold2", "threshold3")], ts_for_a_variable)
 
   return(ts_for_a_variable)
}

ts_for_a_variable <- daily_ts_generator(param_ts, variable=variable, area=area, a_dim=a_dim_as_int)
write.table(ts_for_a_variable, file=dest_file, row.names=FALSE, col.names=TRUE, sep=" ")
