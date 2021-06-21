//
// Created by happycactus on 31/10/20.
//

#include "PersistentCache.h"

#include <QCryptographicHash>
#include <QMutex>
#include <QMutexLocker>
#include <QDateTime>
#include <QDebug>

struct PersistentCache::Impl {
    QMutex m_mutex;
    QDir m_persistent_cache_directory;
    std::chrono::minutes m_persistent_cache_expiry;
    long expirationTimeMs;

    QCryptographicHash hashFunction;

    explicit Impl(std::chrono::minutes expiry)
            : m_persistent_cache_expiry(expiry),
              hashFunction(QCryptographicHash::Algorithm::Md5)
    {
        expirationTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                m_persistent_cache_expiry).count();
    }

    QString hash(QUrl const &url)
    {
        hashFunction.reset();
        hashFunction.addData(url.toEncoded());

        auto hash = QString(hashFunction.result().toHex());
        return m_persistent_cache_directory.absolutePath() + QDir::separator() + hash;
    }

    bool fileHasExpired(QFile const &file)
    {
        return fileHasExpired(QFileInfo(file), QDateTime::currentDateTime());
    }

    bool fileHasExpired(QFileInfo file, QDateTime datetime)
    {
        auto timeToExpiry = file.lastModified().msecsTo(datetime);
        return (m_persistent_cache_expiry.count() > 0 && timeToExpiry > expirationTimeMs);
    }

    void removeFile(QFile const &file)
    {
        removeFile(file.fileName());
    }

    void removeFile(QFileInfo const &file)
    {
        removeFile(file.absoluteFilePath());
    }

    void removeFile(QString filename)
    {
        m_persistent_cache_directory.remove(filename);
    }
};

PersistentCache::PersistentCache(QDir cachePath, std::chrono::minutes expiration)
        : p(spimpl::make_unique_impl<Impl>(expiration))
{
    p->m_persistent_cache_directory = cachePath;
    if (!cachePath.mkpath(cachePath.absolutePath())) {
        //throw std::system_error(errno, std::system_category());
    }
}

QString PersistentCache::findPixmap(QUrl const &path, QPixmap &resource)
{
    auto hash = p->hash(path);
    QFile file(hash);
    if (!file.exists()) {
        return QString();
    }

    if (p->fileHasExpired(file)) {
        p->removeFile(file);
        return QString();
    }

    resource.load(hash);
    return hash;
}

QString PersistentCache::insertPixmap(const QUrl &url, const QPixmap &resource)
{
    auto hash = p->hash(url);

    if (!resource.save(p->m_persistent_cache_directory.absoluteFilePath(hash), "PNG")) {
        qDebug() << "ohoh " << p->m_persistent_cache_directory.absoluteFilePath(hash);
       // throw std::system_error(errno, std::system_category());
    }

    return hash;
}

void PersistentCache::startPersistentCacheHousekeeping()
{
    QMutexLocker locker(&p->m_mutex);

    auto allentries = p->m_persistent_cache_directory.entryInfoList();

    QStringList expiredFiles;
    auto currentTimeMs = QDateTime::currentDateTime();

    for (const auto &entry : allentries) {
        if (p->fileHasExpired(entry, currentTimeMs)) {
            expiredFiles.push_back(entry.fileName());
        }
    }

    for (const auto &fileToDelete : expiredFiles) {
        p->removeFile(fileToDelete);
    }
}

void PersistentCache::clearPersistentCache()
{
    QMutexLocker locker(&p->m_mutex);

    auto allentries = p->m_persistent_cache_directory.entryInfoList();

    for (auto entry : allentries) {
        p->removeFile(entry);
    }
}
