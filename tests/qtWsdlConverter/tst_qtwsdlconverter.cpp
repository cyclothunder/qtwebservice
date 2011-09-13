/**
  This test checks converter in operation (requires Internet connection or a working local web service)
  */

#include <QtTest/QtTest>
#include <qwsdl.h>
#include "../../qtWsdlConverter/headers/wsdlconverter.h"

class tst_qtwsdlconverter : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void initialTest();
    void noForceTest();
    void forceTest();
    void buildSystemTest();
    void noMessagesTest();
    void noMessagesAsynchronousTest();
    void allInOneDirTest();
    void errorsTest();
    void noBuildSystemTest();

    void cleanupTestCase();

private:
    bool removeDir(QString path);
};

void tst_qtwsdlconverter::init()
{
    // Remove files created during conversion:
    removeDir(QString("band_ws"));
}

void tst_qtwsdlconverter::initialTest()
{
    QStringList arguments;
    arguments.append(QString("../../examples/wsdl/band_ws.asmx"));

    WsdlConverter *converter = new WsdlConverter(arguments, this);
    QCOMPARE(converter->isErrorState(), bool(false));
    QCOMPARE(converter->webServiceName(), QString("band_ws"));

    converter->convert();
    QCOMPARE(QFile::exists(QString("band_ws/band_ws.pro")), bool(true));
    QCOMPARE(QFile::exists(QString("band_ws/headers/band_ws.h")), bool(true));
    QCOMPARE(QFile::exists(QString("band_ws/sources/band_ws.cpp")), bool(true));

    QWsdl *wsdl = new QWsdl(QString("../../examples/wsdl/band_ws.asmx"), this);
    QStringList methods = wsdl->methodNames();

    foreach (QString s, methods) {
        QCOMPARE(QFile::exists(QString("band_ws/headers/" + s + ".h")), bool(true));
        QCOMPARE(QFile::exists(QString("band_ws/sources/" + s + ".cpp")), bool(true));
    }

    delete wsdl;
    delete converter;
}

void tst_qtwsdlconverter::noForceTest()
{
    QStringList arguments;
    arguments.append(QString("../../examples/wsdl/band_ws.asmx"));

    WsdlConverter *converter1 = new WsdlConverter(arguments, this);
    QCOMPARE(converter1->isErrorState(), bool(false));
    QCOMPARE(converter1->webServiceName(), QString("band_ws"));
    converter1->convert();

    WsdlConverter *converter2 = new WsdlConverter(arguments, this);
    QCOMPARE(converter2->isErrorState(), bool(false));
    QCOMPARE(converter2->webServiceName(), QString("band_ws"));

    // Try converting with existing code and no --forced set:
    converter2->convert();
    QCOMPARE(converter2->isErrorState(), bool(true));
    QCOMPARE(converter1->isErrorState(), bool(false));

    // Remove dir, retry converting using the same object:
    removeDir(QString("band_ws"));
    converter2->convert();
    QCOMPARE(converter2->isErrorState(), bool(true));
    QCOMPARE(converter1->isErrorState(), bool(false));

    // Restet converter and rerun the conversion once again:
    converter2->resetError();
    converter2->convert();
    QCOMPARE(converter2->isErrorState(), bool(false));
    QCOMPARE(converter1->isErrorState(), bool(false));

    delete converter1;
    delete converter2;
}

void tst_qtwsdlconverter::forceTest()
{
    QStringList arguments;
    arguments.append(QString("--force"));
    arguments.append(QString("../../examples/wsdl/band_ws.asmx"));

    WsdlConverter *converter = new WsdlConverter(arguments, this);
    QCOMPARE(converter->isErrorState(), bool(false));
    QCOMPARE(converter->webServiceName(), QString("band_ws"));
    converter->convert();

    WsdlConverter *converter2 = new WsdlConverter(arguments, this);
    QCOMPARE(converter2->isErrorState(), bool(false));
    QCOMPARE(converter2->webServiceName(), QString("band_ws"));
    converter2->convert();

    QCOMPARE(converter->isErrorState(), bool(false));
    QCOMPARE(converter2->isErrorState(), bool(false));
    QCOMPARE(QFile::exists(QString("band_ws/band_ws.pro")), bool(true));
    QCOMPARE(QFile::exists(QString("band_ws/headers/band_ws.h")), bool(true));
    QCOMPARE(QFile::exists(QString("band_ws/sources/band_ws.cpp")), bool(true));

    QWsdl *wsdl = new QWsdl(QString("../../examples/wsdl/band_ws.asmx"), this);
    QStringList methods = wsdl->methodNames();

    foreach (QString s, methods) {
        QCOMPARE(QFile::exists(QString("band_ws/headers/" + s + ".h")), bool(true));
        QCOMPARE(QFile::exists(QString("band_ws/sources/" + s + ".cpp")), bool(true));
    }

    delete wsdl;
    delete converter;
}

