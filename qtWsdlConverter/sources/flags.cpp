/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the qtWsdlConverter tool.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "../headers/flags.h"

/*!
    \class Flags
    \brief Holds all important flags used by converter.

    All application's switches are unified thus enabling easy OR switching.
    The only exception is the --force switch, which is held as a separate
    bool variable.

    All entries in single sections (with an exception of build system related
    switches) are exclusive - you can use only one at a time (the lates one
    specified is used). For example, if you use --http and --json, only JSON flag
    will be set. Sections are specified in enum documentation (the table below).
  */

/*!
    \enum Flags::Option
    This enum type specifies code creation mode.
    Sections are indicated in brackets:

    \value FullMode
           (mode) All enums and variables are copied, enabling some flexibility
           in use.
    \value DebugMode
           (mode) fullMode + debug messages.
    \value CompactMode
           (mode) Only most needed methods, variables and enums are preserved.
           Code is as small, as possible, at the expense of loosing flexibility.
    \value Subclass
           converter creates messages by subclassing QWebMethod (this option
           requires QWebService library to be present).
    \value Synchronous
           (synchronousness) Web methods will wait for reply and return with data.
    \value Asynchronous
           (synchronousness) Web methods will return instantly. Data availability
           will be announced by emitting a signal.
    \value StandardStructure
           (structure) Headers will be placed in <dir>/headers,
           sources in <dir>/sources, build system and .pro files in <dir>/.
    \value NoMessagesStructure
           (structure) Converter will not create messages as separate classes,
           but use QWebMethod (this option requires QWebService library to be present).
    \value AllInOneDirStructure
           (structure) All generated files will be stored in one folder.
    \value Qmake
           (build system) qmake will be used.
    \value Cmake
           (build system) cmake will be used.
    \value Scons
           (build system) scons will be used.
    \value NoBuildSystem
           (build system) No build system files will be created.
    \value Http
           (protocol) http protocol will be used.
    \value Soap10
           (protocol) SOAP 1.0 protocol will be used.
    \value Soap12
           (protocol) SOAP 1.2 protocol will be used.
    \value Soap
           (protocol) Internall wrapper for all SOAP protocols. When used in app,
           will default to SOAP 1.2
    \value Json
           (protocol) JSON protocol will be used.
    \value Xml
           (protocol) XML protocol (non-SOAP) will be used.
    \value Rest
           (non-exclusive, protocol) Code for communicating with RESTful WSes
           will be generted. This can be combined with any other protocol flag.
 */

/*!
    \enum Flags::HttpMethod

    This enum stores HTTP method chosen by user. Those options cannot be combined!

    \value Post
           HTTP POST will be used.
    \value Get
           HTTP GET will be used.
    \value Put
           HTTP PUT will be used.
    \value Delete
           HTTP DELETE will be used.
  */

/*!
    Constructs the Flags object. All params are optional. If you want to
    set non-default suffixes, use setObjSuffix() and setMsgSuffix().

    Default values:
    \list
    \o \a options_ :
    \list
        \o fullMode
        \o synchronous
        \o standardStructure
        \o soap12
        \o qmake
    \endlist
    \o \a method_ = POST
    \o \a forced = false
    \o \a parent = 0
    \endlist
  */
Flags::Flags(Options options_, HttpMethod method_, bool forced, QObject *parent) :
    QObject(parent)
{
    options = options_;
    method = method_;
    force = forced;

    msgSuffix = "Send";
    objSuffix = "Msg";
}

/*!
    Resets all flags to default values. This does not apply to '--forced'.
  */
void Flags::resetFlags()
{
    options = 0;
    options = FullMode | Synchronous | StandardStructure | Qmake | Soap12;
}

/*!
    Resets (zeroes) flags set to 1 in \a whatToReset.
    This does not apply to '--forced'.
  */
void Flags::resetFlags(Options whatToReset)
{
    this->options &= ~whatToReset;
}

