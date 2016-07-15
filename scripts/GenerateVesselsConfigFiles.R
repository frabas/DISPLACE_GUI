 # some args for the bunch of vessels to be created....
 
   # GENERAL SETTINGS
   general <- list()
   if(.Platform$OS.type == "windows") {
     general$main.path             <- file.path("C:","DISPLACE_outputs")
     general$application           <- "adriatic" # ...or myfish
     general$main.path.param       <- file.path("C:","Users","fbas","Documents","GitHub","DISPLACE_input_raw")
     general$main.path.param.gis   <- file.path("C:","Users","fbas","Documents","GitHub","DISPLACE_input_gis", general$application)
     general$main.path.ibm         <- file.path("C:","Users","fbas","Documents","GitHub",paste("DISPLACE_input_", general$application, sep=''))
  
   # dir_to_clean <-    file.path(general$main.path.ibm, "FISHERIES", "vessels_config_files")
   # file.remove(dir(   dir_to_clean, pattern = "vessels_creator_args", full.names = FALSE))
 
   }
   
  
  
   general$namefolderinput    <- "adriatic"
   dir.create(path=file.path(general$main.path.param.gis, "FISHERIES", "vessels_config_files"))
  
   vessel_specifications <- read.table(file=file.path(general$main.path.param.gis, "FISHERIES", "vessels_specifications_per_harbour_3metiers.csv"), sep=",", header=TRUE )
   vessel_specifications <- cbind.data.frame(vessel_specifications, id=1:nrow(vessel_specifications))
   #=> CAUTION: do not leave white rows at the end of this file! otherwise will create some extra wrong config_files
   
   nb_agent_per_vessels <- 4  # caution: super-individuals to reduce the total nb of vessels to be simulated
   vessel_specifications[, "N..of.vessels"] <- ceiling(vessel_specifications[, "N..of.vessels"])/nb_agent_per_vessels
   
   port_names <- read.table(file=file.path(general$main.path.ibm,
                                paste("harboursspe_",general$namefolderinput,sep=''),
                                  paste("harbours.dat", sep='')), sep=";")
  # quick check
   if(!"bottom_otter_trawl" %in% unique(vessel_specifications[, "metier"]) 
       && !"gillnet" %in% unique(vessel_specifications[, "metier"]) 
       && !"rapido_trawl" %in% unique(vessel_specifications[, "metier"]))  stop("Not defined for these metiers...check metier names")
   
   for (i in 1 : nrow(vessel_specifications)){
      spp                        <- c("hake", "sole", "redmullet", "mantis")
      general$igraph             <- 1  # caution: should be consistent with existing vessels already built upon a given graph
      do_append                  <- TRUE
      harbcode                   <- paste(substr (vessel_specifications[i, "Harbor"], 1, 3), vessel_specifications[i, "id"], sep="")
      nbvids                     <- vessel_specifications[i, "N..of.vessels"]
      
      
      if(vessel_specifications[i, "metier"]=="gillnet"){
         name_gis_file_for_fishing_effort_per_polygon <- "set_nets"
         name_gis_layer_field                         <- "GRIDCODE"                     # giving releative effort ditribtion e.g. in 5 categories: 1 to 5 with 1 high occurence
         is_gis_layer_field_relative_numbers          <- TRUE                           # if relative effort categories (e.g. high to low) then xfold_gis_layer_field will be used to convert in absolute
         xfold_gis_layer_field                        <- c(64, 32, 16, 4, 2, 1)     # giving relative importance of the 5 categories e.g. visting an area of cat 1 is 10000 times more probable than for cat 5
         #xfold_gis_layer_field                        <- c(10000, 1000, 100, 10, 1)     # giving relative importance of the 5 categories e.g. visting an area of cat 1 is 10000 times more probable than for cat 5
         vesselids                                    <- paste("ITA_",harbcode,"_NET", 1:nbvids, sep="") # caution: three first letters (nationality) should be consistent with  popsspe/XXctrysspe_relative_stability_semesterXX 
         vessel_range_km                              <- 15 # netters
         metierids                                    <- 1             # e.g. c(0,1) # look at /metiersspe.... 0:trawler; 1 gillnetter
         metierids_frequencies                        <- c(1)  # pure fishery        # or e.g. c(0.2,0.8)
         cruisespeed                                  <- 18 # knots
                   
         }   
      if(vessel_specifications[i, "metier"]=="otter_trawl"){
         name_gis_file_for_fishing_effort_per_polygon <- "bottom_otter_trawl"
         name_gis_layer_field                         <- "OTB_13" 
         is_gis_layer_field_relative_numbers          <- TRUE                           # if relative effort categories (e.g. high to low) then xfold_gis_layer_field will be used to convert in absolute
         xfold_gis_layer_field                        <- 1     # giving relative importance of the 5 categories e.g. visting an area of cat 1 is 10000 times more probable than for cat 5
         vesselids                                    <-  paste("ITA_",harbcode,"_OTB", 1:nbvids, sep="") # caution: three first letters should be consistent with  popsspe/XXctrysspe_relative_stability_semesterXX
         vessel_range_km                              <- 80  # trawlers
         metierids                                    <- 0             # e.g. c(0,1) # look at /metiersspe.... 0:trawler; 1 gillnetter
         metierids_frequencies                        <- c(1)  # pure fishery        # or e.g. c(0.2,0.8)
         cruisespeed                                  <- 10 # knots
         } 
    if(vessel_specifications[i, "metier"]=="rapido_trawl"){
         name_gis_file_for_fishing_effort_per_polygon <- "Rapidi Drawing_def"
         name_gis_layer_field                         <- "TOTAL"     
         is_gis_layer_field_relative_numbers          <- TRUE                           # if relative effort categories (e.g. high to low) then xfold_gis_layer_field will be used to convert in absolute
         xfold_gis_layer_field                        <- 1     # giving relative importance of the 5 categories e.g. visting an area of cat 1 is 10000 times more probable than for cat 5
         vesselids                                    <-  paste("ITA_",harbcode,"_RAP", 1:nbvids, sep="") # caution: three first letters should be consistent with  popsspe/XXctrysspe_relative_stability_semesterXX
         vessel_range_km                              <- 80  # trawlers
         metierids                                    <- 2             # e.g. c(0,1) # look at /metiersspe.... 0:trawler; 1 gillnetter
         metierids_frequencies                        <- c(1)  # pure fishery        # or e.g. c(0.2,0.8)
         cruisespeed                                  <- 10 # knots
         } 
   visited_ports              <- vessel_specifications[i, "Harbor"]   # e.g. c("ANCONA", "RIMINI") # should exist in harbour.dat!
   
    #  dd <- read.table ("C:\\Users\\fbas\\Documents\\GitHub\\DISPLACE_input_adriatic\\harboursspe_adriatic\\names_harbours.dat", header=T)
    # unique(as.character(vessel_specifications$Harbor))[! unique(as.character(vessel_specifications$Harbor)) %in% dd$name]
   if(!visited_ports %in% row.names(port_names)){  
       visited_ports <- switch(as.character(visited_ports), 
                 "ROSETO DEGLI ABRUZZI"= "GIULIANOVA",
                 "SILVI"= "PESCARA",
                 "TORTORETO"= "GIULIANOVA",
                 "RAVENNA"="PORTO CORSINI",
                 "MONFALCONE"="TRIESTE",
                 "SISTIANA"= "TRIESTE",
                 "CUPRA MARITTIMA"="SAN BENEDETTO DEL TRONTO",
                 "GABICCE MARE"="RIMINI",
                 "GROTTAMMARE"="SAN BENEDETTO DEL TRONTO",
                 "PORTO SAN GIORGIO"="CIVITANOVA MARCHE",
                 "MARGHERITA DI SAVOIA"="VIESTE",
                 "RODI GARGANICO"="PESCHICI",
                 "BURANO"="VENEZIA",
                 "IESOLO"="CAORLE",
                 "LIGNANO SABBIADORO"="MARANO LAGUNARE", 
                 "PELLESTRINA"="CHIOGGIA" ,
                 "PORTO LEVANTE"= "CHIOGGIA",
                 "SCARDOVARI"="PORTO TOLLE" ,
                 "SAN BENEDETTO"="SAN BENEDETTO DEL TRONTO",
                 
                 "ANCONA") ## CAUTION DEBUG: MISSING PORTS IN GRAPH......
     }
   if(visited_ports=="VIESTE") visited_ports <- "RODI GARGANICO" 
   #  "ITA_ROD40_NET1" <="ITA_VIE41_NET2"  and ITA_MAR39_NET1
   
     
     
   name_file_ports            <- "harbours_adriatic.dat" 
   visited_ports_frequencies  <- c(1)          # e.g. c(0.8, 0.2)
   nb_stocks                  <- length(spp)  # from 0 in c++
   fixed_cpue_per_stock       <- vessel_specifications[i, paste(spp, "_kg_h", sep='') ] *nb_agent_per_vessels# kg per hour 
   gshape_cpue_per_stock      <- rep(1, nb_stocks)     # for Gamma on each node
   gscale_cpue_per_stock      <-vessel_specifications[i, paste(spp, "_kg_h", sep='') ]   *nb_agent_per_vessels# for Gamma on each node e.g. hist(rgamma(1000,shape=0.74,scale=1))
   vessel_features            <- c(cruisespeed, 
                                   vessel_specifications[i, "fuel.cons.h"] *nb_agent_per_vessels,
                                   vessel_specifications[i, "mean_LOA_m"] *nb_agent_per_vessels, 
                                   vessel_specifications[i, "mean_kW"] *nb_agent_per_vessels,
                                   vessel_specifications[i, "ave.storage.fish.kg"] *nb_agent_per_vessels,
                                   vessel_specifications[i, "fuel.tank.liter"] *nb_agent_per_vessels,
                                   10, # nb_pings_per_trip
                                   0.4485, # Gamma shape
                                   336.7618, # Gamma scale
                                   vessel_specifications[i, "trip.duration.h"],
                                   vessel_specifications[i, "multip.fuel.steaming"],
                                   vessel_specifications[i, "multip.fuel.fishing"],
                                   vessel_specifications[i, "multip.fuel.ret.port.fish"],
                                   vessel_specifications[i, "multip.fuel.inactive"],
                                   vessel_specifications[i, "weekEndStartDay"],
                                   vessel_specifications[i, "WeekEndEndDay"],
                                   vessel_specifications[i, "WorkHoursStart"],
                                   vessel_specifications[i, "WorkHoursEnd"]
                                   ) 
   step_in_share              <- rep(vessel_specifications[i, "N..of.vessels"]/ sum(vessel_specifications[, "N..of.vessels"], na.rm=TRUE), nb_stocks) # i.e. 100 % of each TAC per stock will be booked for these new vessels
   vesselsspe_betas           <- log(vessel_specifications[i, paste(spp, "_kg_h", sep='') ]*nb_agent_per_vessels +0.01 )  # catch rate log(kg per hour) from the Marche region # stock name 0:Hake, 1:Sole, 2: Mullet, 3: Mantis  CAUTION: log(0)=-Inf !
   create_file_for_fuel_price_per_vessel_size <- TRUE
   some_fuel_price_per_vessel_size            <- c(0.4,0.4,0.4,0.4,0.4)  # euro per litre
   step_in_share_credits                      <- vessel_specifications[i, "N..of.vessels"]/ sum(vessel_specifications[, "N..of.vessels"]) # i.e. % of the credits will be booked for these new vessels

   # create a (intermediate) config file
   namefile <- file.path(general$main.path.param.gis, "FISHERIES", "vessels_config_files",
               paste("vessels_creator_args_",general$namefolderinput, "_", harbcode, "_", vessel_specifications[i, "metier"], ".dat", sep=''))
   
   write("# config file for the vessel editor: adding some vessel(s)", file=namefile)
   write("# (the shortestPaths library will have to be re-created for the graph)", file=namefile, ncolumns=1, append=TRUE)
   write("# --------------", file=namefile, ncolumns=1, append=TRUE)
  
   write("# input folder for config file", file=namefile, ncolumns=1, append=TRUE)
   write(general$main.path.param.gis, file=namefile, ncolumns=1, append=TRUE)
  
   write("# output folder for parameterisation file", file=namefile, ncolumns=1, append=TRUE)
   write(general$main.path.param, file=namefile, ncolumns=1, append=TRUE)
  
   write("# input folder for DISPLACE", file=namefile, ncolumns=1, append=TRUE)
   write(general$main.path.ibm, file=namefile, ncolumns=1, append=TRUE)
  
   write("# name of the application",file=namefile, ncolumns=1, append=TRUE)
   write(general$namefolderinput, file=namefile, ncolumns=1, append=TRUE)
  
   write("# name of the graph for this application", file=namefile, ncolumns=1, append=TRUE)
   write(general$igraph,file=namefile, ncolumns=1, append=TRUE)
   
   write("# append to existing vessel files", file=namefile, ncolumns=1, append=TRUE)
   write(do_append,file=namefile, ncolumns=1, append=TRUE)
  
   write("# name gis file for total effort per polygon",file=namefile, ncolumns=1, append=TRUE)
   write(name_gis_file_for_fishing_effort_per_polygon, file=namefile, ncolumns=1, append=TRUE)
 
   write("# name_gis_layer_field",file=namefile, ncolumns=1, append=TRUE)
   write(name_gis_layer_field, file=namefile, ncolumns=1, append=TRUE)
 
   write("# is_gis_layer_field_relative_numbers",file=namefile, ncolumns=1, append=TRUE)
   write(is_gis_layer_field_relative_numbers, file=namefile, ncolumns=1, append=TRUE)
 
   write("# xfold_gis_layer_field",file=namefile, ncolumns=1, append=TRUE)
   write(xfold_gis_layer_field, file=namefile, ncolumns=length(xfold_gis_layer_field), append=TRUE)
 
   write("# vesselids", file=namefile, ncolumns=1, append=TRUE)
   write(vesselids, file=namefile, ncolumns=length(vesselids), append=TRUE)
 
   write("# vessel_range_km", file=namefile, ncolumns=1, append=TRUE)
   write(vessel_range_km, file=namefile, ncolumns=1, append=TRUE)
     
   write("# metierids", file=namefile, ncolumns=1, append=TRUE)
   write(metierids, file=namefile, ncolumns=length(metierids), append=TRUE)
   
   write("# metierids_frequencies", file=namefile, ncolumns=1, append=TRUE)
   write(metierids_frequencies, file=namefile, ncolumns=length(metierids_frequencies), append=TRUE)
   
   write("# visited_ports (look at the names in harbours.dat in /harboursspe)", file=namefile, ncolumns=1, append=TRUE)
   write(as.character(visited_ports), file=namefile, ncolumns=length(as.character(visited_ports)), append=TRUE)
   
   write("# visited_ports_frequencies", file=namefile, ncolumns=1, append=TRUE)
   write(visited_ports_frequencies, file=namefile, ncolumns=length(visited_ports_frequencies), append=TRUE)
   
   write("# name_file_ports", file=namefile, ncolumns=1, append=TRUE)
   write(name_file_ports, file=namefile, ncolumns=1, append=TRUE)
      
   write("# nb fish or shellfish stocks (should be consistent with /popsspe)",file=namefile, ncolumns=1, append=TRUE)
   write(nb_stocks, file=namefile, ncolumns=length(nb_stocks), append=TRUE)
  
   write("# fixed cpue per stock on fgrounds(plan B)", file=namefile, ncolumns=1, append=TRUE)
   write(as.character(as.numeric(fixed_cpue_per_stock)), file=namefile, ncolumns=length(as.character(as.numeric(fixed_cpue_per_stock))), append=TRUE)
  
   write("# Gamma (shape parameter) cpue per stock on fgrounds (plan A but for implicit stocks or out of range nodes)", file=namefile, ncolumns=1, append=TRUE)
   write(as.character(as.numeric(gshape_cpue_per_stock)), file=namefile, ncolumns=length(as.character(as.numeric(gshape_cpue_per_stock))), append=TRUE)
  
   write("# Gamma (scale parameter) cpue per stock on fgrounds(plan A but for implicit stocks or out of range nodes)", file=namefile, ncolumns=1, append=TRUE)
   write(as.character(as.numeric(gscale_cpue_per_stock)), file=namefile, ncolumns=length(as.character(as.numeric(gscale_cpue_per_stock))), append=TRUE)
  
   write("# vessel features (speed, fuelconsrate, length, kW, carrying_capacity, tank_capacity, nb_pings_per_trip, shape_in_btw, scale_in_btw, av.trip.duration)",
   file=namefile, ncolumns=1, append=TRUE)
  
   write("#  mult_fuelcons_when_steaming, mult_fuelcons_when_fishing, mult_fuelcons_when_returning, mult_fuelcons_when_inactive) ",
     file=namefile, ncolumns=1, append=TRUE)         
   write(vessel_features, file=namefile, ncolumns=length(vessel_features), append=TRUE)
  
   write("# percent step in share for TAC (per stock) for these incoming vessels (only used if existing vessels)",file=namefile, ncolumns=1, append=TRUE)
   write(step_in_share, file=namefile, ncolumns=length(step_in_share), append=TRUE)
   
   write("# vessel effect (per stock) in the catch rate equation ", file=namefile, ncolumns=1, append=TRUE)
   write(as.character(as.numeric(vesselsspe_betas)), file=namefile, ncolumns=length(as.character(as.numeric(vesselsspe_betas))), append=TRUE)
  
   write("# create the file for fuel price per vessel size  ",file=namefile, ncolumns=1, append=TRUE)
   write(create_file_for_fuel_price_per_vessel_size, file=namefile, ncolumns=length(create_file_for_fuel_price_per_vessel_size), append=TRUE)
  
   write("# some fuel price per vessel size (Euro per litre) ",file=namefile, ncolumns=1, append=TRUE)
   write(some_fuel_price_per_vessel_size, file=namefile, ncolumns=length(some_fuel_price_per_vessel_size), append=TRUE)
  
   write("# i% fishing credits taken from incomers (for RTI management) ", file=namefile, ncolumns=1, append=TRUE)
   write(step_in_share_credits, file=namefile, ncolumns=length(step_in_share_credits), append=TRUE)

   }   # end for loop over record
    

    
   
   
   
      
 