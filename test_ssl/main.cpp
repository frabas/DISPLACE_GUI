// main.cpp


#include <QtWidgets/QApplication>
#include "QLabel"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QTimer>
#include <iostream>

// Helper to print OpenSSL availability
static void checkSslSupport()
{
    // Force Qt to load the SSL backend now
    QSslSocket socket;
    if (!socket.supportsSsl()) {
        std::cerr << "❌ OpenSSL not available – TLS will fail.\n";
        std::cerr << "   Ensure libssl-3.dll / libcrypto-3.dll are reachable.\n";
        return;
    }

    std::cout << "✅ OpenSSL loaded successfully.\n";
    std::cout << "   Qt built with OpenSSL version: "
        << QSslSocket::sslLibraryVersionString().toStdString() << "\n";
}

// Simple HTTPS GET request
static void performRequest(QNetworkAccessManager* nam)
{
    const QUrl url(QStringLiteral("https://www.qt.io"));
    QNetworkRequest request(url);

    // Optional: enforce TLS 1.2+ (Qt defaults to the highest supported)
    QSslConfiguration sslConf = request.sslConfiguration();
    sslConf.setProtocol(QSsl::TlsV1_2OrLater);
    request.setSslConfiguration(sslConf);

    QNetworkReply* reply = nam->get(request);
    QObject::connect(reply, &QNetworkReply::finished, [reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            std::cerr << "❌ Request failed: "
                << reply->errorString().toStdString() << "\n";
        }
        else {
            std::cout << "✅ Received " << reply->size()
                << " bytes from " << reply->url().toString().toStdString()
                << "\n";
        }
        reply->deleteLater();
        // Exit the app after the demo
        QCoreApplication::quit();
        });
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // Show a tiny UI so the exe isn’t “console‑only” on Windows
    QLabel label(QStringLiteral("Qt TLS demo – check console output"));
    label.resize(400, 50);
    label.show();

    // 1️⃣ Verify OpenSSL presence early
    checkSslSupport();

    // 2️⃣ Kick off a real HTTPS request
    auto* nam = new QNetworkAccessManager(&app);
    QTimer::singleShot(0, [&]() { performRequest(nam); });

    return app.exec();
}


/*
#include <QCoreApplication>
#include <QtNetwork/QSslSocket>
#include <QSslSocket>
#include <QDebug>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "Compiled‑in SSL feature?" << (QT_FEATURE_ssl ? "yes" : "no");

    // Verify that SSL is actually usable at runtime
    if (!QSslSocket::supportsSsl()) {
        qWarning() << "SSL support is not available at runtime";
        return -1;
    }

    
    
  return 0;   // we don’t need an event loop for this demo
}

*/

