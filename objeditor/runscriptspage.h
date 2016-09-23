#ifndef RUNSCRIPTSPAGE_H
#define RUNSCRIPTSPAGE_H

#include <QWidget>

#include <QGridLayout>

#include <functional>

namespace Ui {
class RunScriptsPage;
}

QT_BEGIN_NAMESPACE
class QProcess;
class QPushButton;
QT_END_NAMESPACE

class RunScriptsPage : public QWidget
{
    Q_OBJECT

public:
    explicit RunScriptsPage(QWidget *parent = 0);
    ~RunScriptsPage();

    using FeedArgsFunction = std::function<void(QStringList &)>;
    using ButtonPushedFunction = std::function<void(QPushButton *)>;
    QPushButton *addScriptButton(const QString &label, const QString &script, FeedArgsFunction feed_args_function, ButtonPushedFunction onButtonPushed = nullptr);

private slots:
    void processStarted();
    void readOutput();
    void readError();
    void processExit(int);

private:
    Ui::RunScriptsPage *ui;
    QGridLayout *mLayout;

    QProcess *mProcess;

    int mNw = 0;

    void runScript(const QString &script, const QStringList &args);
    void checkEnv();
};

#endif // RUNSCRIPTSPAGE_H
