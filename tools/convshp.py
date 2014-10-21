#!/usr/bin/python

from osgeo import ogr, osr
import os
import sys

driver = ogr.GetDriverByName('ESRI Shapefile')

# get the input layer
inDataSet = driver.Open(sys.argv[1])
inLayer = inDataSet.GetLayer()

# input SpatialReference
inSpatialRef = inLayer.GetSpatialRef()

# output SpatialReference
outSpatialRef = osr.SpatialReference()
outSpatialRef.SetWellKnownGeogCS( "WGS84" );

# create the CoordinateTransformation
coordTrans = osr.CoordinateTransformation(inSpatialRef, outSpatialRef)

# create the output layer
outputShapefile = sys.argv[2]
if os.path.exists(outputShapefile):
    driver.DeleteDataSource(outputShapefile)
outDataSet = driver.CreateDataSource(outputShapefile)
outLayer = outDataSet.CreateLayer("wgs84", geom_type=ogr.wkbMultiPolygon)

# add fields
inLayerDefn = inLayer.GetLayerDefn()
for i in range(0, inLayerDefn.GetFieldCount()):
    fieldDefn = inLayerDefn.GetFieldDefn(i)
    outLayer.CreateField(fieldDefn)

# get the output layer's feature definition
outLayerDefn = outLayer.GetLayerDefn()

# loop through the input features
inFeature = inLayer.GetNextFeature()
while inFeature:
    # get the input geometry
    geom = inFeature.GetGeometryRef()
    # reproject the geometry
    geom.Transform(coordTrans)
    # create a new feature
    outFeature = ogr.Feature(outLayerDefn)
    # set the geometry and attribute
    outFeature.SetGeometry(geom)
    for i in range(0, outLayerDefn.GetFieldCount()):
        outFeature.SetField(outLayerDefn.GetFieldDefn(i).GetNameRef(), inFeature.GetField(i))
    # add the feature to the shapefile
    outLayer.CreateFeature(outFeature)
    # destroy the features and get the next input feature
    outFeature.Destroy()
    inFeature.Destroy()
    inFeature = inLayer.GetNextFeature()

# close the shapefiles
inDataSet.Destroy()
outDataSet.Destroy()
