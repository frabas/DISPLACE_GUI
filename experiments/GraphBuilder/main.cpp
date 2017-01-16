#include <iostream>

#include <ogrsf_frmts.h>
#include <cpl_conv.h> // for CPLMalloc()

using namespace std;

#if defined(BUILD_MACOS)
#define PATH HOME "/temp"
#elif defined (BUILD_LINUX)
#define PATH  HOME "/samples"
#endif

#define SHP PATH "/sample.shp"

struct Point {
    double x;
    double y;
    Point (double xi, double yi)
            : x(xi), y(yi) {}
};

const Point InitF1[] = {
        { -2, -2 },
        {  2, -2},
        {  2,  0},
        {  0,  0},
        {  2,  2},
        { -2,  2},
        { -2, -2}
};

int main()
{
    // Create the basic shape

    string drv_shapefile = "ESRI Shapefile";
    string drv_memory = "memory";

    auto shp_type = wkbPolygon;
    auto shp_memory = wkbGeometryCollection;

    string indrivername = drv_shapefile;
    auto type = shp_memory;

    mkdir(PATH, 0700);

    const char* path = SHP;
    OGRRegisterAll();
    OGRSFDriverRegistrar *registrar =  OGRSFDriverRegistrar::GetRegistrar();
    auto indriver = registrar->GetDriverByName(indrivername.c_str());

    if( indriver == NULL )
    {
        cerr << indrivername << " Not available\n";
        return 1;
    }

    auto *file = fopen(path, "r");
    if (file != nullptr) {
        fclose(file);
        indriver->DeleteDataSource(path);
        //indriver->Delete(path);
    }

    OGRSpatialReference sr;
    sr.SetWellKnownGeogCS( "WGS84" );

    auto indataset = indriver->CreateDataSource(path, nullptr );

    char     **papszOptions;
    papszOptions = CSLSetNameValue( papszOptions, "SHPT", "ARC" );

    auto layerShape = indataset->CreateLayer("shape", &sr, wkbPolygon, nullptr);

    OGRFieldDefn oField( "Name", OFTString );
    oField.SetWidth(32);
    if( layerShape->CreateField( &oField ) != OGRERR_NONE )
    {
        printf( "Creating Name field failed.\n" );
        exit( 1 );
    }

    /////
    OGRFeature *poFeature;
    poFeature = OGRFeature::CreateFeature( layerShape->GetLayerDefn() );
    poFeature->SetField( "Name", "ft1" );

    int n = sizeof(InitF1) / sizeof(InitF1[0]);
    OGRLinearRing ring;
    for (int i = 0; i < n; ++i) {
        ring.addPoint( InitF1[i].x, InitF1[i].y );
    }
    OGRPolygon poly;
    poly.addRing(&ring);
    poFeature->SetGeometry( &poly );

    if (layerShape->CreateFeature(poFeature) != OGRERR_NONE) {
        cerr << "Cannot create feature.\n";
        return 1;
    }

    OGRFeature::DestroyFeature(poFeature);

    auto layerPoints = indataset->CreateLayer("points", &sr, wkbPoint, nullptr);

    auto layerGrid = indataset->CreateLayer("Grid", &sr, wkbLineString, papszOptions);
    /////// Here add all points required

    const double stp = 0.5;
    for (double y = -3.2; y < 3.2; y+=stp) {
        for (double x = -3.1; x < 3.2; x+= stp) {
            OGRPoint pt(x,y);
            auto f = OGRFeature::CreateFeature(layerPoints->GetLayerDefn());
            f->SetGeometry(&pt);
            if (layerPoints->CreateFeature(f) != OGRERR_NONE) {
                cerr << "Cannot Create Point\n";
                return 1;
            }
            OGRFeature::DestroyFeature(f);

            if (x > -3.2) {
                OGRLineString line;
                line.addPoint(x-stp, y);
                line.addPoint(x,y);

                auto f = OGRFeature::CreateFeature(layerGrid->GetLayerDefn());
                f->SetGeometry(&line);
                if (layerGrid->CreateFeature(f) != OGRERR_NONE) {
                    cerr << "Cannot create Line\n";
                    return 1;
                }
                OGRFeature::DestroyFeature(f);
            }

            if (y > -3.2) {
                OGRLineString line;
                line.addPoint(x,y-stp);
                line.addPoint(x,y);

                auto f = OGRFeature::CreateFeature(layerGrid->GetLayerDefn());
                f->SetGeometry(&line);
                if (layerGrid->CreateFeature(f) != OGRERR_NONE) {
                    cerr << "Cannot create Line\n";
                    return 1;
                }
                OGRFeature::DestroyFeature(f);
            }
        }
    }

    std::cerr << "Clipping1\n";
    auto layerInPoly = indataset->CreateLayer("inpoly", &sr, wkbPoint, nullptr);
    if (layerPoints->Clip(layerShape, layerInPoly, nullptr, nullptr, nullptr) != OGRERR_NONE) {
        cerr << "Error clipping\n";
    }

    std::cerr << "Diffing\n";
    auto layerOutPoly = indataset->CreateLayer("outpoly", &sr, wkbPoint, nullptr);
    if (layerPoints->SymDifference(layerInPoly, layerOutPoly, nullptr, nullptr, nullptr) != OGRERR_NONE) {
        cerr << "Error clipping\n";
    }

    char     **clipOptions = nullptr;
    clipOptions = CSLSetNameValue( clipOptions, "SKIP_FAILURES", "YES" );

    std::cerr << "Clipping2\n";
    auto layerGridOut = indataset->CreateLayer("gridout", &sr, wkbLineString, papszOptions);
    if (layerGrid->Clip(layerShape, layerGridOut, clipOptions, nullptr, nullptr) != OGRERR_NONE) {
        cerr << "Cannot clip Grid Layer";
    }

    OGRDataSource::DestroyDataSource(indataset);

    return 0;
}