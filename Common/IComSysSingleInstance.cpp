#include <QCryptographicHash>
#include "IComSysSingleInstance.h"


IComSysSingleInstance::IComSysSingleInstance(const QString & krqstrKey) : m_qstrKey(krqstrKey),
                                                                          m_qstrSharedMemKey(GenerateKeyHash(m_qstrKey, "_SharedMemKey")),
                                                                          m_qsmSharedMem(m_qstrSharedMemKey),
                                                                          m_qstrSharedMemSemaKey(GenerateKeyHash(m_qstrKey, "_SharedMemSemaKey")),
                                                                          m_qssSharedMemSema(m_qstrSharedMemSemaKey, 1)
{
    m_qssSharedMemSema.acquire();
    {
        // Fix for *nix: http://habrahabr.ru/post/173281/
        QSharedMemory qsmFix(m_qstrSharedMemKey);
        qsmFix.attach();
    }
    m_qssSharedMemSema.release();
}


IComSysSingleInstance::~IComSysSingleInstance()
{
    Release();
}


bool IComSysSingleInstance::TryToRun()
{
    if (AnotherInstanceRunning())
        return false;

    m_qssSharedMemSema.acquire();
    const bool bResult = m_qsmSharedMem.create(sizeof(int));
    *reinterpret_cast<int*>(m_qsmSharedMem.data()) = 0;
    m_qssSharedMemSema.release();

    if (bResult == false)
        Release();

    return bResult;
}


bool IComSysSingleInstance::AnotherInstanceRunning()
{
    m_qssSharedMemSema.acquire();
    const bool bIsRunning = m_qsmSharedMem.attach();
    if (bIsRunning)
    {
        *reinterpret_cast<int*>(m_qsmSharedMem.data()) = 1;
        m_qsmSharedMem.detach();
    }
    m_qssSharedMemSema.release();

    return bIsRunning;
}


void IComSysSingleInstance::Release()
{
    m_qssSharedMemSema.acquire();
    if (m_qsmSharedMem.isAttached())
        m_qsmSharedMem.detach();
    m_qssSharedMemSema.release();
}


QString IComSysSingleInstance::GenerateKeyHash(const QString & krqstrKey, const QString & krqstrSalt)
{
    QByteArray qbaData;

    qbaData.append(krqstrKey.toUtf8());
    qbaData.append(krqstrSalt.toUtf8());
    qbaData = QCryptographicHash::hash(qbaData, QCryptographicHash::Sha1).toHex();

    return qbaData;
}


bool IComSysSingleInstance::CheckInstanceFlag()
{
    bool bOtherInstanceStarted = false;
    m_qssSharedMemSema.acquire();
    int* piFlag = reinterpret_cast<int*>(m_qsmSharedMem.data());
    if (*piFlag == 1)
    {
        bOtherInstanceStarted = true;
        *piFlag = 0;


    }
    m_qssSharedMemSema.release();

    return bOtherInstanceStarted;
}