void tst_qtwsdlconverter::buildSystemTest()
{
    QStringList arguments;
    arguments.append(QString("--scons"));
    arguments.append(QString("--cmake"));
    arguments.append(QString("--json"));
    arguments.append(QString("../../examples/wsdl/band_ws.asmx"));

    WsdlConverter *converter = new WsdlConverter(arguments, this);
    QCOMPARE(converter->isErrorState(), bool(false));
    QCOMPARE(converter->webServiceName(), QString("band_ws"));

    converter->convert();
    QCOMPARE(QFile::exists(QString("band_ws/band_ws.pro")), bool(true));
    QCOMPARE(QFile::exists(QString("band_ws/SConstruct")), bool(true));
    QCOMPARE(QFile::exists(QString("band_ws/CMakeLists.txt")), bool(true));
    QCOMPARE(QFile::exists(QString("band_ws/headers/band_ws.h")), bool(true));
    QCOMPARE(QFile::exists(QString("band_ws/sources/band_ws.cpp")), bool(true));

    QWsdl *wsdl = new QWsdl(QString("../../examples/wsdl/band_ws.asmx"), this);
    QStringList methods = wsdl->methodNames();

    foreach (QString s, methods) {
        QCOMPARE(QFile::exists(QString("band_ws/headers/" + s + ".h")), bool(true));
        QCOMPARE(QFile::exists(QString("band_ws/sources/" + s + ".cpp")), bool(true));
    }

    delete wsdl;
    delete converter;
}

void tst_qtwsdlconverter::noMessagesTest()
{
    QStringList arguments;
    arguments.append(QString("--no-messages-structure"));
    arguments.append(QString("../../examples/wsdl/band_ws.asmx"));

    WsdlConverter *converter = new WsdlConverter(arguments, this);
    QCOMPARE(converter->isErrorState(), bool(false));
    QCOMPARE(converter->webServiceName(), QString("band_ws"));

    converter->convert();
    QCOMPARE(QFile::exists(QString("band_ws/band_ws.pro")), bool(true));
    QCOMPARE(QFile::exists(QString("band_ws/headers/band_ws.h")), bool(true));
    QCOMPARE(QFile::exists(QString("band_ws/sources/band_ws.cpp")), bool(true));

    QWsdl *wsdl = new QWsdl(QString("../../examples/wsdl/band_ws.asmx"), this);
    QStringList methods = wsdl->methodNames();

    foreach (QString s, methods) {
        QCOMPARE(QFile::exists(QString("band_ws/headers/" + s + ".h")), bool(false));
        QCOMPARE(QFile::exists(QString("band_ws/sources/" + s + ".cpp")), bool(false));
    }

    delete wsdl;
    delete converter;
}

void tst_qtwsdlconverter::noMessagesAsynchronousTest()
{
    QStringList arguments;
    arguments.append(QString("--no-messages-structure"));
    arguments.append(QString("--asynchronous"));
    arguments.append(QString("../../examples/wsdl/band_ws.asmx"));

    WsdlConverter *converter = new WsdlConverter(arguments, this);
    QCOMPARE(converter->isErrorState(), bool(false));
    QCOMPARE(converter->webServiceName(), QString("band_ws"));

    converter->convert();
    QCOMPARE(QFile::exists(QString("band_ws/band_ws.pro")), bool(true));
    QCOMPARE(QFile::exists(QString("band_ws/headers/band_ws.h")), bool(true));
    QCOMPARE(QFile::exists(QString("band_ws/sources/band_ws.cpp")), bool(true));

    QWsdl *wsdl = new QWsdl(QString("../../examples/wsdl/band_ws.asmx"), this);
    QStringList methods = wsdl->methodNames();

    foreach (QString s, methods) {
        QCOMPARE(QFile::exists(QString("band_ws/headers/" + s + ".h")), bool(false));
        QCOMPARE(QFile::exists(QString("band_ws/sources/" + s + ".cpp")), bool(false));
    }

    delete wsdl;
    delete converter;
}

