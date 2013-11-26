//
// C++ Implementation: IMQ  Back-up dir from server 
// usage:
/* 
./imq
FreeSpace home:210.07 GB
Usage: imq (Options)  
 {1} Options:
	--read or -r /foldernameA /foldernameB ... : Read folder name to make an imq image.
	--outfile or -o outputfile dir /foldernameA /foldernameB ... : Read folder name to make an outputfile.imq image.
	--net or -n remoteurl: Download a image file or whatever file.
	--decompress or -d file.imq: open imagefile imq from cd down.. recursive.
	--version or -V: show the version of conversion used
	--help or -h: display this text.
 */
 
 
// Author: Peter Hohl <pehohlva@gmail.com>,    26.11.2013
// http://www.freeroad.ch/
// Copyright: See COPYING file that comes with this distribution

#include "OsFile.h"



static bool invalidname(const QString pwd) {
    if (pwd.indexOf("/.Trash") > -1) {
        return false;
    }
    if (pwd.indexOf("/.git") > -1) {
        return false;
    }
    if (pwd.indexOf("/.svn") > -1) {
        return false;
    }
    if (pwd.indexOf("/.DS_Store") > -1) {
        return false;
    }
    return true;
}

static QString bytesToSize(const qint64 size) {
    if (size < 0)
        return QString();
    if (size < 1024)
        return QObject::tr("%1 B").arg(QString::number(((double) size), 'f', 0));
    if ((size >= 1024) && (size < 1048576))
        return QObject::tr("%1 KB").arg(QString::number(((double) size) / 1024, 'f', 0));
    if ((size >= 1048576) && (size < 1073741824))
        return QObject::tr("%1 MB").arg(QString::number(((double) size) / 1048576, 'f', 2));
    if (size >= 1073741824)
        return QObject::tr("%1 GB").arg(QString::number(((double) size) / 1073741824, 'f', 2));
    return QString();
}



const int pointo = 76;
#define __DOCVERSION__ \
              QString("Ve.0.2.1")

#define __APPNAME__ \
              QString("Backup compressor.")

class Compressor : public QObject {

    Q_OBJECT
public:
    enum {
        MAGICNUMBER = 0xFFFAFFAA, VERSION = 10
    };
    Compressor(const int modus);
    void doDownload(const QUrl &url);

    void SetName(const QString n) {
        nameimage = QString("%1.imq").arg(n);
    }
public slots:
    void execute();
    void get_remote_file(const QString &httpfile);
    void downloadFinished(QNetworkReply *reply);
    void downloadProgress(qint64 r, qint64 tot);
    bool appendDirname(const QString pwd);
    bool appendFilename(QFileInfo file, const QString dir);
private:
    //// download like wget or curl 
    bool saveToDisk(const QString &filename, QIODevice *data);
    QString saveFileName(const QUrl &url, int full = 0);
    void open_imq(const QString imrqm);

    QStringList fulldirimager;
    QString nameimage;
    QFile *mainfilewrite;
    QDataStream writtel;
    int fcursor;
    QNetworkAccessManager manager;
    QList<QNetworkReply *> currentDownloads;
    int wmodus;
};

void Compressor::downloadProgress(qint64 r, qint64 tot) {

    QTextStream xc(stdout);
    xc << "In:" << bytesToSize(r) << "|" << bytesToSize(tot) << "\r";
    xc.flush();
}

void Compressor::doDownload(const QUrl &url) {
    QTextStream out(stdout);
    out << "doDownload:" << url.toString();
    out << "\n";
    out.flush();

    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));
}

Compressor::Compressor(const int modus) {
    wmodus = modus;
    fcursor = -1;
    /* 
    QTextStream out(stdout);
    out << "wmodus:" << wmodus;
    out << "\n";
    out.flush();
     */
    QDateTime timer0(QDateTime::currentDateTime());
    const QString name = timer0.toString("dd.MM.yyyy.HH.mm");
    SetName(name);
    if (wmodus == 1) {

    } else if (wmodus == 2) {
        /// read remote qimage and decompress 
    } else if (wmodus == 3) {
        /// download only 
        connect(&manager, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)));
    }
}

