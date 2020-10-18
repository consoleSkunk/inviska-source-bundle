#include <QtWidgets>
#include "IComUIPrefGeneral.h"
#include "IUIMainWindow.h"
#include "IDlgPreferences.h"


IComUIPrefGeneral::IComUIPrefGeneral(IDlgPreferences* pdprefPreferencesDlg, IUIMainWindow* pmwMainWindow) : QWidget(pdprefPreferencesDlg),
                                                                                                            m_rqsetSettings(pmwMainWindow->GetSettings())
{
    m_pmwMainWindow = pmwMainWindow;
    setupUi(this);

    m_pivalIntOnlyValidator = new QIntValidator(this);
    m_pqleWindowPositionWidth->setValidator(m_pivalIntOnlyValidator);
    m_pqleWindowPositionHeight->setValidator(m_pivalIntOnlyValidator);
    m_pqleWindowPositionX->setValidator(m_pivalIntOnlyValidator);
    m_pqleWindowPositionY->setValidator(m_pivalIntOnlyValidator);
}


void IComUIPrefGeneral::SetWidgetStates()
{
    m_rqsetSettings.beginGroup("Application");
    m_pqchSingleInstanceOnly->setChecked(m_rqsetSettings.value("SingleInstanceOnly", false).toBool());
    m_rqsetSettings.endGroup();

    QSize qsizWinSize;
    QPoint qpntWinPos;
    m_rqsetSettings.beginGroup("MainWindow");
    m_bWinPosUseSpecified = m_rqsetSettings.value("UseSpecifiedSizePos", false).toBool();
    if (m_bWinPosUseSpecified)
    {
        qsizWinSize = m_rqsetSettings.value("UserSpecifiedSize", m_pmwMainWindow->size()).toSize();
        qpntWinPos = m_rqsetSettings.value("UserSpecifiedPosition", m_pmwMainWindow->pos()).toPoint();
    }
    else
    {
        qsizWinSize = m_pmwMainWindow->size();
        qpntWinPos = m_pmwMainWindow->pos();
    }
    m_pqrbWindowPositionUseSpecified->setChecked(m_bWinPosUseSpecified);
    m_pqleWindowPositionWidth->setText(QString::number(qsizWinSize.width()));
    m_pqleWindowPositionHeight->setText(QString::number(qsizWinSize.height()));
    m_pqleWindowPositionX->setText(QString::number(qpntWinPos.x()));
    m_pqleWindowPositionY->setText(QString::number(qpntWinPos.y()));
    m_rqsetSettings.endGroup();

    m_rqsetSettings.beginGroup("VersionCheck");
    if (m_rqsetSettings.value("CheckForNewVersion", true).toBool() == false)
        m_pqrbUpdatesDoNotCheck->setChecked(true);
    else if (m_rqsetSettings.value("NotifyWhenNewVersion", true).toBool() == false)
        m_pqrbUpdatesCheckOnly->setChecked(true);
    else
        m_pqrbUpdatesCheckAndNotify->setChecked(true);
    m_rqsetSettings.endGroup();

    m_bWinPosUseSpecified = m_pqrbWindowPositionUseSpecified->isChecked();
    m_qrctWinRect.setRect(qpntWinPos.x(), qpntWinPos.y(), qsizWinSize.width(), qsizWinSize.height());
}


bool IComUIPrefGeneral::SaveSettings()
{
    if (ValidateWinPositionAndMove() == false)
        return false;

    m_rqsetSettings.beginGroup("Application");
    m_rqsetSettings.setValue("SingleInstanceOnly", m_pqchSingleInstanceOnly->isChecked());
    m_rqsetSettings.endGroup();

    m_rqsetSettings.beginGroup("MainWindow");
    bool bUseSpecifiedSizePos = m_pqrbWindowPositionUseSpecified->isChecked();
    m_rqsetSettings.setValue("UseSpecifiedSizePos", bUseSpecifiedSizePos);
    if (bUseSpecifiedSizePos)
    {
        QSize qsizWinSize(m_pqleWindowPositionWidth->text().toInt(), m_pqleWindowPositionHeight->text().toInt());
        QPoint qpntWinPos(m_pqleWindowPositionX->text().toInt(), m_pqleWindowPositionY->text().toInt());
        m_rqsetSettings.setValue("UserSpecifiedSize", qsizWinSize);
        m_rqsetSettings.setValue("UserSpecifiedPosition", qpntWinPos);
    }
    m_rqsetSettings.endGroup();

    m_rqsetSettings.beginGroup("VersionCheck");
    if (m_pqrbUpdatesCheckAndNotify->isChecked())
    {
        m_rqsetSettings.setValue("CheckForNewVersion", true);
        m_rqsetSettings.setValue("NotifyWhenNewVersion", true);
    }
    else if (m_pqrbUpdatesCheckOnly->isChecked())
    {
        m_rqsetSettings.setValue("CheckForNewVersion", true);
        m_rqsetSettings.setValue("NotifyWhenNewVersion", false);
    }
    else //if (m_pqrbUpdatesDoNotCheck->isChecked())
    {
        m_rqsetSettings.setValue("CheckForNewVersion", false);
        m_rqsetSettings.setValue("NotifyWhenNewVersion", false);
    }
    m_rqsetSettings.endGroup();

    return true;
}


void IComUIPrefGeneral::ProcessChanges()
{
    // Change language here
}


bool IComUIPrefGeneral::ValidateWinPositionAndMove()
{
    bool bNewWinPosUseSpecified = m_pqrbWindowPositionUseSpecified->isChecked();
    if (bNewWinPosUseSpecified)
    {
        QRect m_qrctNewRect(m_pqleWindowPositionX->text().toInt(), m_pqleWindowPositionY->text().toInt(), m_pqleWindowPositionWidth->text().toInt(), m_pqleWindowPositionHeight->text().toInt());
        if (m_pmwMainWindow->IsWindowOnScreen(m_qrctNewRect) == false)
        {
            QMessageBox::warning(this, tr("Window Position Error"),
                                 tr("The specified window position and size would put the window or title bar off screen.\n"
                                    "Please adjust the size and position values accordingly."), QMessageBox::Ok);
            return false;
        }

        if (bNewWinPosUseSpecified != m_bWinPosUseSpecified || m_qrctWinRect != m_qrctNewRect)
            m_pmwMainWindow->setGeometry(m_qrctNewRect);
    }

    return true;
}
