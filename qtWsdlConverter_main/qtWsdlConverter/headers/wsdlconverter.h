#ifndef WSDLCONVERTER_H
#define WSDLCONVERTER_H

#include <QObject>
#include "../../../QWebService_main/QWebService/headers/QWebService_global.h"
#include "flags.h"
#include "standardpath.h"

class WsdlConverter : public QObject
{
    Q_OBJECT

public:
    explicit WsdlConverter(QString wsdlFileOrUrl,
                           QObject *parent = 0,
                           QString outputDirectory = "",
                           QString baseOutputClassName = "");
    ~WsdlConverter();

    void setFlags(Flags flgs);
    bool isErrorState();
    void convert();
    QString getWebServiceName();

signals:
    void errorEncountered(QString errMessage);

public slots:

private:
    bool enterErrorState(QString errMessage = "");

    bool errorState;
    QString errorMessage;
    QString baseClassName;
    QString outputDir;
    QWsdl *wsdl;
    Flags flags;
};

#endif // WSDLCONVERTER_H
