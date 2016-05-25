#include <QTest>
#include <QScopedPointer>
#include <memory>

#include "qtutils.h"

namespace {
// Helper class to get access to protected fields of QtUtils::Connector.
template <typename T1, typename T2>
struct ConnectorFriend : protected QtUtils::Connector<T1, T2> {
public:
    typedef QtUtils::Connector<T1, T2> BaseType;
    typedef typename BaseType::SignallingType SignallingType;
    typedef typename BaseType::ReceivingType ReceivingType;

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

class ConnectorTestRunner : public QObject
{
   Q_OBJECT
public:
   ConnectorTestRunner() = default;

private Q_SLOTS:

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
};

#include "connectortests.moc"

std::unique_ptr<QObject> connectorTestRunner() {
    return std::make_unique<ConnectorTestRunner>();
}
