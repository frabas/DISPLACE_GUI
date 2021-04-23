//
// Created by happycactus on 31/10/20.
//

#ifndef QMAPCONTROL_PERSISTENTCACHE_H
#define QMAPCONTROL_PERSISTENTCACHE_H

#include "utils/spimpl.h"

#include <QDir>
#include <QPixmap>
#include <QUrl>

#include <chrono>
#include <tuple>

/**
 * @brief A cache to persistently store the downloaded pixmaps.
 */
class PersistentCache {
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;
public:
    /**
     * @brief Create the cache in the proper directory and with the proper expiration timeout
     * @param cachePath
     * @param expiration
     */
    PersistentCache(QDir cachePath, std::chrono::minutes expiration);

    /**
     * @brief Look for the resource in the cache, returning it if found.
     * @param path
     * @return The ash of the resource found in the cache. In that case, the pixmap argument will contain the
     * resource. If the resource was not found, returns an empty string, and the pixmap argument is untouched.
     */
    QString findPixmap(QUrl const &path, QPixmap &resource);

    /**
     * @brief
     * @param url
     * @param resource
     * @return
     */
    QString insertPixmap(QUrl const &url, QPixmap const &resource);

    /**
     * @brief Starts the Persistent cache housekeeping. Scan the current cache for expired enties, and remove them.
     * This function should be called just after the enablePersistentCache(), preferably in a separate thread using
     * QtConcurrent for example.
     * Indeed the persistentCacheFind only removes the files that are searched for, this means that
     * even if a file has expired, it will remain in cache forever if never accessed.
     */
    void startPersistentCacheHousekeeping();

    /**
     * @brief Remove all the files in the persistent cache, regardless of the expiration.
     */
    void clearPersistentCache();


};


#endif //QMAPCONTROL_PERSISTENTCACHE_H
