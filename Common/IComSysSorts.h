#ifndef IComSysSorts_h
#define IComSysSorts_h

#include <QTableWidgetSelectionRange>


struct IComTableWidgetSelectionSortAsc
{
    inline bool operator() (const QTableWidgetSelectionRange & krqtwirSelection1, const QTableWidgetSelectionRange & krqtwirSelection2)
    {
        return (krqtwirSelection1.topRow() < krqtwirSelection2.topRow());
    }
};


struct IComTableWidgetSelectionSortDesc
{
    inline bool operator() (const QTableWidgetSelectionRange & krqtwirSelection1, const QTableWidgetSelectionRange & krqtwirSelection2)
    {
        return (krqtwirSelection1.bottomRow() > krqtwirSelection2.bottomRow());
    }
};



#endif // IComSysSorts_h
