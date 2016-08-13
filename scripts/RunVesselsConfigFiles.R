
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#-----read input config file----------------------------------------------------
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
# Run with the following arguments:
# 1: Output Path
# 2: Harbours Path
# 3: Application name ("adriatic")
# 4: Path Param  i.e. point to DISPLACE_input_raw
# 5: Path Param GIS point to DISPLACE_input_gis
# 6: Path Input (IBM) i.e. pointing to displace_input_test

   # GENERAL SETTINGS
   cmdlineargs <- list()

   args <- commandArgs(trailingOnly = TRUE)


   cmdlineargs$application           <- args[3]
   cmdlineargs$main.path.param       <- args[4] #file.path("C:","Users","fbas","Documents","GitHub","DISPLACE_input_raw")
   cmdlineargs$main.path.param.gis   <- args[5] #file.path("C:","Users","fbas","Documents","GitHub","DISPLACE_input_gis", cmdlineargs$application)
   cmdlineargs$main.path.ibm         <- args[6] #file.path("C:","Users","fbas","Documents","GitHub",paste("DISPLACE_input_", cmdlineargs$application, sep=''))


   # create from different set of vessels, for example:
   path      <- file.path(cmdlineargs$main.path.param.gis, cmdlineargs$application, "FISHERIES", "vessels_config_files" )
            #was: file.path("C:","Users","fbas","Documents","GitHub","DISPLACE_input_gis", "adriatic", "FISHERIES", "vessels_config_files")
   namefiles <- list.files(file.path( path))

   count <- 0
 for (namefile in namefiles){ # LOOP OVER CONFIG FILES
   count <- count+1

    dat              <- readLines(file.path(path, namefile))

   my_split <- function(x) unlist(strsplit(x, " "))
   my_split2 <- function(x) unlist(strsplit(x, "_"))

   general <- list()
   general$main.path.param.gis   <- as.character(dat[5])
   general$main.path.param       <- as.character(dat[7])
   general$main.path.ibm         <- as.character(dat[9])
   general$namefolderinput       <- as.character(dat[11])
   general$igraph                <- as.numeric(dat[13])

   do_append                     <- as.logical(dat[15])
   if (count==1) do_append <- FALSE # NOT ADDED TO PREVIOUS FILE......


   name_gis_file_for_fishing_effort_per_polygon <- dat[17]
   name_gis_layer_field                        <- dat[19]
   is_gis_layer_field_relative_numbers         <- dat[21]
   xfold_gis_layer_field                        <- as.numeric(my_split(dat[23]))
   vesselids                     <- as.character(my_split(dat[25]))
   vessel_range_km               <- as.numeric(dat[27])
   metierids                     <- as.numeric(my_split(dat[29]))
   metierids_frequencies         <- as.numeric(my_split(dat[31]))
   if(length(metierids)!=length(metierids_frequencies)) stop("Check config file for vessel creator - length(metierids)")
   visited_ports                 <- as.character(my_split2(dat[33]))
   visited_ports_frequencies     <- as.numeric(my_split(dat[35]))
   if(length(visited_ports)!=length(visited_ports_frequencies)) stop("Check config file for vessel creator - length(visited_ports)")
   name_file_ports               <-  dat[37]
   nb_stocks                     <- as.numeric(my_split(dat[39]))
   fixed_cpue_per_stock          <- as.numeric(my_split(dat[41]))
   if(length(fixed_cpue_per_stock)!=nb_stocks) stop("Check config file for vessel creator - length(fixed_cpue_per_stock)")
   gshape_cpue_per_stock         <- as.numeric(my_split(dat[43]))
   if(length(gshape_cpue_per_stock)!=nb_stocks) stop("Check config file for vessel creator - length(gshape_cpue_per_stock)")
   gscale_cpue_per_stock         <- as.numeric(my_split(dat[45]))
   if(length(gscale_cpue_per_stock)!=nb_stocks) stop("Check config file for vessel creator - length(gscale_cpue_per_stock)")
   vessel_features               <- as.numeric(my_split(dat[48]))
   step_in_share                 <- as.numeric(my_split(dat[50]))
   if(length(step_in_share)!=nb_stocks) stop("Check config file for vessel creator - length(step_in_share)")
   vesselsspe_betas              <- as.numeric(my_split(dat[52]))
   if(length(vesselsspe_betas)!=nb_stocks) stop("Check config file for vessel creator - length(vesselsspe_betas)")
   create_file_for_fuel_price_per_vessel_size   <-  as.logical(dat[54])
   some_fuel_price_per_vessel_size              <-   as.numeric(my_split(dat[56]))
   step_in_share_credits                        <-   as.numeric(dat[58])


#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#-----utils---------------------------------------------------------------------
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
 # loop over the SpatialPoly
 detectingCoordInPolygonsFromSH <- function (sh, coord, name_column="poly"){

     coord <- eval(parse(text=paste("cbind(coord, ",name_column,"= 0)", sep='')))
     dd                   <- sapply(slot(sh, "polygons"), function(x) lapply(slot(x, "Polygons"), function(x) x@coords)) # tricky there...

     ids <- sapply(slot(sh, "polygons"),  function (x) x@ID)

     library(sp)
     for(iLand in 1:length(dd)){
      if(length(dd)>1){
       for(i in 1:length(dd[[iLand]])){
          print(iLand)
          # Points on land are 1 or 2 and not is 0
          er <- try({res   <- point.in.polygon(coord[,1],coord[,2],dd[[iLand]][[i]][,1],dd[[iLand]][[i]][,2])}, silent=TRUE)
          if(class(er)=="try-error") res   <- point.in.polygon(coord[,1],coord[,2],dd[[iLand]][,1],dd[[iLand]][,2])
          coord[which(res!=0), name_column] <- ids[iLand]
       }
      } else{
          er <- try({res   <- point.in.polygon(coord[,1],coord[,2],dd[[1]][,1],dd[[1]][,2])}, silent=TRUE)
          if(class(er)=="try-error") res   <- point.in.polygon(coord[,1],coord[,2],dd[[1]][,1],dd[[1]][,2])
          coord[which(res!=0), name_column] <- ids[iLand]

      }

     }
 return(coord)
 }

getPolyAroundACoord <- function(dat, a_dist_m){
  lst <- list()
    for(i in 1: nrow(dat)){
      x <- dat[i,'coords.x1'] + a_dist_m * cos(seq(1,360, by=5)*pi/180)
      y <- dat[i,'coords.x2'] + a_dist_m * sin(seq(1,360, by=5)*pi/180)
      assign(paste("poly", i, sep=""),
        Polygon(cbind(  c(x,x[1]),
                   c(y, y[1])
                    )))
      assign  (paste("Poly", i, sep=""),  Polygons(list(get(paste("poly", i, sep=""))), ID=paste(dat[i, "ID"])) )
      lst[[i]] <- get(paste("Poly", i, sep=""))
     }
  return(lst)
  }



#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
 # load the graph
  #load(file.path(general$main.path.igraph, paste(general$igraph, "_graphibm.RData",sep=''))) # built from the R code
  coord <- read.table(file=file.path(general$main.path.ibm, "graphsspe",
             paste("coord", general$igraph, ".dat", sep=""))) # build from the c++ gui
  coord <- as.matrix(as.vector(coord))
  coord <- matrix(coord, ncol=3)
  coord <- cbind(coord, 1:nrow(coord))
  colnames(coord) <- c('x', 'y', 'harb', 'pt_graph')
  plot(coord[,1], coord[,2])

  saved_coord <- coord

  graph <- read.table(file=file.path(general$main.path.ibm, "graphsspe",
           paste("graph", general$igraph, ".dat", sep=""))) # build from the c++ gui
  graph <- as.matrix(as.vector(graph))
  graph <- matrix(graph, ncol=3)
  segments(coord[graph[,1]+1,1], coord[graph[,1]+1,2], coord[graph[,2]+1,1], coord[graph[,2]+1,2], col=4) # CAUTION: +1, because c++ to R


  #-------------------------------------------------------------------------------
  #-------------------------------------------------------------------------------
  # GET ALL THE NODES IN THE RANGE OF THE VESSEL SPECIFIC HARBOURS

  harbours <- read.table(file.path(general$main.path.param.gis, "GRAPH", name_file_ports), sep=";")
  harbours <- harbours[visited_ports,]
  harbours <- cbind.data.frame(harbours, ID=1:nrow(harbours))
  # convert to UTM
  library(sp)
  library(rgdal)
  SP       <- SpatialPoints(cbind(as.numeric(as.character(harbours[,'x'])), as.numeric(as.character(harbours[,'y']))),
                       proj4string=CRS("+proj=longlat +ellps=WGS84"))
  UTMzone  <- 33
  harbours <- cbind.data.frame(harbours,
                 spTransform(SP, CRS(paste("+proj=utm +zone=",UTMzone," +datum=WGS84 +units=m +no_defs +ellps=WGS84 +towgs84=0,0,0", sep=''))))    # convert to UTM


  lst             <- getPolyAroundACoord(harbours, a_dist_m= vessel_range_km * 1000)
  sp              <- SpatialPolygons(lst, 1:nrow(harbours))
  plot(sp)


  library(rgdal)
  library(maptools)
  library(raster)

  projection(sp)  <- CRS(paste("+proj=utm +zone=",UTMzone," +datum=WGS84 +units=m +no_defs +ellps=WGS84 +towgs84=0,0,0", sep='') )
   # transform back to decimal longlat
  sp            <- spTransform(sp, CRS("+proj=longlat +ellps=WGS84"))

  spo             <- SpatialPoints(coordinates(data.frame(CELL_LONG=coord[,1],
                                             CELL_LATI=coord[,2])))
  projection(spo) <-  CRS("+proj=longlat +ellps=WGS84")
  idx                           <- over(spo,sp)
  coord       <- cbind.data.frame(coord, poly=names(sp)[idx])

  #...then only keep the nodes inside the range of these particular vessels
  coord <- coord[!is.na(coord$poly),]

  # check
  plot(sp)
  points(coord[, c(1,2)], col=3)


#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------

 # IF (CREATING A SHAPE FILE FROM SCRATCH)
 # {
 # in ArcGIS 10:
 # create a blank shapefile in ArcCatalog by right click the folder and select New > shapefile (Feature Type: Polygon)
 # go to ArcMap and add the shape file with File>Add Data...choose a file name that will be put in the config file as well (see name_gis_file_for_fishing_effort_per_polygon)
 # (optional) open a coastline shape file e.g. in DISPLACE_input_raw\shp
 # (optional) open an informative XY layer e.g. grounds for cod from VMS analysis e.g. in DISPLACE_input_raw
 # Open the Editor toolbar from Custumize > toolbars> Editor
 # create (non-intersecting) polygons by selecting the polygon layr in Create Features dialog and choosing the Polygon Construction tools, click once to start the polygon, etc. and right click and Finish sketch...save and stop editing
 # define the projection
 # add a Field in menu TOC Open Attribute Table and click on Options button in the Table Frame and choose Add field... e.g. "feffort_h"
 # ...then go to the Editor toolbar >start editing and double click on a polygon to edit the feffort_h value...save and stop editing
 # }
 # {
 # ELSE USE INFO FROM PARTNERS
 # }


 library(maptools)
 handmade            <- readShapePoly(file.path(general$main.path.param.gis, "FISHERIES", name_gis_file_for_fishing_effort_per_polygon))  # build in ArcGIS 10.1
 library(rgdal)
 handmade2           <- readOGR(file.path(general$main.path.param.gis,  "FISHERIES"), name_gis_file_for_fishing_effort_per_polygon ) #  Projection info in a .prj associated with the shp should be imported automagically.
 if(is.na( projection(handmade2))) projection(handmade2) <- CRS("+proj=longlat +datum=WGS84")   # a guess!
 # How can I get the proj4 string from a shapefile .prj file? http://gis.stackexchange.com/questions/55196/how-can-i-get-the-proj4-string-or-epsg-code-from-a-shapefile-prj-file

 library(sp)
 library(rgdal)
 handmade_WGS84 <- spTransform(handmade2, CRS("+proj=longlat +datum=WGS84"))    # convert to longlat

 names(handmade_WGS84)  # "Id"         name_gis_layer_field

 plot(handmade_WGS84,  add=TRUE, border=as.data.frame(handmade_WGS84)[,name_gis_layer_field])


 # test coord for polygon inclusion
  coord <-  detectingCoordInPolygonsFromSH (handmade_WGS84, coord, name_column="poly_id")
  points(coord[,1], coord[,2], col=as.numeric(coord[,"poly_id"])+1, pch=16)  # check


 handmade_WGS84_df    <- as.data.frame(handmade_WGS84) # get attributes(handmade_WGS84)$data
 handmade_WGS84_df$ID <- 1:nrow(handmade_WGS84_df)  # CAUTION! rename ids because not all are kept when as.data.frame

 # caution:
 if(all(xfold_gis_layer_field==1)){
  # do nothing
   handmade_WGS84_df$xfold <- 1
 } else{
 handmade_WGS84_df$xfold         <- factor(handmade_WGS84_df[,name_gis_layer_field]) # init
 levels(handmade_WGS84_df$xfold) <- xfold_gis_layer_field
 handmade_WGS84_df$xfold         <-   as.character(handmade_WGS84_df$xfold)
 handmade_WGS84_df               <- rbind.data.frame( c(ID=0, 0, min(xfold_gis_layer_field)/2), handmade_WGS84_df)   # add an ID 0 for not included coord points AND ASSUME SOME ACTIVITY IN IT
 handmade_WGS84_df$xfold         <-   as.factor(handmade_WGS84_df$xfold)
 }


 # then merge to coord  (caution: 'poly' give the polygon in the harbour range; 'poly_id' give the polygon id from the handmade_WGS84 shape file)
 coord<- merge(coord, handmade_WGS84_df, by.x="poly_id", by.y="ID")




 # check
 plot(sp)
 plot(handmade_WGS84,  add=TRUE, border=as.data.frame(handmade_WGS84)[,name_gis_layer_field])
 coord$color <-  factor(coord[,name_gis_layer_field]) #init
 levels(coord$color) <- 1:length(levels(coord$color))
 points(coord[, "x"], coord[, "y"], col=  coord[,"color"])

 # or if on contineous scale:
 par(mfrow=c(1,2))
 plot(coord[, "x"], coord[, "y"], col= as.numeric( cut(coord[,name_gis_layer_field], breaks=c(-1,100,200, 500, 1000, 2000)) ), pch=16)
 if("XMIN" %in% colnames(handmade_WGS84_df)) plot(handmade_WGS84_df[, "XMIN"], handmade_WGS84_df[, "YMIN"], col= as.numeric( cut(handmade_WGS84_df[,name_gis_layer_field], breaks=c(-1,100,200, 500, 1000, 2000)) ), pch=16)


# WORKFLOW 2 - QUARTER-BASED-----------
# however, note that the seasonnality of the spatial and total effort application
# is not parameterized but is instead an emerging feature from the model.
 fgrounds <- NULL
 an <- function(x) as.numeric(as.character(x))
for (a.quarter in c("Q1","Q2","Q3","Q4")){

    # dispatch the feffort among nodes by dividing proba in area per the number of included graph nodes
    fgrounds_this_quarter                                <- coord
    fgrounds_this_quarter                                <- cbind.data.frame(fgrounds_this_quarter, quarter=a.quarter) # init
    fgrounds_this_quarter$nb_nodes_in_this_cat           <- factor(fgrounds_this_quarter[,name_gis_layer_field]) # init
    levels(fgrounds_this_quarter$nb_nodes_in_this_cat )  <- table(fgrounds_this_quarter[,name_gis_layer_field])
    fgrounds_this_quarter$effort_on_node                 <- an(fgrounds_this_quarter[,name_gis_layer_field]) * an(fgrounds_this_quarter[,'xfold'])  /   an(fgrounds_this_quarter$nb_nodes_in_this_cat)
    fgrounds_this_quarter$freq_feffort                   <- an(fgrounds_this_quarter$effort_on_node) /sum(an(fgrounds_this_quarter$effort_on_node))
     #=> scale to 1 to obtain a proba of visit per node
    if(is.na(fgrounds_this_quarter$freq_feffort)) fgrounds_this_quarter$freq_feffort  <- 1 # debug

    fgrounds <- rbind.data.frame(fgrounds, fgrounds_this_quarter)
 }

 # duplicate per vessel id  (i.e. assuming the same parameterisation for all the vesselids)
 fgrounds  <- fgrounds[fgrounds$freq_feffort!=0,] # remove if 0
 fgrounds_allvessels <- NULL
 for(vid in vesselids){
  cat(paste(vid, "\n"))
  fgrounds_allvessels <- rbind.data.frame(fgrounds_allvessels, cbind(fgrounds, vids=vid))
 }

 # duplicate per metier id (i.e. assuming the same relative effort distribution per polygon for all the metierids)
 fgrounds_allvessels_allmet <- NULL
 for(met in metierids){
  fgrounds_allvessels_allmet <- rbind.data.frame(fgrounds_allvessels_allmet, cbind(fgrounds_allvessels, met))
 }


 # create the c++ input files  (CAUTION if also active Workflow 1 then need to append to the existing files...)
 # i.e.
 # vesselsspe_fgrounds_quarter
 # vesselsspe_freq_fgrounds_quarter
 # vesselsspe_harbours_quarter
 # vesselsspe_freq_harbours_quarter

  ####-------
 an <- function(x) as.numeric(as.character(x))
 for (a.quarter in c("Q1","Q2","Q3","Q4")){

    x        <- fgrounds_allvessels[ fgrounds_allvessels$quarter==a.quarter,]
    x$freq   <- round(an(x$freq_feffort) ,6)

    # save .dat files
    x$pt_graph <-  as.numeric(as.character(x$pt_graph)) - 1 ##!!! OFFSET FOR C++ !!!##
        vesselsspe_fgrounds_quarter <- x[,c('vids','pt_graph')]
        write.table(vesselsspe_fgrounds_quarter,
            file=file.path(general$main.path.param, "vesselsspe",
              paste("vesselsspe_fgrounds_quarter",gsub("Q","",a.quarter),".dat",sep='')),
                  col.names=ifelse(do_append, FALSE, TRUE),  row.names=FALSE, sep= ' ', quote=FALSE, append=do_append)
        vesselsspe_freq_fgrounds_quarter <- x[,c('vids','freq')]
        vesselsspe_freq_fgrounds_quarter$freq <- format(vesselsspe_freq_fgrounds_quarter$freq, scientific = FALSE)
        write.table(vesselsspe_freq_fgrounds_quarter,
          file=file.path(general$main.path.param, "vesselsspe",
            paste("vesselsspe_freq_fgrounds_quarter",gsub("Q","",a.quarter),".dat",sep='')),
               col.names=ifelse(do_append, FALSE, TRUE),  row.names=FALSE, sep= ' ', quote=FALSE, append=do_append)
    #-----------





    #-----------
    # vesselsspe_harbours_quarter[xx].dat
    # vesselsspe_freq_harbours_quarter[xx].dat
     ## get back the port name
    port_names <- read.table(file=file.path(general$main.path.ibm,
                                paste("harboursspe_",general$namefolderinput,sep=''),
                                  paste("harbours.dat", sep='')), sep=";")
    port_names$pt_graph   <- saved_coord[match(port_names$idx.port, saved_coord[,"harb"]), "pt_graph"]

    visited               <- expand.grid( port_names[visited_ports, "pt_graph"], vesselids) # retrieve the corresponding pt_graph
    visited               <- visited[,2:1]
    colnames(visited)     <- c('vids','pt_graph')
    visited_freq          <- visited # init
    visited_freq[,2]      <- rep (visited_ports_frequencies)
    colnames(visited_freq)     <- c('vids','freq')

    # save .dat files
        visited$pt_graph <-  as.numeric(as.character(visited$pt_graph)) - 1 ##!!! FOR C++ !!!##
        write.table(visited,
            file=file.path(general$main.path.param, "vesselsspe",
              paste("vesselsspe_harbours_quarter",gsub("Q","",a.quarter),".dat",sep='')),
               col.names=ifelse(do_append, FALSE, TRUE),  row.names=FALSE, sep= ' ', quote=FALSE, append=do_append)
        visited_freq$freq <- format(visited_freq$freq, scientific = FALSE)
        write.table(visited_freq,
            file=file.path(general$main.path.param, "vesselsspe",
              paste("vesselsspe_freq_harbours_quarter",gsub("Q","",a.quarter),".dat",sep='')),
                col.names=ifelse(do_append, FALSE, TRUE),  row.names=FALSE, sep= ' ', quote=FALSE, append=do_append)

        # check for NAs
        dd<- visited [is.na(visited[,c('pt_graph')]) , ]
        if(nrow(dd)!=0) {print ("NAs in visited"); browser()}
        dd<- visited_freq [is.na(visited_freq[,c('freq')]) , ]
        if(nrow(dd)!=0) {print ("NAs in visited_freq"); browser()}
   #----------



   #-----------
   # DNK00[xx]_possible_metiers_quarter2.dat
   # DNK00[xx]_freq_possible_metiers_quarter[xx].dat
    x        <-  fgrounds_allvessels_allmet[ fgrounds_allvessels_allmet$quarter==a.quarter,]
    x$vids   <- factor(x$vids)
    for(vid in unique(x$vids)){
       x.vid                <- x[  x$vids %in% vid,]
       x.vid$VE_REF         <- factor(x.vid$vids)
       x.vid$pt_graph       <- factor(x.vid$pt_graph)
       x.vid$met            <- factor(x.vid$met)

       # save .dat files (vessel-spe .dat files)
       x.vid$pt_graph            <- as.numeric(as.character(x.vid$pt_graph)) - 1 ##!!! FOR C++ !!!##
       vesselsspe_possible_metiers_quarter <- x.vid[,c('pt_graph', 'met')]
       vesselsspe_possible_metiers_quarter <- vesselsspe_possible_metiers_quarter [order(vesselsspe_possible_metiers_quarter$pt_graph),]
       write.table(vesselsspe_possible_metiers_quarter,
           file=file.path(general$main.path.param, "vesselsspe",
             paste(vid,"_possible_metiers_quarter",gsub("Q","",a.quarter),".dat",sep='')),
               col.names=TRUE,  row.names=FALSE, sep= ' ', quote=FALSE, append=FALSE)

       vesselsspe_freq_possible_metiers_quarter <- x.vid[,c('pt_graph', 'met')]
       levels(vesselsspe_freq_possible_metiers_quarter[,'met']) <-   metierids_frequencies
       vesselsspe_freq_possible_metiers_quarter[,2]  <- as.numeric(as.character(vesselsspe_freq_possible_metiers_quarter[,2] ))
       colnames(vesselsspe_freq_possible_metiers_quarter) <- c('pt_graph', 'freq')
       vesselsspe_freq_possible_metiers_quarter <- vesselsspe_freq_possible_metiers_quarter [order(vesselsspe_freq_possible_metiers_quarter$pt_graph),]

        vesselsspe_freq_possible_metiers_quarter$freq <- format(vesselsspe_freq_possible_metiers_quarter$freq, scientific = FALSE)
       write.table(vesselsspe_freq_possible_metiers_quarter,
           file=file.path(general$main.path.param, "vesselsspe",
             paste(vid,"_freq_possible_metiers_quarter",gsub("Q","",a.quarter),".dat",sep='')),
               col.names=TRUE,  row.names=FALSE, sep= ' ', quote=FALSE, append=FALSE)

      # check for NAs
      dd<- vesselsspe_possible_metiers_quarter [is.na(vesselsspe_possible_metiers_quarter[,c('met')]) , ]
        if(nrow(dd)!=0) {print ("NAs in vesselsspe_possible_metiers_quarter"); browser()}
      dd <- vesselsspe_freq_possible_metiers_quarter [is.na(vesselsspe_freq_possible_metiers_quarter[,c('freq')]) , ]
        if(nrow(dd)!=0){print ("NAs in vesselsspe_freq_possible_metiers_quarter"); browser()}

     } # end vid
     #----------


   #-----------
   #-----------
   # DNK000XXX_gshape_cpue_per_stk_on_nodes_quarter  # plan A
   # DNK000XXX_gscale_cpue_per_stk_on_nodes_quarter  # plan A
    x        <- fgrounds_allvessels[fgrounds_allvessels$quarter==a.quarter,]
    x$vids   <- factor( x$vids )

    for(vid in unique(x$vids)){
       x.vid                <- x[  x$vids %in% vid,]
       x.vid$VE_REF         <- factor(x.vid$vids)
       x.vid$pt_graph       <- factor(x.vid$pt_graph)
       x.vid$pt_graph       <- as.numeric(as.character(x.vid$pt_graph)) - 1 ##!!! FOR C++ !!!##

       vesselsspe_gshape_cpue_per_stock_fgrounds_quarter <-   cbind.data.frame(rep(x.vid$pt_graph, each=nb_stocks), gshape_cpue_per_stock)
       colnames(vesselsspe_gshape_cpue_per_stock_fgrounds_quarter) <-  c('pt_graph', 'shape')

       # save .dat files
       write.table(vesselsspe_gshape_cpue_per_stock_fgrounds_quarter,
           file=file.path(general$main.path.param, "vesselsspe",
             paste(vid,"_gshape_cpue_per_stk_on_nodes_quarter",gsub("Q","",a.quarter),".dat",sep='')),
               col.names=TRUE,  row.names=FALSE, sep= ' ', quote=FALSE, append=FALSE)
    }
   # DNK000XXX_gscale_cpue_per_stk_on_nodes_quarter  # plan A
    #-----------
    x        <- fgrounds_allvessels[fgrounds_allvessels$quarter==a.quarter,]
    x$vids   <- factor( x$vids )

    for(vid in unique(x$vids)){
       x.vid                <- x[  x$vids %in% vid,]
       x.vid$VE_REF         <- factor(x.vid$vids)
       x.vid$pt_graph       <- factor(x.vid$pt_graph)
       x.vid$pt_graph       <- as.numeric(as.character(x.vid$pt_graph)) - 1 ##!!! FOR C++ !!!##

       vesselsspe_gscale_cpue_per_stock_fgrounds_quarter <-   cbind.data.frame(rep(x.vid$pt_graph, each=nb_stocks), gscale_cpue_per_stock)
       colnames(vesselsspe_gscale_cpue_per_stock_fgrounds_quarter) <-  c('pt_graph', 'scale')

       # save .dat files
       write.table(vesselsspe_gscale_cpue_per_stock_fgrounds_quarter,
           file=file.path(general$main.path.param, "vesselsspe",
             paste(vid,"_gscale_cpue_per_stk_on_nodes_quarter",gsub("Q","",a.quarter),".dat",sep='')),
               col.names=TRUE,  row.names=FALSE, sep= ' ', quote=FALSE, append=FALSE)
    }
    #-----------



   #-----------
   #-----------
   # DNK000XXX__cpue_per_stk_on_nodes_quarter        # plan B
    x        <- fgrounds_allvessels[fgrounds_allvessels$quarter==a.quarter,]
    x$vids   <- factor( x$vids )

    for(vid in unique(x$vids)){
       x.vid                <- x[  x$vids %in% vid,]
       x.vid$VE_REF         <- factor(x.vid$vids)
       x.vid$pt_graph       <- factor(x.vid$pt_graph)
       x.vid$pt_graph       <- as.numeric(as.character(x.vid$pt_graph)) - 1 ##!!! FOR C++ !!!##

       vesselsspe_fixed_cpues_fgrounds_quarter <-   cbind.data.frame(rep(x.vid$pt_graph, each=nb_stocks), fixed_cpue_per_stock)
       colnames(vesselsspe_fixed_cpues_fgrounds_quarter) <-  c('pt_graph', 'cpue_kghour')

       # save .dat files
       write.table(vesselsspe_fixed_cpues_fgrounds_quarter,
           file=file.path(general$main.path.param, "vesselsspe",
             paste(vid,"_cpue_per_stk_on_nodes_quarter",gsub("Q","",a.quarter),".dat",sep='')),
               col.names=TRUE,  row.names=FALSE, sep= ' ', quote=FALSE, append=FALSE)
    }
   #-----------


   #-----------
   #-----------
   #vesselsspe_features_quarter1

   vesselsspe_features_quarter <- cbind.data.frame (vesselids,  matrix(rep(vessel_features, length(vesselids)),ncol= length(vessel_features), byrow=TRUE) )
     # save .dat files
       write.table(vesselsspe_features_quarter,
           file=file.path(general$main.path.param, "vesselsspe",
             paste("vesselsspe_features_quarter",gsub("Q","",a.quarter),".dat",sep='')),
               col.names=FALSE,  row.names=FALSE, quote=FALSE, append=do_append, sep = "|")
   #-----------


 } # end a.quarter






# WORKFLOW 2 - SEMESTER-BASED-----------
 for (a.semester in 1:2){


   #-----------
   #-----------
   # vesselsspe_percent_tacs_per_pop_semester
   vesselsspe_percent_tacs_per_pop <- cbind.data.frame(rep(vesselids, each=nb_stocks), rep(100/ length(vesselids), length(vesselids)) )
   #colnames(vesselsspe_percent_tacs_per_pop) <- c('vid', 'percent')
   colnames(vesselsspe_percent_tacs_per_pop) <- c('V1', 'V2')


   if(do_append && file.exists(file.path(general$main.path.param, "vesselsspe",
             paste("vesselsspe_percent_tacs_per_pop_semester",a.semester,".dat",sep='')))){ # need to re-compute the percent for ALL vessels i.e. existing and incomers depending on step_in_share
      existing <- read.table(file.path(general$main.path.param, "vesselsspe",
             paste("vesselsspe_percent_tacs_per_pop_semester",a.semester,".dat",sep='')), header=TRUE)
      existing[,2] <-  an(existing[,2])* (100-rep(step_in_share, length(unique(existing[,1])) ))/100

      if(any(vesselsspe_percent_tacs_per_pop[,1] %in% existing[,1])) stop("Check incomers vesselids...already there!")

      vesselsspe_percent_tacs_per_pop[,2] <- vesselsspe_percent_tacs_per_pop[,2]  * (rep(step_in_share, length(vesselids) ))/100

      vesselsspe_percent_tacs_per_pop <- rbind.data.frame(existing, vesselsspe_percent_tacs_per_pop)
   }

     # if(floor(sum(vesselsspe_percent_tacs_per_pop[,2])) != (100*nb_stocks)) stop("The TAC share over vessels per stock should sum to 100%...check the dimension")

     # save .dat files
       write.table(vesselsspe_percent_tacs_per_pop,
           file=file.path(general$main.path.param, "vesselsspe",
             paste("vesselsspe_percent_tacs_per_pop_semester",a.semester,".dat",sep='')),
               col.names=TRUE,  row.names=FALSE, quote=FALSE, append=FALSE, sep = " ")
   #-----------



   #-----------
   #-----------
   # vesselsspe_betas_semester
   vesselsspe_betas_semester <- cbind.data.frame(rep(vesselids, each=nb_stocks), rep(vesselsspe_betas, length(vesselids)) )
   colnames(vesselsspe_betas_semester) <- c('VE_REF', 'beta.VE_REF')

     # save .dat files
       write.table(vesselsspe_betas_semester,
           file=file.path(general$main.path.param, "vesselsspe",
             paste("vesselsspe_betas_semester", a.semester,".dat",sep='')),
               col.names=ifelse(do_append, FALSE, TRUE),  row.names=FALSE, quote=FALSE, append=do_append, sep = " ")
   #-----------

 }

 # WORKFLOW 2 - ADDITIONAL FILE(S)-----------
 if(create_file_for_fuel_price_per_vessel_size){
  fuel_price_per_vessel_size <- data.frame(
                                        vsize=c(0:4),
                                        fuelprice_euro= some_fuel_price_per_vessel_size
                                        )
    # save .dat files
       write.table(fuel_price_per_vessel_size,
           file=file.path(general$main.path.param, "vesselsspe",
             paste("fuel_price_per_vessel_size.dat",sep='')),
               col.names=TRUE,  row.names=FALSE, quote=FALSE, append=FALSE, sep = " ")

 }


# fishing credits allocation taken from the pool
  share_of_fishing_credits <- cbind.data.frame(vesselids,  100/length(vesselids))

  if(do_append && file.exists(file.path(general$main.path.param, "vesselsspe", paste("initial_fishing_credits_per_vid.dat",sep='')))
             ){ # need to re-compute the percent for ALL vessels i.e. existing and incomers depending on step_in_share
      existing <- read.table(file.path(general$main.path.param, "vesselsspe",
             paste("initial_fishing_credits_per_vid.dat",sep='')), header=TRUE)
      existing[,2] <-  an(existing[,2])* (100-rep(step_in_share_credits, length(unique(existing[,1])) ))/100

      if(any(share_of_fishing_credits[,1] %in% existing[,1])) stop("Check incomers vesselids...already there!")

      share_of_fishing_credits[,2] <- share_of_fishing_credits[,2]  * (rep(step_in_share_credits, length(vesselids) ))/100

      colnames(share_of_fishing_credits) <- colnames(existing)
      fishing_credits <- rbind.data.frame(existing, share_of_fishing_credits)

   } else{
     fishing_credits            <- cbind.data.frame(vesselids, an(share_of_fishing_credits[,2])*10000 )
     colnames(fishing_credits)  <- c("VE_REF", "annual_fishing_credits_per_vid")
     fishing_credits$annual_fishing_credits_per_vid <- format(fishing_credits$annual_fishing_credits_per_vid, scientific = FALSE)
   }


# save .dat files
       write.table(fishing_credits,
           file=file.path(general$main.path.param, "vesselsspe",
             paste("initial_fishing_credits_per_vid.dat",sep='')),
               col.names=TRUE,  row.names=FALSE, quote=FALSE, append=FALSE, sep = " ")


} # END LOOP OVER CONFIG FILES




 ## LASTLY,
   # fix the files to avoid vessel fishing in harbour (!)
   # but do not remove a vessel by inadvertance....
 for (a.quarter in c("Q1","Q2","Q3","Q4")){

    port_names <- read.table(file=file.path(general$main.path.ibm,
                                paste("harboursspe_",general$namefolderinput,sep=''),
                                  paste("harbours.dat", sep='')), sep=";")
    port_names$pt_graph   <- saved_coord[match(port_names$idx.port, saved_coord[,"harb"]), "pt_graph"]
    port_names$pt_graph <-  as.numeric(as.character(port_names$pt_graph)) - 1 ##!!! FOR C++ !!!##

    # 1
    vesselsspe_fgrounds_quarter <- read.table(file.path(general$main.path.param, "vesselsspe",
              paste("vesselsspe_fgrounds_quarter",gsub("Q","",a.quarter),".dat",sep='')), header=TRUE)
    idx  <- vesselsspe_fgrounds_quarter$pt_graph %in%  port_names$pt_graph
    vids_before <- vesselsspe_fgrounds_quarter[,1]
    vesselsspe_fgrounds_quarter <- vesselsspe_fgrounds_quarter[!idx,]  # correct
    vids_after <- vesselsspe_fgrounds_quarter[,1]
    print(vids_before[!vids_before %in% vids_after])  #check if all vid kept on board...if not you are in trouble....

    colnames(vesselsspe_fgrounds_quarter) <- c("vids", "pt_graph")


    write.table(vesselsspe_fgrounds_quarter,
          file=file.path(general$main.path.param, "vesselsspe",
            paste("vesselsspe_fgrounds_quarter",gsub("Q","",a.quarter),".dat",sep='')),
               col.names=TRUE,  row.names=FALSE, sep= ' ', quote=FALSE, append=FALSE)
    # 2
    vesselsspe_freq_fgrounds_quarter <- read.table(file.path(general$main.path.param, "vesselsspe",
              paste("vesselsspe_freq_fgrounds_quarter",gsub("Q","",a.quarter),".dat",sep='')), header=TRUE)
    vesselsspe_freq_fgrounds_quarter <- vesselsspe_freq_fgrounds_quarter[!idx,]  # correct
    colnames(vesselsspe_freq_fgrounds_quarter) <- c("vids", "freq")


    vesselsspe_freq_fgrounds_quarter$freq <- format(vesselsspe_freq_fgrounds_quarter$freq, scientific = FALSE)
    write.table(vesselsspe_freq_fgrounds_quarter,
          file=file.path(general$main.path.param, "vesselsspe",
            paste("vesselsspe_freq_fgrounds_quarter",gsub("Q","",a.quarter),".dat",sep='')),
               col.names=TRUE,  row.names=FALSE, sep= ' ', quote=FALSE, append=FALSE)
    }
    #-----------


cat("Remenber that because some new fgrounds are created \n you will have (in DISPLACE GUI) to derive a new graph with a new shortestPath library (say 57 by loading the graph 56 and then create the shortestPaths) \n and also create a new baseline.dat /simusspe with the right missing port idx (say 9979) (absence of the right index for the port makes the ui crash)\n")






