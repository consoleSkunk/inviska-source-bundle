#ifndef IDlgRenameErrorList_h
#define IDlgRenameErrorList_h

#include <QDialog>
class QTableWidget;
class QPushButton;
class QString;
class IUIFileList;


class IDlgRenameErrorList : public QDialog
{
    Q_OBJECT

private:
    // Table showing files that could not be renamed
    QTableWidget*           m_pqtwFileList;

    // Close button
    QPushButton*            m_pqpbCloseButton;

public:
    IDlgRenameErrorList(IUIFileList* puifmFileList);

    // Add file to failed rename list
    void AddFileToList(const QString & krqstrCurrentName, const QString & krqstrFailedRename, const QString & krqstrFailureReason);

    // Resizes colums so that text is visible and shows window
    void ResizeColumnsAndShow();
};

#endif // IDlgRenameErrorList_h
