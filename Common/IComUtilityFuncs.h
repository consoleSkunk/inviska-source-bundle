#ifndef IComUtilityFuncs_h
#define IComUtilityFuncs_h

#include <QUrl>
class QTableWidget;

class IComUtilityFuncs
{
public:
    // Returns the row height to use for a QTableWidget based on the current font
    static int GetTableRowHeightFitToFont(QTableWidget* pqtwTable);

    // Returns URL for My Computer on Windows - Other GUIDs are at: https://msdn.microsoft.com/en-us/library/windows/desktop/dd378457.aspx
    static QUrl GetMyComputerURL()  {return QUrl("clsid:0AC0837C-BBF8-452A-850D-79D08E667CA7");}
};

#endif // IComUtilityFuncs_h