void Compressor::open_imq(const QString imrqm) {
    QTextStream out(stdout);
    out << "init read:" << imrqm << "\n";
    QFile *f = new QFile(imrqm);
    if (f->open(QIODevice::ReadOnly)) {
        quint32 magic, version;
        QByteArray signature;
        QDataStream ds(f);
        ds.setVersion(QDataStream::Qt_4_2);
        ds >> signature;
        if (!signature.contains(QByteArray("IMQ_BIN_FILE"))) {
            f->close();
            return;
        }
        ds >> magic;
        if ((quint32) MAGICNUMBER != magic) {
            qWarning() << "######## Compressor::MAGICNUMBER not ok  " << magic;
            f->close();
            return;
        }
        ds >> version;
        if ((quint32) VERSION != version) {
            qWarning() << "######## Compressor::VERSION not ok  " << version;
            f->close();
            return;
        }
        out << "File ok pass in read ...." << imrqm << "\n";
        QString qpwd = QDir::currentPath();
        while (!ds.atEnd()) {
            OsFile data;
            ds >> data;
            if (data.modus() == 1) {
                const QString fdest = qpwd + data.name();
                if (data.saveDest(fdest)) {
                    out << "File extract to:" << fdest << "\n";
                } else {
                    out << "Unable to write in:" << fdest << "\n";
                }
                out.flush();

            } else if (data.modus() == 3) {
                ////  out << "Dir:" << data.name() << "\n";
            }
        }
        f->close();
    }
}

void Compressor::downloadFinished(QNetworkReply *reply) {
    QUrl url = reply->url();
    if (reply->error()) {
        fprintf(stderr, "Download of %s failed: %s\n",
                url.toEncoded().constData(),
                qPrintable(reply->errorString()));
        QCoreApplication::instance()->quit();
    } else {
        QString filename = saveFileName(url);
        if (saveToDisk(filename, reply)) {
            QCoreApplication::instance()->quit();
        }
    }

}

bool Compressor::saveToDisk(const QString &filename, QIODevice *data) {
    QFile file(filename);
    if (file.exists()) {
        fprintf(stderr, "Overwriting file: %s\n",
                qPrintable(filename));
    }
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

void Compressor::get_remote_file(const QString &httpfile) {
    doDownload(QUrl(httpfile));
}

QString Compressor::saveFileName(const QUrl &url, int full) {
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();
    if (basename.isEmpty()) {
        return QString("NullName");
    }
    return basename;
}

void Compressor::execute() {
    QStringList args = QCoreApplication::instance()->arguments();
    args.takeFirst();
    if (args.isEmpty()) {
        QCoreApplication::instance()->quit();
        return;
    }

    QTextStream out(stdout);
    if (wmodus == 1) {
        /// read dirs 

        QByteArray signature("IMQ_BIN_FILE");
        /// read and compress foder to qimage 
        mainfilewrite = new QFile(nameimage);
        if (mainfilewrite->open(QFile::WriteOnly)) {
            writtel.setDevice(mainfilewrite);
            writtel.setVersion(QDataStream::Qt_4_2);
            writtel << signature;
            writtel << (quint32) Compressor::MAGICNUMBER;
            writtel << (quint32) Compressor::VERSION;
            /// write os info from image 
        }



        fulldirimager = args;

        foreach(QString adir, fulldirimager) {
            QDir hdir(QString(adir.toLocal8Bit()));
            if (hdir.exists() && hdir.makeAbsolute()) {
                out << "Dir Handle:" << hdir.absolutePath();
                out << "\n";
                out.flush();
                if (appendDirname(hdir.absolutePath())) {
                    out << "Ready Dir Handle:" << hdir.absolutePath();
                    out << "\n";
                    out.flush();
                }
            }
        }
        if (writtel.device()) {
            writtel.device()->close();

            QFileInfo zinfo(nameimage);

            out << "Close file:" << nameimage << " " << bytesToSize(zinfo.size());
            out << "\n";
            out.flush();
        }
        QCoreApplication::instance()->quit();

    } else if (wmodus == 3) {
        /// download urls
        args.takeFirst();
        //// args.takeFirst(); /// -n or download

        foreach(QString urls, args) {
            QUrl url(urls.toLocal8Bit());
            if (url.isValid()) {
                doDownload(url);
            }
        }
    } else if (wmodus == 2) {
        /// explode - read image 
        args.takeFirst();
        //// args.takeFirst(); /// -n or download
        if (args.size() == 1) {
            QString fileimq(args.at(0).toLocal8Bit());
            QFileInfo imqrec(fileimq);
            if (imqrec.exists()) {
                open_imq(fileimq);
                out << "Read imq file:" << fileimq;
                out << "\n";
                out.flush();
            }
        }
        QCoreApplication::instance()->quit();
    }



}

bool Compressor::appendFilename(QFileInfo ifile, const QString dir) {
    QTextStream out(stdout);
    const QString full = dir + "/" + ifile.fileName();
    OsFile filefromOS;
    if (!invalidname(full)) {
        return false;
    }
    out << "read:" << full << "\n";
    QByteArray item;
    QFile *f = new QFile(full);
    if (f->exists()) {
        fcursor++;
        filefromOS.set_File(full, 1);
        if (writtel.device()) {
            writtel << filefromOS;
        }

    }
    return true;
}

bool Compressor::appendDirname(const QString pwd) {
    QTextStream out(stdout);
    if (!invalidname(pwd)) {
        return false;
    }
    bool result = true;
    QDir dir(pwd);
    if (dir.exists(pwd)) {
        OsFile dirfromOS;
        dirfromOS.set_File(dir.absolutePath(), 3);
        if (writtel.device()) {
            writtel << dirfromOS;
        }
        out << "D:" << dir.absolutePath() << "\n";
        out.flush();

        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = appendDirname(info.absoluteFilePath());
            } else if (info.isSymLink()) {
                OsFile symslinkOS;
                symslinkOS.set_File(info.symLinkTarget(), 2);
                symslinkOS.set_NameSimslink(info.fileName());
                if (writtel.device()) {
                    writtel << symslinkOS;
                }

            } else if (info.isFile()) {
                result = appendFilename(info, info.absolutePath());
            }
        }
    }

    return result;
}