/*!
    Does OR combination of \a options with those already set.
  */
void Flags::setFlags(Options options)
{
    Options changed = (options ^ this->options);

    if (!changed)
        return;

    this->options |= options;
}

/*!
  Sets HTTP method flag to \a newMethod. Any previous setting is discarded.

  */
void Flags::setHttpMethod(HttpMethod newMethod)
{
    method = newMethod;
}

/*!
  Sets HTTP method flag to \a newMethod. Any previous setting is discarded.
  Only valid values are accepted, but checks are not case sensitive.
  Therefore, you can for example pass "delete" or "DELETE".

  Returns true if succesfull.
  */
bool Flags::setHttpMethod(const QString &newMethod)
{
    QString tempMethod = newMethod.toLower();
    if (tempMethod == QString::fromLatin1("post"))
        method = Post;
    else if (tempMethod == QString::fromLatin1("get"))
        method = Get;
    else if (tempMethod == QString::fromLatin1("put"))
        method = Put;
    else if (tempMethod == QString::fromLatin1("delete"))
        method = Delete;
    else
        return false;

    return true;
}

/*!
    Sets the \a forced value to given one.

    When 'true', converter will delete old sources, and create
    a fresh copy every time it is run.
  */
void Flags::setForced(bool forced)
{
    this->force = forced;
}

/*!
    Sets the message suffix using \a newMsgSuffix.
    */
void Flags::setMsgSuffix(const QString &newMsgSuffix)
{
    msgSuffix = newMsgSuffix;
}

/*!
    Sets the object suffix using \a newObjSuffix.
    */
void Flags::setObjSuffix(const QString &newObjSuffix)
{
    objSuffix = newObjSuffix;
}

/*!
    Returns currently set options enum.
  */
Flags::Options Flags::flags() const
{
    return options;
}

/*!
  Returns protocol used in form of a QString. If \a includeRest is
  true, and --rest flag was specified, it appends ",rest" to the result.
  */
QString Flags::protocolString(bool includeRest) const
{
    QString result = "";

    if (options & Http)
        result = "http";
    else if (options & Soap10)
        result = "soap10";
    else if (options & Soap12)
        result = "soap12";
    else if (options & Json)
        result = "json";
    else if (options & Xml)
        result = "xml";

    if (includeRest && (options & Rest))
        result += ",rest";

    return result;
}

/*!
  Returns the HTTP method used.

  \sa httpMethodString()
  */
Flags::HttpMethod Flags::httpMethod() const
{
    return method;
}

/*!
  Returns the HTTP method used, in form of a QString.

  \sa httpMethod()
  */
QString Flags::httpMethodString() const
{
    QString result = "";

    if (method == Post)
        result = "POST";
    else if (method == Get)
        result = "GET";
    else if (method == Put)
        result = "PUT";
    else if (method == Delete)
        result = "DELETE";

    return result;
}

/*!
    Same as isForced().

    Returns force state.

    \sa isForced()
  */
bool Flags::forced() const
{
    return isForced();
}

/*!
    Same as forced(). Returns force state.

    \sa forced()
  */
bool Flags::isForced() const
{
    return this->force;
}

/*!
    Returns message suffix, which is appended to methods (ones that send
    the message) in generated code.
  */
QString Flags::messageSuffix() const
{
    return msgSuffix;
}

/*!
    Returns the suffix, which is appended to object names (used in service
    header and source when in asynchronous).
  */
QString Flags::objectSuffix() const
{
    return objSuffix;
}

/*!
    Assign operator for Flags object. Copies r-value \a flag into
    r-value object, and returns the object.
  */
Flags &Flags::operator =(const Flags &flag)
{
    if (this == &flag)
        return *this;

    this->options = flag.options;
    this->method = flag.method;
    this->force = flag.force;
    this->msgSuffix = flag.msgSuffix;
    this->objSuffix = flag.objSuffix;

    return *this;
}
