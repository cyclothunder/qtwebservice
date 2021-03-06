/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the QWebMethod test suite.
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.txt included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#include <QtTest/QtTest>
#include <qwebmethod.h>

/**
  This test checks QWebMethod in operation (requires Internet connection or a working local web service)
  */
class TestQWebMethod : public QObject
{
    Q_OBJECT

private slots:
    void initialTest();
    void gettersTest();
    void settersTest();
    void qpropertyTest();
    void asynchronousSendingTest();

private:
    void defaultGettersTest(QWebMethod *msg);
};

/*
  Performs basic checks of constructor and basic methods.
  */
void TestQWebMethod::initialTest()
{
    QWebMethod *method = new QWebMethod(0, QWebMethod::Soap12, QWebMethod::Post);
    QCOMPARE(method->isErrorState(), bool(false));
    delete method;
}

/*
  Performs basic checks of getters.
  */
void TestQWebMethod::gettersTest()
{
    QWebMethod *method = new QWebMethod(0, QWebMethod::Soap12, QWebMethod::Post);
    defaultGettersTest(method);
    delete method;
}

/*
  Performs basic checks of setters.
  */
void TestQWebMethod::settersTest()
{
    QWebMethod *method = new QWebMethod(0, QWebMethod::Soap12, QWebMethod::Post);
    defaultGettersTest(method);

    QUrl tempUrl(QString("http://www.currencyserver.de/webservice/currencyserverwebservice.asmx"));
    method->setHost(tempUrl);
    QCOMPARE(method->hostUrl(), tempUrl);

    QString tempName("getProviderList");
    method->setMethodName(tempName);
    QCOMPARE(method->methodName(), tempName);

    QString tempTargetNmspc("http://www.daenet.de/webservices/CurrencyServer");
    method->setTargetNamespace(tempTargetNmspc);
    QCOMPARE(method->targetNamespace(), tempTargetNmspc);

    method->setProtocol(QWebMethod::Json);
    QCOMPARE(method->protocol(), QWebMethod::Json);
    QCOMPARE(method->protocolString(), QString("Json"));
    QCOMPARE(method->protocolString(true), QString("Json"));

    method->setHttpMethod(QWebMethod::Delete);
    QCOMPARE(method->httpMethod(), QWebMethod::Delete);
    QCOMPARE(method->httpMethodString(), QString("Delete"));

    method->setHttpMethod("Get");
    QCOMPARE(method->httpMethod(), QWebMethod::Get);
    QCOMPARE(method->httpMethodString(), QString("Get"));

    method->setHttpMethod("pUT");
    QCOMPARE(method->httpMethod(), QWebMethod::Put);
    QCOMPARE(method->httpMethodString(), QString("Put"));

    method->setHttpMethod("POST");
    QCOMPARE(method->httpMethod(), QWebMethod::Post);
    QCOMPARE(method->httpMethodString(), QString("Post"));

    QMap<QString, QVariant> tmpP;
    tmpP.insert("symbol", QVariant("NOK"));
    method->setParameters(tmpP);
    QCOMPARE(method->parameterNames().size(), int(1));
    QCOMPARE(method->parameterNames().first(), QString("symbol"));
    QCOMPARE(method->parameterNamesTypes().value("symbol"), QVariant("NOK"));

    method->setReturnValue(tmpP);
    QCOMPARE(method->returnValueName().size(), int(1));
    QCOMPARE(method->returnValueName().first(), QString("symbol"));
    QCOMPARE(method->returnValueNameType().value("symbol"), QVariant("NOK"));

    delete method;
}

/*
  Checks whether qproperties are working as intended.
  */
void TestQWebMethod::qpropertyTest()
{
    QWebMethod *method = new QWebMethod(0, QWebMethod::Soap12, QWebMethod::Post);

    QCOMPARE(method->isErrorState(), bool(false));

    QCOMPARE(method->property("host").toString(), QString());
    QString temp = "http://www.currencyserver.de/webservice/currencyserverwebservice.asmx";
    method->setProperty("host", QVariant(temp));
    QCOMPARE(method->property("host").toString(), QString(temp));
    QCOMPARE(QUrl(method->property("hostUrl").toString()), QUrl(temp));
    temp += ".changed";
    method->setProperty("host", QVariant(QUrl(temp)));
    QCOMPARE(method->property("host").toString(), QString(temp));
    QCOMPARE(QUrl(method->property("hostUrl").toString()), QUrl(temp));

    method->setProperty("protocol", QVariant("json"));
    QCOMPARE(method->property("protocol").toString(), QString("Json"));

    method->setProperty("httpMethod", QVariant("get"));
    QCOMPARE(method->property("httpMethod").toString(), QString("Get"));

    temp = "getProviderList";
    method->setProperty("name", QVariant(temp));
    QCOMPARE(method->property("name").toString(), QString(temp));

    temp = "http://www.daenet.de/webservices/CurrencyServer";
    method->setProperty("targetNamespace", QVariant(temp));
    QCOMPARE(method->property("targetNamespace").toString(), QString(temp));

    QMap<QString, QVariant> tmpP;
    tmpP.insert("symbol", QVariant("NOK"));
    method->setParameters(tmpP);
    QCOMPARE(method->property("parameterNames").toStringList().length(), int(1));

    delete method;
}

/*
  Checks QWebMethod operation when using the default sendmethod(QByteArray)
  */
void TestQWebMethod::asynchronousSendingTest()
{
    QWebMethod *method = new QWebMethod(0, QWebMethod::Soap12, QWebMethod::Post);
    method->setHost("http://www.currencyserver.de/webservice/currencyserverwebservice.asmx");
    method->setMethodName("getProviderList");
    method->setTargetNamespace("http://www.daenet.de/webservices/CurrencyServer");
    method->invokeMethod();
    QCOMPARE(method->isErrorState(), bool(false));

    bool result = false;
    for (int i = 0; (i < 50) && (!method->isReplyReady()); i++)
        QTest::qWait(250);

    result = method->isReplyReady();

    QCOMPARE(result, bool(true));

    delete method;
}

void TestQWebMethod::defaultGettersTest(QWebMethod *method)
{
    QCOMPARE(method->isErrorState(), bool(false));
    QCOMPARE(method->isReplyReady(), bool(false));
    QCOMPARE(method->errorInfo(), QString(""));
    QCOMPARE(method->httpMethodString(), QString("Post"));
    QCOMPARE(method->httpMethod(), QWebMethod::Post);
    QCOMPARE(method->protocolString(false), QString("Soap12"));
    QCOMPARE(method->protocolString(true), QString("Soap12"));
    QCOMPARE(method->protocol(), QWebMethod::Soap12);
    QCOMPARE(method->hostUrl(), QUrl(""));
    QCOMPARE(method->host(), QString(""));
    QCOMPARE(method->targetNamespace(), QString(""));
    QCOMPARE(method->methodName(), QString(""));
    QCOMPARE(method->returnValueNameType().size(), int(0));
    QCOMPARE(method->returnValueName().size(), int(0));
    QCOMPARE(method->parameterNamesTypes().size(), int(0));
    QCOMPARE(method->parameterNames().size(), int(0));
}

QTEST_MAIN(TestQWebMethod)
#include "tst_qwebmethod.moc"
