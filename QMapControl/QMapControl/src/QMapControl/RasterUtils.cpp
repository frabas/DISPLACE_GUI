//
// Created by happycactus on 07/08/20.
//

#include "RasterUtils.h"

#include <gdal_priv.h>
#include <QDebug>

QImage RasterUtils::imageFromRaster(GDALDataset *dataset)
{
    int rows = dataset->GetRasterYSize();
    int cols = dataset->GetRasterXSize();
    int channels = dataset->GetRasterCount();

    qDebug() << "Raster x,y,channels: " << rows << cols << channels;

    int scanlineSize = std::ceil(static_cast<float>(channels) * cols / 4.0) * 4;

    qDebug() << "Scanline: " << scanlineSize;

    char *data = new char[rows * scanlineSize];

    // Iterate over each channel
    std::vector<int> bands(channels);
    std::iota(bands.begin(), bands.end(), 1);

    auto conversionResult = dataset->RasterIO(GF_Read, 0, 0,
                                              cols, rows,
                                              data,
                                              cols, rows,
                                              GDT_Byte,
                                              bands.size(), bands.data(),
                                              bands.size(),            // pixelspace
                                              scanlineSize, // linespace
                                              (channels > 1 ? 1 : 0),            // bandspace
                                              nullptr);

    if (conversionResult != CE_None) {
        qWarning() << "GetRaster returned " << conversionResult;
        return QImage{};
    }

    QImage::Format imageFormat = QImage::Format::Format_RGB888;
    auto mainBand = dataset->GetRasterBand(1);
    GDALColorTable *ct;
    QVector<QRgb> newImageColorTable;
    if (mainBand && (ct = mainBand->GetColorTable())) {
        imageFormat = QImage::Format::Format_Indexed8;

        auto n = ct->GetColorEntryCount();
        qDebug() << "Image has: " << n << " colors as: "
                 << ct->GetPaletteInterpretation();
        for (int i = 0; i < n; ++i) {
            GDALColorEntry entry;
            ct->GetColorEntryAsRGB(i, &entry);
//            qDebug() << "Color: " << i << QString("%1").arg(qRgb(entry.c1, entry.c2, entry.c3), 8, 16, QChar('0'));
            newImageColorTable.push_back(
                    qRgb(entry.c1, entry.c2, entry.c3)
            );
        }
    }

    QImage image(reinterpret_cast<uchar *>(data), cols, rows, imageFormat);
    if (!newImageColorTable.empty()) {
        image.setColorTable(newImageColorTable);
    }

//    image.save("qimage.png");
    return image;
}
