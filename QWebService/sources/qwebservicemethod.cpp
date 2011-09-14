#include "../headers/qwebservicemethod.h"

/*!
    \class QWebServiceMethod
    \brief Extends QWebMethod with some generic constructors and synchronous message sending.


    Subclass of QWebMethod, contains many generic methods for sending messages.
    Can be used both synchronously (through static sendMessage() method),
    or asynchronously (indicates, when reply is ready by emitting
    a replyReady() signal).

    More description will be added in the future.
  */

/*!
  Constructs web method object with \a parent.
  Requires specifying other parameters later.
  If you use that constructor, you can probably go on
  and use the base QWebMethod class.
  */
QWebServiceMethod::QWebServiceMethod(QObject *parent) :
    QWebMethod(parent)
{
}

/*!
    A constructor that takes in \a protocol information,
    \a httpMethod to use, and \a parent
    to satisfy QObject requirements. All other data has to
    be set using setter methods.
  */
QWebServiceMethod::QWebServiceMethod(Protocol protocol, HttpMethod httpMethod,
                                     QObject *parent) :
    QWebMethod(protocol, httpMethod, parent)
{
}

/*!
    Constructs the message using \a url, \a messageName, \a parent,
    \a protocol (which defaults to soap12),
    and \a method (which defaults to POST).
    Requires params to be specified later.

    \sa init(), setParameters(), setProtocol(), sendMessage()
  */
QWebServiceMethod::QWebServiceMethod(QUrl url, QString messageName,
                                     Protocol protocol, HttpMethod method,
                                     QObject *parent) :
    QWebMethod(protocol, method, parent)
{
    m_hostUrl = url;
    m_methodName = messageName;
    setProtocol(protocol);
    setHttpMethod(method);
}

/*!
    Constructs the message using \a url, \a messageName, \a parent,
    \a protocol (which defaults to soap12),
    and \a method (which defaults to POST).
    Requires params to be specified later.

    \sa init(), setParameters(), setProtocol(), sendMessage()
  */
QWebServiceMethod::QWebServiceMethod(QString url, QString messageName,
                                     Protocol protocol, HttpMethod method,
                                     QObject *parent) :
    QWebMethod(protocol, method, parent)
{
    m_methodName = messageName;
    setProtocol(protocol);
    setHttpMethod(method);
    m_hostUrl.setUrl(url);
}

/*!
    Constructs the message using \a url, \a messageName, \a tNamespace,
    \a parent, \a protocol (which defaults to soap12),
    and \a method (which defaults to POST).
    This constructor also takes message parameters (\a params).
    Does not require specifying any more information, but you still need to
    manually send the message using sendMessage() (without any arguments,
    or else - if you want to change ones specified here).

    \sa init(), sendMessage(), setProtocol()
  */
QWebServiceMethod::QWebServiceMethod(QString url, QString messageName, QString tNamespace,
                                     QMap<QString, QVariant> params,
                                     Protocol protocol, HttpMethod method,
                                     QObject *parent) :
    QWebMethod(protocol, method, parent)
{
    m_methodName = messageName;
    m_targetNamespace = tNamespace;
    parameters = params;
    setProtocol(protocol);
    setHttpMethod(method);
    m_hostUrl.setUrl(url);
}


/*!
    \overload sendMessage()

    Sends the message asynchronously using parameters specified in \a params.

    Returns true on success.
  */
bool QWebServiceMethod::sendMessage(QMap<QString, QVariant> params)
{
    parameters = params;
    sendMessage();
    return true;
}

/*!
    \overload sendMessage()

    STATIC method. Sends the message synchronously, using \a url, \a msgName,
    \a tNamespace, \a params and \a parent.
    Protocol can optionally be specified by \a protocol (default is SOAP 1.2),
    as well as HTTP \a method (default is POST).
    Returns with web service reply.
  */
QByteArray QWebServiceMethod::sendMessage(QUrl url,
                                          QString msgName, QString tNamespace,
                                          QMap<QString, QVariant> params,
                                          Protocol protocol, HttpMethod method,
                                          QObject *parent)
{
    QWebServiceMethod qsm(url.toString(), msgName, tNamespace, params, protocol,
                          method, parent);

    qsm.sendMessage();
    // TODO: ADD ERROR HANDLING!
    forever {
        if (qsm.replyReceived) {
            return qsm.reply;
        } else {
            qApp->processEvents();
        }
    }
}
