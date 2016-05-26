#include <QTest>
#include <QScopedPointer>
#include <QString>
#include <memory>

#include "qtutils.h"

namespace {
// Helper class to get access to protected fields of QtUtils::Connector.
template <typename T1, typename T2>
struct ConnectorFriend : protected QtUtils::Connector<T1, T2> {
public:
    using BaseType = QtUtils::Connector<T1, T2>;
    using SignallingType = typename BaseType::SignallingType;
    using ReceivingType = typename BaseType::ReceivingType;

    static ReceivingType getSignalObj(const BaseType& connector) {
        return ConnectorFriend(connector).m_signallingObject;
    }

    static SignallingType getRecvObj(const BaseType& connector) {
        return ConnectorFriend(connector).m_receivingObject;
    }

private:
    ConnectorFriend(const BaseType& other) : BaseType(other) {}
};

// Actual helpers to get data form connectors.
// There is a way to get this with less amount of code:
// static_cast<const ConnectorFriend&>(connector).m_receivingObject
// But it's like a hack of C++ type system.
template <typename T1, typename T2>
inline typename QtUtils::Connector<T1, T2>::SignallingType
getSignalObj(const QtUtils::Connector<T1, T2>& connector) {
    return ConnectorFriend<T1, T2>::getSignalObj(connector);
}

template <typename T1, typename T2>
inline typename QtUtils::Connector<T1, T2>::ReceivingType
getRecvObj(const QtUtils::Connector<T1, T2>& connector) {
    return ConnectorFriend<T1, T2>::getRecvObj(connector);
}

struct PlainObject {};
}

// Simple Qt signal emitter.
class TestSignalEmitter : public QObject
{
   Q_OBJECT
public:
    TestSignalEmitter () = default;

Q_SIGNALS:
    void onSignal(const QString& someMessage);
};

// Simple signal handler.
// Will save latest signal data.
class TestSignalHandler : public QObject
{
   Q_OBJECT
public:
    TestSignalHandler () = default;

    const QString& lastSignalMessage() const { return m_lastSignalMessage; }

public Q_SLOTS:
    void handleSignal(const QString& someMessage) {
        m_lastSignalMessage = someMessage;
    }

private:
    QString m_lastSignalMessage;
};

// Main runner for test cases in this file.
class ConnectorTestRunner : public QObject
{
   Q_OBJECT
public:
   ConnectorTestRunner() = default;

private Q_SLOTS:
    // All tests.

    void testCaseCheckObjects() {
        const PlainObject obj = {};
        const QtUtils::Connector<PlainObject, PlainObject> connector(&obj);
        QCOMPARE(&obj, getSignalObj(connector));
        QCOMPARE(&obj, getRecvObj(connector));

        const PlainObject obj2 = {};
        const QtUtils::Connector<PlainObject, PlainObject> connector2(&obj, &obj2);
        QCOMPARE(&obj, getSignalObj(connector2));
        QCOMPARE(&obj2, getRecvObj(connector2));
    }

    void testCaseCheckCheckDefaultQObjects() {
        const QObject* const obj = reinterpret_cast<QObject*>(0xDEADBEEF);
        const QtUtils::Connector<> connector(obj);
        QCOMPARE(obj, getSignalObj(connector));
        QCOMPARE(obj, getRecvObj(connector));
    }

    void testCaseCheckCheckConnectionType() {
        const PlainObject obj = {};
        const PlainObject obj2 = {};
        const QtUtils::ConnectorWithType<PlainObject, PlainObject>
                connector(&obj, &obj2, Qt::QueuedConnection);
        QCOMPARE(&obj, getSignalObj(connector));
        QCOMPARE(&obj2, getRecvObj(connector));
        QCOMPARE(Qt::QueuedConnection, connector.type());
    }

    void testCaseCheckCheckMakers() {
        const PlainObject obj = {};
        const PlainObject obj2 = {};
        const QtUtils::Connector<PlainObject, PlainObject> connector(&obj, &obj2);
        const auto& copyConnector = QtUtils::makeConnector(&obj, &obj2);
        QCOMPARE(getSignalObj(copyConnector), getSignalObj(connector));
        QCOMPARE(getRecvObj(copyConnector), getRecvObj(connector));

        const QtUtils::ConnectorWithType<PlainObject, PlainObject>
                queuedConnector(&obj, &obj2, Qt::QueuedConnection);
        const auto& copyQueuedConnector = QtUtils::makeQueuedConnector(&obj, &obj2);
        QCOMPARE(getSignalObj(copyQueuedConnector), getSignalObj(queuedConnector));
        QCOMPARE(getRecvObj(copyQueuedConnector), getRecvObj(queuedConnector));
        QCOMPARE(copyQueuedConnector.type(), queuedConnector.type());
    }

    void testCaseCheckCheckSmartPointersMakers() {
        QScopedPointer<PlainObject> smartPointer(new PlainObject());
        QScopedPointer<PlainObject> smartPointer2(new PlainObject());
        const auto& connector = QtUtils::makeConnector(smartPointer, smartPointer2);
        QCOMPARE(smartPointer.data(), getSignalObj(connector));
        QCOMPARE(smartPointer2.data(), getRecvObj(connector));

        const auto& queuedConnector =
                QtUtils::makeQueuedConnector(smartPointer, smartPointer2);
        QCOMPARE(smartPointer.data(), getSignalObj(queuedConnector));
        QCOMPARE(smartPointer2.data(), getRecvObj(queuedConnector));
    }

    void testCaseEmitSignal() {
        TestSignalEmitter signalEmitter;
        TestSignalHandler signalHandler;

        const auto& connector = QtUtils::makeConnector(&signalEmitter, &signalHandler);
        connector.connect(&TestSignalEmitter::onSignal, &TestSignalHandler::handleSignal);

        const QString testMessage = QString::fromLatin1("CONNECT");
        emit signalEmitter.onSignal(testMessage);

        QCOMPARE(testMessage, signalHandler.lastSignalMessage());

        connector.disconnect(&TestSignalEmitter::onSignal, &TestSignalHandler::handleSignal);
        emit signalEmitter.onSignal(QString::fromLatin1("DISCONNECT"));

        QCOMPARE(testMessage, signalHandler.lastSignalMessage());
    }
};

#include "connectortests.moc"

std::unique_ptr<QObject> connectorTestRunner() {
    return std::make_unique<ConnectorTestRunner>();
}
