README
======

Summary
- Rationale
- How to cite
- How to install
- How to build
- GDAL notes



Rationale
============

Dynamic Individual vessel-based Spatial fishing PLAnning and effort displaCEment

DISPLACE: a dynamic, individual-based model for spatial fishing planning and effort displacement 
integrating underlying fish population models, check out the paper 2014 in 
Canadian Journal of Fisheries and Aquatic Sciences

The DISPLACE project develops and provides a platform primarily for research purposes 
to transform the fishermens detailed knowledge into models, evaluation tools, and methods 
that can provide the fisheries with research and advice. 
The model intends to serve as a basis for decision support tools for (fishery) managers.

Among other goals, economic benefit of stock replenishment and sustainable harvesting 
can be demonstrated. This contributes to evaluate the combined ecological and economic impacts 
of fishery management before its implementation (i.e. impact assessment).

An individual-based model (IBM) on a per-vessel basis covering several fisheries and stocks 
is a benchmark tool capable of integrating fishermens decision-making processes 
when they face changes in fishery management, economic factors influencing the fishery, 
economic viability, and underlying stock conditions, including spatial and 
seasonal patterns in resource availability.

DISPLACE operates with high resolution in time and space (spatio-temporal explicit). 
It allows contributing to marine/maritime spatial planning for e.g. evaluating 
the effects of smaller and larger marine area restrictions on stocks and fisheries, 
i.e. impact assessment on stocks and fisheries of broader marine management. 
The economic evaluation includes evaluation of spatial restrictions for the fishery 
with particular emphasis to test if there are any economic benefits or 
consequences from different area restrictions caused by e.g. renewable energy, conservation, etc.
Also it evaluates whether these benefits compensate the additional costs of 
the effort displaced to surrounding or new areas, i.e. the bio-economic consequences 
of fishing effort displacement.

This ultimately incorporates other utilization of the sea such as energy production, 
transport, recreational use, etc., e.g. offshore windmill farms, 
large marine constructions, NATURA 2000 areas, transport routes of 
commercial shipping, pipelines, cables, etc.

Fishermen communities have an obvious stake in using the sea to ensure food supply with seafood 
and should not then be left out the maritime spatial planning process. 
As a stakeholder once said if you are not at the table, you are on the menu! 
While fishermen are therefore likely to gain at designing their own priorities areas, 
the DISPLACE model should be a support for evaluating and balance these important areas 
against other potential uses and the economic consequences 
of spatial plans on fishermen´s living conditions.


How to cite
============

* Bastardie F, Nielsen JR, Miethe T. 2014. DISPLACE: a dynamic, individual-based model for 
spatial fishing planning and effort displacement - integrating underlying 
fish population models. Canadian Journal of Fisheries and Aquatic Sciences. 71(3):366-386. 
Available from: 10.1139/cjfas-2013-0126

* Bastardie, F., Nielsen, J. R., Eigaard, O. R., Fock, H. O., Jonsson, P., and Bartolino, V. 
Competition for marine space: modelling the Baltic Sea fisheries and effort displacement 
under spatial restrictions.  ICES Journal of Marine Science, doi: 10.1093/icesjms/fsu215.


How to install
============


Look at the Release section on the GitHub repository 
to download an installer for Windows. 

An example dataset is also available (DISPLACE_input)

or alternatively, look at the google drive for DISPLACE
also hosting additional files


How to compile
============
[compiling on Windows](docs/Building.win)
[compiling on unix](docs/Building.unix)
[compiling on MacOSX](docs/Building.MacOSX)



GDAL Notes
==========

Current version of QMapControl supports shapefiles loading ONLY for WGS84 Coordinates system. This is because QMapControl itself uses WGS84 coordinates.
So you need to convert your shapefiles if not using this CRS.
You can use ogr2ogr or the provided script in the scripts/ directory.

$ ogr2ogr -t_srs WGS84 DEST.shp SRC.shp

Note that DEST file is specified before the source!


UNIT TESTING
============

Unit testing is performed using the Boost::Test framework. It can be compiled and linked in two ways:

- Dynamic linked, using the system installed boost::test library. This option is enabled by default on Unix
- Compiled in, using the boost/test/included/unit_test.hpp (included in main.cpp). This is suitable for Windows, where boost::test is not available with our version of mingw64. A standard installation of boost::test is required.

The two methods can be selected by defining boost_test_included in the CONFIG line of the Qt Project. It is enabled by default on Windows (see localconfig.pri).

If boost::test is not available in any form, it can be disabled by removing the units-test option from the CONFIG variable in the pro file.