void tst_qtwsdlconverter::allInOneDirTest()
{
    QStringList arguments;
    arguments.append(QString("--all-in-one-dir-structure"));
    arguments.append(QString("../../examples/wsdl/band_ws.asmx"));

    WsdlConverter *converter = new WsdlConverter(arguments, this);
    QCOMPARE(converter->isErrorState(), bool(false));
    QCOMPARE(converter->webServiceName(), QString("band_ws"));

    converter->convert();
    QCOMPARE(QFile::exists(QString("band_ws/band_ws.pro")), bool(true));
    QCOMPARE(QFile::exists(QString("band_ws/band_ws.h")), bool(true));
    QCOMPARE(QFile::exists(QString("band_ws/band_ws.cpp")), bool(true));

    QWsdl *wsdl = new QWsdl(QString("../../examples/wsdl/band_ws.asmx"), this);
    QStringList methods = wsdl->methodNames();

    foreach (QString s, methods) {
        QCOMPARE(QFile::exists(QString("band_ws/" + s + ".h")), bool(true));
        QCOMPARE(QFile::exists(QString("band_ws/" + s + ".cpp")), bool(true));
    }

    delete wsdl;
    delete converter;
}

void tst_qtwsdlconverter::errorsTest()
{
    QStringList arguments;
    WsdlConverter *converter = new WsdlConverter(arguments, this);
    QCOMPARE(converter->isErrorState(), bool(true));

    converter->convert();
    QCOMPARE(converter->isErrorState(), bool(true));

    delete converter;

    arguments.append(QString("--json"));
    arguments.append(QString("--xml"));
    arguments.append(QString("../../examples/wsdl/band_ws.asmx"));
    converter = new WsdlConverter(arguments, this);
    QCOMPARE(converter->isErrorState(), bool(false));

    converter->convert();
    QCOMPARE(converter->isErrorState(), bool(false));

    delete converter;
}

void tst_qtwsdlconverter::noBuildSystemTest()
{
    QStringList arguments;
    arguments.append(QString("--no-build-system"));
    arguments.append(QString("../../examples/wsdl/band_ws.asmx"));

    WsdlConverter *converter = new WsdlConverter(arguments, this);
    QCOMPARE(converter->isErrorState(), bool(false));
    QCOMPARE(converter->webServiceName(), QString("band_ws"));

    converter->convert();
    QCOMPARE(QFile::exists(QString("band_ws/band_ws.pro")), bool(false));
    QCOMPARE(QFile::exists(QString("band_ws/CMakeLists.txt")), bool(false));
    QCOMPARE(QFile::exists(QString("band_ws/SConstruct")), bool(false));

    QCOMPARE(QFile::exists(QString("band_ws/headers/band_ws.h")), bool(true));
    QCOMPARE(QFile::exists(QString("band_ws/sources/band_ws.cpp")), bool(true));

    QWsdl *wsdl = new QWsdl(QString("../../examples/wsdl/band_ws.asmx"), this);
    QStringList methods = wsdl->methodNames();

    foreach (QString s, methods) {
        QCOMPARE(QFile::exists(QString("band_ws/headers/" + s + ".h")), bool(true));
        QCOMPARE(QFile::exists(QString("band_ws/sources/" + s + ".cpp")), bool(true));
    }

    delete wsdl;
    delete converter;
}

void tst_qtwsdlconverter::cleanupTestCase()
{
    // Remove files created during conversion:
    removeDir(QString("band_ws"));
}

bool tst_qtwsdlconverter::removeDir(QString path)
{
    QDir dir(path);
    bool err = false;
    if (dir.exists()) {
        QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot |
                                                   QDir::Dirs | QDir::Files);

        int count = entries.size();
        for (int i = 0; ((i < count) && (err == false)); i++) {
            QFileInfo entryInfo = entries[i];
            QString tpath = entryInfo.absoluteFilePath();

            if (entryInfo.isDir()) {
                err = removeDir(tpath);
            }
            else {
                QFile file(tpath);
                if (!file.remove())
                    err = true;
            }
        }

        if (!dir.rmdir(dir.absolutePath()))
            err = true;
    }
    return(err);
}

QTEST_MAIN(tst_qtwsdlconverter)
#include "tst_qtwsdlconverter.moc"

