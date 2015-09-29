# Generate vessels, starting from the param file passed as argument
#
# Usage:
# Rscript gen_vessels.R -- dest_file vid area a_dim param_file

args <- commandArgs(trailingOnly = TRUE)

dest_file     <- args[1]
vid           <- args[2]
met           <- args[3]
harb          <- args[4]
param_file    <- args[5]

 cat(paste("Read the input geodata file...", param_file, "\n")) #=> message sent to the console in the editor.
 #param_file <-    "C:\\Users\\fbas\\Documents\\GitHub\\DISPLACE_GUI\\data\\param_fgrounds.dat"

 x <- read.table(file=param_file, header=TRUE, sep= " ")

## add quarters, and then semesters
 x$quarter            <- quarters(as.POSIXct(x$SI_DATE, tz="GMT"))
 x$semester           <- factor(x$quarter)
 levels(x$semester)   <- c(1,1,2,2)
 x$date               <- strptime(  paste(x$SI_DATE) ,  "%e/%m/%Y" )
 x$day                <- format(strptime(  paste( x$date) , tz='GMT',  "%Y-%m-%d" ), "%j")

##...and slightly modify the name of metiers
 x$LE_MET_level6 <- factor(x$LE_MET_level6)
 x$LE_MET_level6 <- as.factor(unlist(lapply(strsplit( as.character(x$LE_MET_level6), split="_"),
                                  function(x) paste(x[1],'_',x[2],sep=''))))   # remove mesh size and useless selective grid info
## get metier_names
 metier_names    <- read.table(file="C:\\Users\\fbas\\Documents\\GitHub\\DISPLACE_GUI\\data\\metier_names_myfish.dat", header=TRUE, sep= " ")

## NEED JUST INTEGERS! for c++, 
 levels(x$LE_MET_level6) <- metier_names [match(levels(x$LE_MET_level6), metier_names[,1]), 2]

 # then, aggregate...
 nm           <- names(x)
 idx.col.w    <- grep('KG', nm) # index columns with species weight
 idx.col.e    <- grep('LE_EFF_VMS', nm)
 x$LE_EFF_VMS <- as.numeric(as.character(x$LE_EFF_VMS))
 idx.col      <- c(idx.col.w, idx.col.e)

head(x)

##write.table(ts_for_a_vid, file=dest_file, row.names=FALSE, col.names=FALSE, sep=" ")
