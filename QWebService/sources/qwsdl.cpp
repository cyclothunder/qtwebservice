/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the QWebService library.
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.txt included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#include "../headers/qwsdl_p.h"

/*!
    \class QWsdl
    \brief Class for interaction with local and remote WSDL files.
           Currently read-only.

    Reads web service data (message names, parameters, return values,
    web service name etc.) from a WSDL file. The file can be located on a local
    filesystem, or on a remote one (specified by URL). QWsdl automatically detects
    the nature of this localisation, and downloads the file, if it is not local.

    To get started, you have to:
    \list
        \o construct QWsdl object
        \o specify WSDL file or URL (can be done in constructor,
           or using setWsdl(), resetWsdl(),
        \o check for errors using isErrorState(),
        \o web methods are ready to read using methods(). Other info
           available from numerous getter methods (webServiceName(),
           targetNamespace() etc.)
    \endlist

    Example snippet for that may be:
    \code
        QWsdl wsdl("../../../examples/wsdl/band_ws.asmx", this);
        if (!wsdl.isErrorState()) {
            QMap<QString, QWebMethod *> *methods = wsdl.methods();
            QStringList list = wsdl.methodNames();
        }
    \endcode
  */

/*!
    \property QWsdl::webServiceName
    \brief Holds web service name

    This property's default is empty string.
*/
/*!
    \property QWsdl::host
    \brief Holds web service's host Url as QString

    This property's default is empty string.
*/
/*!
    \property QWsdl::hostUrl
    \brief Holds web service's host Url as QUrl

    This property's default is empty QUrl.
*/
/*!
    \property QWsdl::targetNamespace
    \brief Holds web service's targetNamespace

    This property's default is empty string.
*/
/*!
    \property QWsdl::wsdlFile
    \brief Holds path to web service file

    This property's default is empty string.
*/

/*!
    Simple constructor, requires \a parent only, but needs other information
    to be specified later in order to run.
    You need to run setWsdlFile(), which automatically parses the file.

    Initialises the whole object with default values.

    \sa setWsdlFile(), resetWsdl()
  */
QWsdl::QWsdl(QObject *parent) :
    QObject(parent), d_ptr(new QWsdlPrivate)
{
    Q_D(QWsdl);
    d->init();
}

/*!
    Constructs the object using optional \a parent. Uses the file path or URL
    specified in \a wsdlFile to parse the WSDL file.
  */
QWsdl::QWsdl(const QString &wsdlFile, QObject *parent) :
    QObject(parent), d_ptr(new QWsdlPrivate)
{
    Q_D(QWsdl);
    d->m_wsdlFilePath = wsdlFile;
    d->init();
    parse();
    emit wsdlFileChanged();
}

/*!
  \internal

  COnstructor needed in private header implementation.
  */
QWsdl::QWsdl(QWsdlPrivate &dd, QObject *parent) :
    QObject(parent), d_ptr(&dd)
{
    Q_D(QWsdl);
    d->init();
}

/*!
    Destructor - cleans some internal variables (private ones).
  */
QWsdl::~QWsdl()
{
    Q_D(QWsdl);
    delete d->methodsMap;
    delete d->workMethodList;
    delete d->workMethodParameters;
}

/*!
    \fn QWsdl::errorEncountered(const QString &errMessage)

    Singal emitted when QWsdl encounters an error.
    Carries \a errMessage for convenience.
  */

/*!
    \fn QWsdl::wsdlFileChanged()

    Singal emitted when wsdl file changes.
  */

/*!
    Returns path to WSDL file.
  */
QString QWsdl::wsdlFile() const
{
    Q_D(const QWsdl);
    return d->m_wsdlFilePath;
}

/*!
    Wrapper for resetWsdl(). Used to set the WSDL file or URL using \a wsdlFile.
    Compulsory after simple constructor, but not needed if you have already
    specified the file in the constructor or resetWsdl().

    \sa resetWsdl()
  */
void QWsdl::setWsdlFile(const QString &wsdlFile) // == resetWsdl()
{
    resetWsdl(wsdlFile);
}

/*!
    Can be used to set or reset a WSDL file (or URL), using \a newWsdl.
    Cleans and reinitialises the object, parses the file.

    \sa setWsdlFile()
  */
