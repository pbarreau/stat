#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QtGui>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QMdiArea>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <cstdio>

class QSslError;

using namespace std;

#include "mainwindow.h"
#include "choixjeux.h"

void MainWindow::pslot_newGame()
{
    ChoixJeux *selection = new ChoixJeux(this);
    selection->setModal(true);
    selection->show();
}

void MainWindow::pslot_open()
{
}

bool MainWindow::pslot_save()
{
    bool status = true;

    return status;
}

bool MainWindow::pslot_saveAs()
{
    bool status = true;

    return status;

}

void MainWindow::pslot_close()
{
    for(int i= 0; i<3; i++)
    {
        //delete(une_vue[i]);
    }
    close();
}

void MainWindow::pslot_GetFromFdj()
{
    manager = new QNetworkAccessManager(this);
    //QNetworkRequest urlCible;
    //QNetworkReply *reply;

    QStringList urlFdJeux = {
        "https://www.fdj.fr/generated/game/loto/loto2017.zip",
        "https://www.fdj.fr/generated/game/loto/nouveau_loto.zip",
        "https://www.fdj.fr/generated/game/loto/loto.zip",
        "https://www.fdj.fr/generated/game/loto/superloto2017.zip",
        "https://www.fdj.fr/generated/game/loto/nouveau_superloto.zip",
        "https://www.fdj.fr/generated/game/loto/superloto.zip",
        "https://www.fdj.fr/generated/game/loto/lotonoel2017.zip"
    };

    QStringList urlFdJeux_euro = {
        "https://www.fdj.fr/generated/game/euromillions/euromillions_4.zip",
        "https://www.fdj.fr/generated/game/euromillions/euromillions_3.zip",
        "https://www.fdj.fr/generated/game/euromillions/euromillions_2.zip",
        "https://www.fdj.fr/generated/game/euromillions/euromillions.zip"
    };

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slot_replyFinished(QNetworkReply*)));

    for (const QString &arg : urlFdJeux) {
        QUrl url = QUrl::fromEncoded(arg.toLocal8Bit());
        doDownload(url);
    }
}

void MainWindow::doDownload(const QUrl &url)
{
    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);


    currentDownloads.append(reply);
}

void MainWindow::slot_replyFinished(QNetworkReply *reply)
{
    QUrl url = reply->url();
    if (reply->error()) {
        fprintf(stderr, "Download of %s failed: %s\n",
                url.toEncoded().constData(),
                qPrintable(reply->errorString()));
    } else {
        if (isHttpRedirect(reply)) {
            fputs("Request was redirected.\n", stderr);
        } else {
            QString filename = saveFileName(url);
            if (saveToDisk(filename, reply)) {
                printf("Download of %s succeeded (saved to %s)\n",
                       url.toEncoded().constData(), qPrintable(filename));
            }
        }
    }

    currentDownloads.removeAll(reply);
    reply->deleteLater();

    if (currentDownloads.isEmpty()) {
        // all downloads finished
        QCoreApplication::instance()->quit();
    }



}

#if 0
void MainWindow::slot_sslErrors(const QList<QSslError> &sslErrors)
{
#if QT_CONFIG(ssl)
    for (const QSslError &error : sslErrors)
        fprintf(stderr, "SSL error: %s\n", qPrintable(error.errorString()));
#else
    Q_UNUSED(sslErrors);
#endif
}
#endif

QString MainWindow::saveFileName(const QUrl &url)
{
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "download.zip";

    if (QFile::exists(basename)) {
        // already exists
        if (QMessageBox::question(this, tr("HTTP"),
                                  tr("There already exists a file called %1 in "
                                     "the current directory. Overwrite?").arg(basename),
                                  QMessageBox::Yes|QMessageBox::No, QMessageBox::No)
                == QMessageBox::No){

            //don't overwrite
            int i = 0;
            basename += '.';
            while (QFile::exists(basename + QString::number(i)))
                ++i;

            basename += QString::number(i);
        }
        else
        {
            QFile::remove(basename);
        }
    }

    return basename;
}

bool MainWindow::saveToDisk(const QString &filename, QIODevice *data)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Could not open %s for writing: %s\n",
                qPrintable(filename),
                qPrintable(file.errorString()));
        return false;
    }

    file.write(data->readAll());
    file.close();

    return true;
}

bool MainWindow::isHttpRedirect(QNetworkReply *reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    return statusCode == 301 || statusCode == 302 || statusCode == 303
            || statusCode == 305 || statusCode == 307 || statusCode == 308;
}

void MainWindow::pslot_about()
{
    QMessageBox::about(this, tr("A propos de Prevision"),
                       tr("L'application <b>Prevision</b> charge une base loto/euro de la FDJ, "
                          "et cherche une interaction entres les boules des divers tirages ! "));
}

