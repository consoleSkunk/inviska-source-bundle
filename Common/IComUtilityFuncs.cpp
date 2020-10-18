#include <QtWidgets>
#include "IComUtilityFuncs.h"


int IComUtilityFuncs::GetTableRowHeightFitToFont(QTableWidget* pqtwTable)
{
    //return pqtwTable->verticalHeader()->fontMetrics().height()+2;

    const int kiMinRowHeight = 17;
    int iRowHeight = pqtwTable->verticalHeader()->fontMetrics().height()+2;
    if (iRowHeight < kiMinRowHeight)
        return kiMinRowHeight;
    return iRowHeight;
}