void QWsdl::resetWsdl(const QString &newWsdl)
{
    Q_D(QWsdl);
    d->m_wsdlFilePath = newWsdl;

    d->methodsMap->clear();
    d->workMethodList->clear();
    d->workMethodParameters->clear();
    d->replyReceived = false;
    d->errorState = false;
    d->errorMessage = QString();
    d->m_webServiceName = QString();
    d->m_hostUrl.setUrl(QString());
    d->m_targetNamespace = QString();
    d->xmlReader.clear();

    parse();
    emit wsdlFileChanged();
}

/*!
    Returns a QMap<QString, QWebServiceMethod *> pointer.
    Keys are method names (just as in getMethodNames()), and values are
    QWebServiceMethods themselves (which means they can be used
    not only to get information, but also to send messages, set them up etc.).

    \sa methodNames()
  */
QMap<QString, QWebMethod *> *QWsdl::methods()
{
    Q_D(QWsdl);
    return d->methodsMap;
}

/*!
    Returns a QStringList of names of web service's methods.

    \sa methods()
  */
QStringList QWsdl::methodNames() const
{
    Q_D(const QWsdl);
    QList<QString> tempMethods = d->methodsMap->keys();
    QStringList result;
    result.append(tempMethods);

    return result;
}

/*!
    Returns QString with the name of the web service specified in WSDL.
  */
QString QWsdl::webServiceName() const
{
    Q_D(const QWsdl);
    return d->m_webServiceName;
}

/*!
    Returns web service's URL. If there is no valid URL in WSDL file,
    path to this file is returned.

    \sa hostUrl()
  */
QString QWsdl::host() const
{
    Q_D(const QWsdl);
    if (!d->m_hostUrl.isEmpty())
        return d->m_hostUrl.toString();
    else
        return d->m_wsdlFilePath;
}

/*!
    Returns web service's URL. If there is no valid URL in WSDL file,
    path to this file is returned.

    \sa host()
  */
QUrl QWsdl::hostUrl() const
{
    Q_D(const QWsdl);
    if (!d->m_hostUrl.isEmpty())
        return d->m_hostUrl;
    else
        return QUrl(d->m_wsdlFilePath);
}

/*!
    Returns target namespace specified in WSDL.
  */
QString QWsdl::targetNamespace() const
{
    Q_D(const QWsdl);
    return d->m_targetNamespace;
}

/*!
    Returns QString with error message in case an error occured.
    Otherwise, returns empty string.

    \sa isErrorState()
  */
QString QWsdl::errorInfo() const
{
    Q_D(const QWsdl);
    return d->errorMessage;
}

/*!
    Returns true if there was an error, false otherwise.
    Details about an error can be read with getErrorInfo().

    \sa errorInfo()
  */
bool QWsdl::isErrorState() const
{
    Q_D(const QWsdl);
    return d->errorState;
}

/*!
    Asynchronous public return slot. Reads WSDL reply (\a rply)
    from server (used in case URL was specified in wsdl file path).
  */
void QWsdl::fileReplyFinished(QNetworkReply *rply)
{
    Q_D(QWsdl);
    QString replyString = d->convertReplyToUtf(QLatin1String(rply->readAll()));
    QFile file(QLatin1String("tempWsdl.asmx~"));
    d->m_wsdlFilePath = QLatin1String("tempWsdl.asmx~");

    if (file.exists())
        file.remove();

    if (!file.open(QFile::WriteOnly)) {
        d->enterErrorState(
                    QString(QLatin1String("Error: cannot write WSDL file from "
                                          "remote location. Reason: ")
                            + file.errorString()));
        return;
    } else {
        file.write(replyString.toUtf8());
    }

    file.close();
    d->replyReceived = true;
    rply->deleteLater();
//    emit replyReady(reply);
}

/*!
    \internal

    Initialises the object.
  */
void QWsdlPrivate::init()
{
    replyReceived = false;
    errorState = false;

    workMethodList = new QStringList();
    workMethodParameters = new QMap<int, QMap<QString, QVariant> >();
    methodsMap = new QMap<QString, QWebMethod *>();
}

/*!
    \internal

    Enters into error state with message \a errMessage.
  */
bool QWsdlPrivate::enterErrorState(const QString &errMessage)
{
    Q_Q(QWsdl);
    errorState = true;
    errorMessage += QString(errMessage + QLatin1String(" "));
//    qDebug() << errMessage;
    emit q->errorEncountered(errMessage);
    return false;
}

/*!
    Central method of this class. Parses the WSDL file, creates all
    QWebServiceMethods, reads all necessary data,
    like web service's name etc.
  */
