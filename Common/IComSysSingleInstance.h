#ifndef IComSysSingleInstance_h
#define IComSysSingleInstance_h

#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>


class IComSysSingleInstance
{
private:
    // Key used to generate other keys
    const QString           m_qstrKey;

    // Shared memory object and key
    const QString           m_qstrSharedMemKey;
    QSharedMemory           m_qsmSharedMem;

    // Semaphore for controlling access to shared memory
    const QString           m_qstrSharedMemSemaKey;
    QSystemSemaphore        m_qssSharedMemSema;

    Q_DISABLE_COPY(IComSysSingleInstance)

public:
    IComSysSingleInstance(const QString & krqstrKey);
    ~IComSysSingleInstance();

    // If the shared memory has not been allocated this function allocats it
    bool TryToRun();

private:
    // Checks if another instance is running by checking if shared memory is already allocated
    bool AnotherInstanceRunning();

    // Releases shared memory
    void Release();

    // Generated hash key for shared memory and semaphore
    QString GenerateKeyHash(const QString & krqstrKey, const QString & krqstrSalt);

public:
    // Checks if shared memory flag has been set, and if so clears it and returns true
    bool CheckInstanceFlag();
};


#endif // IComSysSingleInstance_h
