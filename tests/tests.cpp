#include <QTest>

#include "qtutils.h"

class TestRunner : public QObject
{
   Q_OBJECT
public:
   TestRunner() = default;

private Q_SLOTS:
   void testCaseRun() {}
};

#include "tests.moc"

int main(int argc, char** argv) {
    TestRunner runner;
    return QTest::qExec(&runner, argc, argv);
}