bool QWsdl::parse()
{
    Q_D(QWsdl);
    /*
      Algorithm extracts method names from "types" tags,
      which is most probably wrong, as it should be
      cross-checked with other tags ("message", etc.)
    */
    if (d->errorState) {
        d->enterErrorState(QLatin1String("WSDL reader is in error state "
                                            "and cannot parse the file."));
        return false;
    }

    prepareFile();

    QFile file(d->m_wsdlFilePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        d->enterErrorState(QString(QLatin1String("Error: cannot read WSDL file: ")
                                + d->m_wsdlFilePath
                                + QLatin1String(". Reason: ")
                                + file.errorString()));
        return false;
    }

    d->xmlReader.setDevice(&file);
    d->xmlReader.readNext();

    while (!d->xmlReader.atEnd()) {
        if (d->xmlReader.isStartElement()) {
            QString tempN = d->xmlReader.name().toString();

            if (tempN == QLatin1String("definitions")) {
                d->m_targetNamespace = d->xmlReader.attributes().value(
                            QLatin1String("targetNamespace")).toString();
                d->readDefinitions();
            } else {
                d->enterErrorState(QLatin1String("Error: file does not have "
                                                    "WSDL definitions inside!"));
                return false;
            }
        } else {
            d->xmlReader.readNext();
        }
    }

    d->prepareMethods();

    if (!d->errorState)
        return true;
    else
        return false;
}

/*!
    \internal

    If the host path is not a local file, but URL, QWsdl will download
    it into a temporary file, then read, and delete at exit.
  */
void QWsdl::prepareFile()
{
    Q_D(QWsdl);
    QUrl filePath;
    filePath.setUrl(d->m_wsdlFilePath);

    if (!QFile::exists(d->m_wsdlFilePath) && filePath.isValid()) {
        d->m_hostUrl = filePath;
        QNetworkAccessManager *manager = new QNetworkAccessManager();
        QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(fileReplyFinished(QNetworkReply*)));
        manager->get(QNetworkRequest(filePath));

        forever {
            if (d->replyReceived == true) {
                return;
            } else {
                QCoreApplication::instance()->processEvents();
            }
        }

        delete manager;
    }
}

/*!
    \internal
  */
void QWsdlPrivate::readDefinitions()
{
    //EXPERIMENTAL
    // QMap used to supress multiple "tag not supported yet" messages.
    // May actually evolve into more universal tag recognition and handling structure.
    QMap<QString, bool> tagUsed;
    tagUsed.insert(QLatin1String("types"), false);
    tagUsed.insert(QLatin1String("message"), false);
    tagUsed.insert(QLatin1String("portType"), false);
    tagUsed.insert(QLatin1String("binding"), false);
    tagUsed.insert(QLatin1String("service"), false);
    tagUsed.insert(QLatin1String("documentation"), false);
    //END of EXPERIMENTAL

    xmlReader.readNext();
    QString tempName = xmlReader.name().toString();

    while(!xmlReader.atEnd()) {
        tempName = xmlReader.name().toString();

        if (xmlReader.isEndElement()
                && (tempName == QLatin1String("definitions"))) {
            xmlReader.readNext();
            break;
        } else if (xmlReader.isEndElement()) {
            xmlReader.readNext();
            continue;
        }

        if (tempName == QLatin1String("types")) {
            readTypes();
//            tagUsed.insert("types", true);
            xmlReader.readNext();
        } else if (tempName == QLatin1String("message")) {
            if (!tagUsed.value(QLatin1String("message")))
                readMessages();
            tagUsed.insert(QLatin1String("message"), true);
            xmlReader.readNext();
        } else if (tempName == QLatin1String("portType")) {
            if (!tagUsed.value(QLatin1String("portType")))
                readPorts();
            tagUsed.insert(QLatin1String("portType"), true);
            xmlReader.readNext();
        } else if (tempName == QLatin1String("binding")) {
            if (!tagUsed.value(QLatin1String("binding")))
                readBindings();
            tagUsed.insert(QLatin1String("binding"), true);
            xmlReader.readNext();
        } else if (tempName == QLatin1String("service")) {
//            if (!tagUsed.value("service"))
                readService();
            tagUsed.insert(QLatin1String("service"), true);
            xmlReader.readNext();
        } else if (tempName == QLatin1String("documentation")) {
            if (!tagUsed.value(QLatin1String("documentation")))
                readDocumentation();
            tagUsed.insert(QLatin1String("documentation"), true);
            xmlReader.readNext();
        } else {
            xmlReader.readNext();
        }
    }
}

