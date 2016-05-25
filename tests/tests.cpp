#include <QTest>
#include <memory>

extern std::unique_ptr<QObject> connectorTestRunner();

int main(int argc, char** argv) {
    auto test_runner = connectorTestRunner();
    return QTest::qExec(test_runner.get(), argc, argv);
}
