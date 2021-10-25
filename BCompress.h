#ifndef BCOMPRESS_H
#define BCOMPRESS_H

/// http://3adly.blogspot.com/2011/06/qt-folder-compression.html

#include <QFile>
#include <QObject>
#include <QDir>
#include <QDataStream>

class BCompress : public QObject
{
    Q_OBJECT
public:
    explicit BCompress(QObject *parent = 0);

    //A recursive function that scans all files inside the source folder
    //and serializes all files in a row of file names and compressed
    //binary data in a single file
    bool compressFolder(QString sourceFolder, QString destinationFile);

    //A function that deserializes data from the compressed file and
    //creates any needed subfolders before saving the file
    bool decompressFolder(QString sourceFile, QString destinationFolder);

private:
    QFile file;
    QDataStream dataStream;

    bool compress(QString sourceFolder, QString prefex);
};

#endif // BCOMPRESS_H