/*!
    \internal
  */
void QWsdlPrivate::readTypes()
{
    xmlReader.readNext();
    xmlReader.readNext();
    QString tempName = xmlReader.name().toString();

    if (xmlReader.name().toString() == QLatin1String("schema")) {
        xmlReader.readNext();
    } else {
        enterErrorState(QLatin1String("Error: file does not have WSDL "
                                            "schema tag inside!"));
        return;
    }

    QString lastName;
    tempName = QLatin1String("");
    while(!xmlReader.atEnd()) {
        if (tempName != QLatin1String(""))
            lastName = tempName;
        tempName = xmlReader.name().toString();

        if (tempName == QLatin1String("element")
                && (xmlReader.attributes().count() == 1)) {
            QString elementName = xmlReader.attributes().value(
                        QLatin1String("name")).toString();
            workMethodList->append(elementName);
            readTypeSchemaElement();
        } else if (xmlReader.isEndElement()
                 && (tempName == QLatin1String("schema"))) {
            xmlReader.readNext();
            break;
        } else {
            xmlReader.readNext();
        }
    }
}

/*!
    \internal
  */
void QWsdlPrivate::readTypeSchemaElement()
{
    xmlReader.readNext();
    QMap<QString, QVariant> params;

    bool firstElem = true;
    QString tempName; //xmlReader.name().toString();
    QString lastName;
    while(!xmlReader.atEnd()) {
        if (tempName != QLatin1String(""))
            lastName = tempName;
        tempName = xmlReader.name().toString();

        if (xmlReader.isEndElement()
                && (xmlReader.name() == QLatin1String("element"))
                && ((lastName == QLatin1String("complexType"))
                    || (lastName == QLatin1String("sequence"))
//                    || (lastName == QLatin1String("part"))
                    )
                && (firstElem == false)) {
            int currentMethodIndex = workMethodList->length() - 1;
            workMethodParameters->insert(currentMethodIndex, params);
            xmlReader.readNext();
            break;
        }

        if ((tempName == QLatin1String("complexType"))
                || (tempName == QLatin1String("sequence"))) {
            firstElem = false;
            xmlReader.readNext();
            continue;
        }

        if (tempName == QLatin1String("element")) {
            firstElem = false;
            // Min and max occurences are not taken into account!
            QString elementName = xmlReader.attributes().value(
                        QLatin1String("name")).toString();
            // Ommits namespace ("s:int" => "int")
            QString elementType = xmlReader.attributes().value(
                        QLatin1String("type")).toString();

            if ((elementName == QLatin1String(""))
                    || (elementType == QLatin1String(""))) {
                xmlReader.readNext();
                continue;
            }

            QVariant element;
            // The int(1) looks very bad.
            elementType = elementType.split(QChar(':')).at(1);
            // NEEDS MANY MORE VALUE TYPES! VERY SHAKY IMPLEMENTATION!
            // Prob'ly better to use schemas.
            if (elementType == QLatin1String("int")) {
                element.setValue(int());
            } else if (elementType == QLatin1String("float")) {
                element.setValue(float());
            } else if (elementType == QLatin1String("double")) {
                element.setValue(double());
            } else if (elementType == QLatin1String("boolean")) {
                element.setValue(true);
            } else if (elementType == QLatin1String("dateTime")) {
                element.setValue(QDateTime());
            } else if (elementType == QLatin1String("string")) {
                element.setValue(QString());
            } else if (elementType == QLatin1String("char")) {
                element.setValue(QChar());
            } else if (elementType.startsWith(QLatin1String("ArrayOf"))) {
                elementType = elementType.mid(7);

                if (elementType == QLatin1String("String")) {
                    element.setValue(QStringList());
                } else {//if (elementType == "DateTime") {
                    element.setValue(QList<QVariant>());
                }
            } else {
                element.setValue(QString());
            }
            // VERY SHAKY IMPLEMENTATION!

            params.insert(elementName, element);
            xmlReader.readNext();
//        } else if (tempName == QLatin1String("part")) {
//            firstElem = false;
//            QString elementName = xmlReader.attributes().value(
//                        QLatin1String("name")).toString();
//            QVariant element = QString("XSD not supported yet");
//            params.insert(elementName, element);
//            xmlReader.readNext();
        } else {
            xmlReader.readNext();
        }
    }
}

