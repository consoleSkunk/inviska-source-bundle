#ifndef IComSysMKVToolNixVer_h
#define IComSysMKVToolNixVer_h

#include <QObject>
#include <QProcess>
#include <QString>
class IUIMainWindow;
class IComSysMKVToolNix;


class IComSysMKVToolNixVer : public QObject
{
    Q_OBJECT

private:
    // For use as parent for message boxes and to get access to UI
    IUIMainWindow*          m_pmwMainWindow;

    // For setting version
    IComSysMKVToolNix*      m_pimkvMKVToolNix;

    // Process for running MKVMerge to check version
    QProcess                m_qprocMKVMerge;

    // Contains name of MKVToolNix executable files used by the program
    QString                 m_qstrVersionString;

public:
    IComSysMKVToolNixVer(IUIMainWindow* pmwMainWindow, IComSysMKVToolNix* pimkvMKVToolNix);

    // Runs MKVMerge to determine version
    void DetermineMKVToolNixVersion(const QString & krqstrMKVMergePath);

private slots:
    // Functions for pocessing output from MKVMerge version string
    void MKVMergeOutputText();
    void MKVMergeFinished(const int kiExitCode, const QProcess::ExitStatus kqpesExitStatus);
    void MKVMergeErrorText();
    void MKVMergeError(const QProcess::ProcessError kqpeError);
};

#endif // IComSysMKVToolNixVer_h