static void usagethisapp(const char *name, int size) {
    printf("Usage: %s (Options)  \n", name);
    printf(" {%d} Options:\n", size);
    printf("\t--read or -r /foldernameA /foldernameB ... : Read folder name to make an imq image.\n");
    printf("\t--outfile or -o outputfile dir /foldernameA /foldernameB ... : Read folder name to make an outputfile.imq image.\n");
    printf("\t--net or -n remoteurl: Download a image file or whatever file.\n");
    printf("\t--decompress or -d file.imq: open imagefile imq from cd down.. recursive.\n");
    printf("\t--version or -V: show the version of conversion used\n");
    printf("\t--help or -h: display this text.\n");
}

int main(int argc, char *argv[]) {
    QTextStream out(stdout);
    QString str("*");
    out << "FreeSpace home:" << SystemSecure::freespaceonHome() << "\n";
    out.flush();
    
    int i;
    if (argc <= 1) {
        usagethisapp(argv[0], argc);
        return (1);
    }
    QDateTime timer0(QDateTime::currentDateTime());
    const QString name = timer0.toString("dd.MM.yyyy-HH:mm");
    int modus = 0;
    QCoreApplication a(argc, argv);
    qRegisterMetaType<OsFile>();
    QString nameout;
    QStringList dirlistener = a.arguments();


    


    //// dirlistener.takeFirst(); // app name not take 
    for (i = 1; i < argc; i++) {
        if ((!strcmp(argv[i], "-V")) ||
                (!strcmp(argv[i], "--version")) ||
                (!strcmp(argv[i], "-v"))) {
            out << str.fill('*', pointo) << "\n";
            out << __APPNAME__ << ":" << __DOCVERSION__ << "\n";
            out << str.fill('*', pointo) << "\n";
            out.flush();
            return (1);
        } else if ((!strcmp(argv[i], "--help")) || !strcmp(argv[i], "-h")) {
            usagethisapp(argv[0], argc);
            return (1);
        } else if ((!strcmp(argv[i], "--net")) || !strcmp(argv[i], "-n")) {
            modus = 3;
        } else if ((!strcmp(argv[i], "--read")) || !strcmp(argv[i], "-r")) {
            modus = 1;
        } else if ((!strcmp(argv[i], "--decompress")) || !strcmp(argv[i], "-d")) {
            modus = 2;
        } else if ((!strcmp(argv[i], "--outfile")) || !strcmp(argv[i], "-o")) {
            modus = 1;
            int namer = i + 1;
            if (argc >= namer) {
                nameout = dirlistener.at(namer);
            }
        }

    }

    if (modus == 0) {
        return (1);
    }

    Compressor *czip = new Compressor(modus);
    if (!nameout.isEmpty()) {
        czip->SetName(nameout);
    }


    QTimer::singleShot(0, czip, SLOT(execute()));
    return a.exec();
}


#include "main.moc"
