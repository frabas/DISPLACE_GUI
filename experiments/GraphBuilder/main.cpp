#include <iostream>

#include <gdal/ogrsf_frmts.h>
#include <gdal/cpl_conv.h> // for CPLMalloc()

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

    const char* path = "sample.shp";
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

    /////// Here add all points required

    for (double y = -3; y < 3.1; y+=0.5) {
        for (double x = -3; x < 3.1; x+= 0.5) {
            OGRPoint pt(x,y);
            auto f = OGRFeature::CreateFeature(ptlayer->GetLayerDefn());
            f->SetGeometry(&pt);
            if (ptlayer->CreateFeature(f) != OGRERR_NONE) {
                cerr << "Cannot Create Point\n";
                return 1;
            }
            OGRFeature::DestroyFeature(f);
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

    OGRDataSource::DestroyDataSource(indataset);

    return 0;
}