/*!
    \internal

    Analyses both "working" QList and QMap, and extracts methods data,
    which is then put into 'methods' QMap.
 */
void QWsdlPrivate::prepareMethods()
{
    if (errorState)
        return;

    QList<bool> methodsDone;//[workMethodList->length()];

    for (int x = 0; x < workMethodList->length(); x++)
        methodsDone.append(false);


    for (int i = 0; (i < workMethodList->length()); i++) {
        if (methodsDone.at(i) == false) {
            bool isMethodAndResponsePresent = false;
            int methodMain = 0;
            int methodReturn = 0;
            QString methodName = workMethodList->at(i);

            if (methodName.contains(QLatin1String("Response"))) {
                methodReturn = i;
                methodsDone.insert(i, true);
                QString tempMethodName = methodName;
                tempMethodName.chop(8);

                for (int j = 0; j < workMethodList->length(); j++) {
                    if ((workMethodList->at(j) == tempMethodName)
                            || (workMethodList->at(j) == (tempMethodName + "Request"))) {
                        methodMain = j;
                        methodsDone.insert(j, true);
                        isMethodAndResponsePresent = true;
                        methodName = tempMethodName;
                        break;
                    }
                }
            } else {
                methodMain = i;

                for (int j = 0; j < workMethodList->length(); j++) {
                    QString cleanMethodName = QString(methodName + QLatin1String("Response"));
                    QString requestMethodName = methodName;
                    requestMethodName.chop(7);
                    requestMethodName += QLatin1String("Response");

                    if ((workMethodList->at(j) == cleanMethodName)
                        || workMethodList->at(j) == requestMethodName) {
                        methodReturn = j;
                        methodsDone.insert(j, true);
                        isMethodAndResponsePresent = true;
                        break;
                    }
                }
            }

            if (isMethodAndResponsePresent == true) {
                QString methodPath;
                if (m_hostUrl.isEmpty())
                    methodPath = m_wsdlFilePath;
                else
                    methodPath = m_hostUrl.path();

                QWebMethod *m = new QWebMethod(methodPath);
                m->setMethodName(methodName);
                m->setTargetNamespace(m_targetNamespace);
                m->setParameters(workMethodParameters->value(methodMain));
                m->setReturnValue(workMethodParameters->value(methodReturn));
                methodsMap->insert(methodName, m);
            }
        }
    }
}

/*!
    \internal
  */
void QWsdlPrivate::readMessages()
{
//    qDebug() << "WSDL :messages tag not supported yet.";
    xmlReader.readNext();
}

/*!
    \internal
  */
void QWsdlPrivate::readPorts()
{
//    qDebug() << "WSDL :portType tag not supported yet.";
    xmlReader.readNext();
}

/*!
    \internal
  */
void QWsdlPrivate::readBindings()
{
//    qDebug() << "WSDL :binding tag not supported yet.";
    xmlReader.readNext();
}

/*!
    \internal
  */
void QWsdlPrivate::readService()
{
    // TODO: add different addresses for different message types.
//    qDebug() << "WSDL :service tag not supported yet.";
    QString tempName;

    while (!xmlReader.atEnd()) {
        tempName = xmlReader.name().toString();

        if ((xmlReader.isEndElement())
                && (tempName == QLatin1String("service"))) {
            xmlReader.readNext();
            return;
        }

        if ((tempName == QLatin1String("service"))
                && xmlReader.attributes().hasAttribute(
                    QLatin1String("name"))
                && (m_webServiceName == QLatin1String(""))) {
            m_webServiceName = xmlReader.attributes().value(
                        QLatin1String("name")).toString();
        }

        if ((tempName == QLatin1String("address"))
                && xmlReader.attributes().hasAttribute(
                    QLatin1String("location"))) {
            m_hostUrl.setUrl(xmlReader.attributes().value(
                                 QLatin1String("location")).toString());
        }

        xmlReader.readNext();
    }
}

/*!
    \internal
  */
void QWsdlPrivate::readDocumentation()
{
//    qDebug() << "WSDL :documentation tag not supported yet.";
    xmlReader.readNext();
}

/*!
    \internal
  */
QString QWsdlPrivate::convertReplyToUtf(const QString &textToConvert)
{
    QString result = textToConvert;

    result.replace(QLatin1String("&lt;"), QLatin1String("<"));
    result.replace(QLatin1String("&gt;"), QLatin1String(">"));

    return result;
}
