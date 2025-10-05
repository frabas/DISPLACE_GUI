/*
 *
 * This file is part of QMapControl,
 * an open-source cross-platform map widget
 *
 * Copyright (C) 2009 - Federico Fuga <fuga@studiofuga.com>
 * Copyright (C) 2007 - 2008 Kai Winter
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with QMapControl. If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact e-mail: fuga@studiofuga.com
 * Contact e-mail: kaiwinter@gmx.de
 * Program URL   : http://qmapcontrol.sourceforge.net/
 *
 */

#include "ImageManager.h"
#include "Projection.h"

#include <QDateTime>
#include <QtCore/QCryptographicHash>
#include <QtGui/QPainter>

namespace qmapcontrol
{
    namespace
    {
        /// Singleton instance of Image Manager.
        std::unique_ptr<ImageManager> m_instance = nullptr;
    }

    ImageManager& ImageManager::get()
    {
        // Does the singleton instance exist?
        if(m_instance == nullptr)
        {
            // Create a default instance (256px tiles).
            m_instance.reset(new ImageManager(256));
        }

        // Return the reference to the instance object.
        return *(m_instance.get());
    }

    void ImageManager::destory()
    {
        // Ensure the singleton instance is destroyed.
        m_instance.reset(nullptr);
    }

    ImageManager::ImageManager(const int& tile_size_px, QObject* parent)
        : QObject(parent),
          m_tile_size_px(tile_size_px),
          m_pixmap_loading(),
          m_batch_mode(false),
          m_batch_initial_queue_size(0),
          m_batch_had_downloads(false)
    {
        // Setup a loading pixmap.
        setupLoadingPixmap();

        // Connect signal/slot for image downloads.
        QObject::connect(this, &ImageManager::downloadImage, &m_nm, &NetworkManager::downloadImage);
        QObject::connect(&m_nm, &NetworkManager::imageDownloaded, this, &ImageManager::imageDownloaded);
        QObject::connect(&m_nm, &NetworkManager::downloadingInProgress, this, &ImageManager::downloadingInProgress);
        QObject::connect(&m_nm, &NetworkManager::downloadingFinished, this, &ImageManager::onDownloadingFinished);
    }

    int ImageManager::tileSizePx() const
    {
        // Return the tiles size in pixels.
        return m_tile_size_px;
    }

    void ImageManager::setTileSizePx(const int& tile_size_px)
    {
        // Set the new tile size.
        m_tile_size_px = tile_size_px;

        // Create a new loading pixmap.
        setupLoadingPixmap();
    }

    void ImageManager::setProxy(const QNetworkProxy& proxy)
    {
        // Set the proxy on the network manager.
        m_nm.setProxy(proxy);
    }

    bool ImageManager::enablePersistentCache(const std::chrono::minutes& expiry, const QDir& path)
    {
        m_disk_cache = std::make_unique<PersistentCache>(path, expiry);
        return true;
    }

    void ImageManager::abortLoading()
    {
        // Abort any remaing network manager downloads.
        m_nm.abortDownloads();
    }

    int ImageManager::loadQueueSize() const
    {
        // Return the network manager downloading queue size.
        return m_nm.downloadQueueSize();
    }

    QPixmap ImageManager::getImage(const QUrl& url)
    {
        // Holding resource for image to be loaded into.
        QPixmap return_pixmap(m_pixmap_loading);

        // Is the image already been downloaded by the network manager?
        if(m_nm.isDownloading(url) == false)
        {
            // Is the image in our volatile "in-memory" cache?
            const auto find_itr = m_pixmap_cache.find(md5hex(url));
            if (find_itr != m_pixmap_cache.end()) {
                // Set the return image to the "in-memory" cached version.
                return_pixmap = find_itr->second;
            }
                // Is the persistent cache enabled?
            else if (m_disk_cache != nullptr) {
                auto hash = m_disk_cache->findPixmap(url, return_pixmap);
                if (!hash.isEmpty()) {
                    m_pixmap_cache[hash] = return_pixmap;
                } else {
                    qDebug() << "Url: " << url << " Requires download";
                    if (m_batch_mode) {
                        if (!m_batch_urls.contains(url)) {
                            m_batch_urls.append(url);
                        }
                        m_batch_had_downloads = true;
                    }
                    emit downloadImage(url);
                }
            } else {
                // Emit that we need to download the image using the network manager.
                if (m_batch_mode) {
                    if (!m_batch_urls.contains(url)) {
                        m_batch_urls.append(url);
                    }
                    m_batch_had_downloads = true;
                }
                emit downloadImage(url);
            }
        } else if (m_batch_mode && !m_batch_urls.contains(url)) {
            // Image is already being downloaded, but track it for batch completion
            m_batch_urls.append(url);
            m_batch_had_downloads = true;
        }

        // Default return the image.
        return return_pixmap;
    }

    QPixmap ImageManager::prefetchImage(const QUrl& url)
    {
        QMutexLocker locker(&mMutex);

        // Add the url to the prefetch list.
        m_prefetch_urls.append(url);

        // Return the image for the url.
        return getImage(url);
    }

    void ImageManager::setLoadingPixmap(const QPixmap &pixmap)
    {
        m_pixmap_loading = pixmap;
    }

