#include <iostream>

#include <ogrsf_frmts.h>
#include <cpl_conv.h> // for CPLMalloc()

using namespace std;

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

    const char* path = "/Users/HappyCactus/temp/sample.shp";
    OGRRegisterAll();
    OGRSFDriverRegistrar *registrar =  OGRSFDriverRegistrar::GetRegistrar();
    auto indriver = registrar->GetDriverByName(indrivername.c_str());

    //auto indriver = GDALDriverManager().GetDriverByName(indrivername.c_str());

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

    auto indataset = indriver->CreateDataSource(path, nullptr );
    //auto indataset = indriver->Create(path, 0, 0, 0, GDT_Unknown, NULL );

    auto inlayer = indataset->CreateLayer("base", nullptr, wkbPolygon, nullptr);

    OGRFieldDefn oField( "Name", OFTString );
    oField.SetWidth(32);
    if( inlayer->CreateField( &oField ) != OGRERR_NONE )
    {
        printf( "Creating Name field failed.\n" );
        exit( 1 );
    }

    /////
    OGRFeature *poFeature;
    poFeature = OGRFeature::CreateFeature( inlayer->GetLayerDefn() );
    poFeature->SetField( "Name", "ft1" );

    int n = sizeof(InitF1) / sizeof(InitF1[0]);
    OGRLinearRing ring;
    for (int i = 0; i < n; ++i) {
        ring.addPoint( InitF1[i].x, InitF1[i].y );
    }
    OGRPolygon poly;
    poly.addRing(&ring);
    poFeature->SetGeometry( &poly );

    if (inlayer->CreateFeature(poFeature) != OGRERR_NONE) {
        cerr << "Cannot create feature.\n";
        return 1;
    }

    OGRFeature::DestroyFeature(poFeature);

    auto ptlayer = indataset->CreateLayer("points", nullptr, wkbPoint, nullptr);

    char     **papszOptions;
    papszOptions = CSLSetNameValue( papszOptions, "SHPT", "ARC" );

    auto gridlayer = indataset->CreateLayer("Grid", nullptr, wkbLineString, papszOptions);
    /////// Here add all points required

    const double stp = 0.5;
    for (double y = -3; y < 3.1; y+=stp) {
        for (double x = -3; x < 3.1; x+= stp) {
            OGRPoint pt(x,y);
            auto f = OGRFeature::CreateFeature(ptlayer->GetLayerDefn());
            f->SetGeometry(&pt);
            if (ptlayer->CreateFeature(f) != OGRERR_NONE) {
                cerr << "Cannot Create Point\n";
                return 1;
            }
            OGRFeature::DestroyFeature(f);

            if (x > -3.2) {
                OGRLineString line;
                line.addPoint(x-stp, y);
                line.addPoint(x,y);

                auto f = OGRFeature::CreateFeature(gridlayer->GetLayerDefn());
                f->SetGeometry(&line);
                if (gridlayer->CreateFeature(f) != OGRERR_NONE) {
                    cerr << "Cannot create Line\n";
                    return 1;
                }
                OGRFeature::DestroyFeature(f);
            }

            if (y > -3.2) {
                OGRLineString line;
                line.addPoint(x,y-stp);
                line.addPoint(x,y);

                auto f = OGRFeature::CreateFeature(gridlayer->GetLayerDefn());
                f->SetGeometry(&line);
                if (gridlayer->CreateFeature(f) != OGRERR_NONE) {
                    cerr << "Cannot create Line\n";
                    return 1;
                }
                OGRFeature::DestroyFeature(f);
            }
        }
    }

    auto outlayer = indataset->CreateLayer("inpoly", nullptr, wkbPoint, nullptr);
    if (ptlayer->Clip(inlayer, outlayer, nullptr, nullptr, nullptr) != OGRERR_NONE) {
        cerr << "Error clipping\n";
    }

    auto dellayer = indataset->CreateLayer("outpoly", nullptr, wkbPoint, nullptr);
    if (ptlayer->SymDifference(outlayer, dellayer, nullptr, nullptr, nullptr) != OGRERR_NONE) {
        cerr << "Error clipping\n";
    }

    auto outgridlayer = indataset->CreateLayer("grid-oud", nullptr, wkbLineString, papszOptions);
    if (gridlayer->Clip(inlayer, outgridlayer, nullptr, nullptr, nullptr) != OGRERR_NONE) {
        cerr << "Cannot clip Grid Layer";
    }

    OGRDataSource::DestroyDataSource(indataset);

    return 0;
}