    void ImageManager::imageDownloaded(const QUrl& url, const QPixmap& pixmap)
    {
#ifdef QMAP_DEBUG
        qDebug() << "ImageManager::imageDownloaded '" << url << "' " << md5hex(url);
#endif

        m_pixmap_cache[md5hex(url)] = pixmap;

        if (m_disk_cache) {
            m_disk_cache->insertPixmap(url, pixmap);
        }

        // Is this a prefetch request?
        if (m_prefetch_urls.contains(url)) {
            // Remove the url from the prefetch list.
            m_prefetch_urls.removeAt(m_prefetch_urls.indexOf(url));
        } else if (m_batch_mode) {
            // In batch mode, remove from batch urls list
            if (m_batch_urls.contains(url)) {
                m_batch_urls.removeAll(url);
#ifdef QMAP_DEBUG
                qDebug() << "ImageManager: Batch URL downloaded. Remaining:" << m_batch_urls.size()
                         << "Queue:" << m_nm.downloadQueueSize();
#endif
            }
            // Don't emit individual updates in batch mode, wait for batch completion
        } else {
            // Let the world know we have received an updated image.
            emit imageUpdated(url);
        }
    }

    void ImageManager::setupLoadingPixmap()
    {
        // Create a new pixmap.
        m_pixmap_loading = QPixmap(m_tile_size_px, m_tile_size_px);

        // Make is transparent.
        m_pixmap_loading.fill(Qt::transparent);

        // Add a pattern.
        QPainter painter(&m_pixmap_loading);
        QBrush brush(Qt::lightGray, Qt::Dense5Pattern);
        painter.fillRect(m_pixmap_loading.rect(), brush);

        // Add "LOADING..." text.
        painter.setPen(Qt::black);
        painter.drawText(m_pixmap_loading.rect(), Qt::AlignCenter, "LOADING...");
    }

QString ImageManager::md5hex(const QUrl &url)
{
    // Return the md5 hex value of the given url at a specific projection and tile size.
    return QString(QCryptographicHash::hash(
            (url.toString() + QString::number(projection::get().epsg()) + QString::number(m_tile_size_px)).toUtf8(),
            QCryptographicHash::Md5).toHex());
}

void ImageManager::startPersistentCacheHousekeeping()
{
    if (m_disk_cache != nullptr) {
        m_disk_cache->startPersistentCacheHousekeeping();
    }
}

void ImageManager::clearPersistentCache()
{
    if (m_disk_cache != nullptr) {
        m_disk_cache->clearPersistentCache();
    }
}

void ImageManager::beginBatchDownload()
{
#ifdef QMAP_DEBUG
    qDebug() << "ImageManager: Beginning batch download mode";
#endif
    m_batch_mode = true;
    m_batch_urls.clear();
    m_batch_initial_queue_size = m_nm.downloadQueueSize();
    m_batch_had_downloads = false;
}

void ImageManager::endBatchDownload()
{
#ifdef QMAP_DEBUG
    qDebug() << "ImageManager: Ending batch download mode. Pending URLs:" << m_batch_urls.size()
             << "Queue size:" << m_nm.downloadQueueSize()
             << "Had downloads:" << m_batch_had_downloads;
#endif

    // If no downloads were requested during this batch, just exit batch mode without emitting
    if (!m_batch_had_downloads) {
#ifdef QMAP_DEBUG
        qDebug() << "ImageManager: No downloads in this batch, exiting batch mode silently";
#endif
        m_batch_mode = false;
        m_batch_urls.clear();
        return;
    }

    // If we had downloads but batch URLs list is empty AND queue is empty,
    // all tiles were downloaded very quickly (from disk cache probably)
    if (m_batch_urls.isEmpty() && m_nm.downloadQueueSize() == 0) {
#ifdef QMAP_DEBUG
        qDebug() << "ImageManager: All tiles already downloaded/cached, emitting imageUpdated immediately";
#endif
        m_batch_mode = false;
        m_batch_had_downloads = false;
        emit imageUpdated(QUrl());
        return;
    }

    // Otherwise, we have pending downloads - batch mode stays active
    // until downloadingFinished signal is received or all URLs are downloaded
#ifdef QMAP_DEBUG
    qDebug() << "ImageManager: Waiting for downloads. Batch URLs:" << m_batch_urls.size()
             << "Queue:" << m_nm.downloadQueueSize();
#endif
}

void ImageManager::onDownloadingFinished()
{
#ifdef QMAP_DEBUG
    qDebug() << "ImageManager::onDownloadingFinished - Batch mode:" << m_batch_mode
             << "Pending URLs:" << m_batch_urls.size()
             << "Had downloads:" << m_batch_had_downloads;
#endif

    if (m_batch_mode && m_batch_had_downloads) {
        // All downloads finished, emit the batch complete signal
#ifdef QMAP_DEBUG
        qDebug() << "ImageManager: Batch download finished, emitting imageUpdated";
#endif
        m_batch_mode = false;
        m_batch_urls.clear();
        m_batch_had_downloads = false;
        emit imageUpdated(QUrl());
    } else {
        // Forward the signal as normal (only if not in batch mode or no downloads happened)
        if (!m_batch_mode) {
            emit downloadingFinished();
        }
    }
}

}
