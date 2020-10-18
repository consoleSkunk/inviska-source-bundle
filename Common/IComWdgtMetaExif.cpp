#include <QtWidgets>
#include "IComWdgtMetaExif.h"
#include "IComUtilityFuncs.h"
#include "IUIMainWindow.h"


IComWdgtMetaExif* IComWdgtMetaExif::CreateWidget(const QString & krqstrFilePath, QWidget* pqwParent)
{
    IComWdgtMetaExif* pwmeExifMetaWidget = nullptr;
    IComMetaExif mexExifMeta(krqstrFilePath);

    if (mexExifMeta.ExifDataPresent())
        pwmeExifMetaWidget = new IComWdgtMetaExif(mexExifMeta, pqwParent);

    return pwmeExifMetaWidget;
}


IComWdgtMetaExif::IComWdgtMetaExif(IComMetaExif & rmexExifMeta, QWidget* pqwParent) : QTableWidget(pqwParent)
{
    m_bSkipEmptyRows = false;

    setColumnCount(2);
    setHorizontalHeaderLabels(QStringList() << tr("Tag") << tr("Value"));
    setStyleSheet("QTableWidget::item { padding: 0px; }");
    verticalHeader()->setMinimumSectionSize(17);
    verticalHeader()->setDefaultSectionSize(IComUtilityFuncs::GetTableRowHeightFitToFont(this));
    verticalHeader()->hide();

    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    horizontalHeader()->setSectionsClickable(false);

    setEditTriggers(QAbstractItemView::NoEditTriggers);

    PopulateTable(rmexExifMeta);
}


void IComWdgtMetaExif::PopulateTable(IComMetaExif & rmexExifMeta)
{
    PopulateTableAddDivision(tr("Camera"));
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_0,      EXIF_TAG_MAKE);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_0,      EXIF_TAG_MODEL);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_0,      EXIF_TAG_SOFTWARE);

    PopulateTableAddDivision(tr("Date Time"));
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_DATE_TIME_ORIGINAL);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_DATE_TIME_DIGITIZED);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_0,      EXIF_TAG_DATE_TIME);
    // Of the sample pictures I downloaded only Canon encods these values, and they were blank for the other brands
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_SUB_SEC_TIME_ORIGINAL);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_SUB_SEC_TIME_DIGITIZED);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_SUB_SEC_TIME);

    PopulateTableAddDivision(tr("Camera Settings"));
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_FNUMBER);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_ISO_SPEED_RATINGS);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_EXPOSURE_TIME);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_EXPOSURE_PROGRAM);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_FOCAL_LENGTH);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_FLASH);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_0,      EXIF_TAG_ORIENTATION);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_DIGITAL_ZOOM_RATIO);

    PopulateTableAddDivision(tr("Advanced Camera Settings"));
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_SHUTTER_SPEED_VALUE);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_MAX_APERTURE_VALUE);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_EXPOSURE_BIAS_VALUE);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_BRIGHTNESS_VALUE);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_APERTURE_VALUE);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_SCENE_CAPTURE_TYPE);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_CUSTOM_RENDERED);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_EXPOSURE_MODE);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_WHITE_BALANCE);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_GAIN_CONTROL);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_CONTRAST);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_SATURATION);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_SHARPNESS);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_METERING_MODE);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_LIGHT_SOURCE);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_FOCAL_PLANE_X_RESOLUTION);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_FOCAL_PLANE_Y_RESOLUTION);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_SUBJECT_DISTANCE_RANGE);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_SENSING_METHOD);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_SCENE_TYPE);

    PopulateTableAddDivision(tr("Image Information"));
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_PIXEL_X_DIMENSION);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_PIXEL_Y_DIMENSION);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_COLOR_SPACE);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_1,      EXIF_TAG_COMPRESSION);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_COMPONENTS_CONFIGURATION);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_COMPRESSED_BITS_PER_PIXEL);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_0,      EXIF_TAG_YCBCR_POSITIONING);
    /* On some camers these give different values - Not sure which to use
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_0,      EXIF_TAG_X_RESOLUTION);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_0,      EXIF_TAG_Y_RESOLUTION);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_0,      EXIF_TAG_RESOLUTION_UNIT);*/
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_1,      EXIF_TAG_X_RESOLUTION);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_1,      EXIF_TAG_Y_RESOLUTION);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_1,      EXIF_TAG_RESOLUTION_UNIT);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_FILE_SOURCE);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_EXIF_VERSION);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_EXIF,   EXIF_TAG_FLASH_PIX_VERSION);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_INTEROPERABILITY, EXIF_TAG_INTEROPERABILITY_INDEX);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_INTEROPERABILITY, EXIF_TAG_INTEROPERABILITY_VERSION);

    PopulateTableAddDivision(tr("Image Description"));
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_0,      EXIF_TAG_XP_TITLE);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_0,      EXIF_TAG_XP_SUBJECT);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_0,      EXIF_TAG_XP_AUTHOR);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_0,      EXIF_TAG_XP_COMMENT);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_0,      EXIF_TAG_XP_KEYWORDS);
    PopulateTableAddItem(rmexExifMeta,  EXIF_IFD_0,      EXIF_TAG_COPYRIGHT);
}


void IComWdgtMetaExif::PopulateTableAddDivision(const QString & krqstrTitle)
{
    const int kiRowCount = rowCount();
    insertRow(kiRowCount);

    QTableWidgetItem* pqtwiItem;
    pqtwiItem = new QTableWidgetItem(krqstrTitle);
    pqtwiItem->setBackground(QColor::fromRgb(192, 192, 192));
    pqtwiItem->setFlags(pqtwiItem->flags() ^ (Qt::ItemIsEditable | Qt::ItemIsSelectable));
    setItem(kiRowCount, 0, pqtwiItem);

    pqtwiItem = new QTableWidgetItem;
    pqtwiItem->setBackground(QColor::fromRgb(192, 192, 192));
    pqtwiItem->setFlags(pqtwiItem->flags() ^ (Qt::ItemIsEditable | Qt::ItemIsSelectable));
    setItem(kiRowCount, 1, pqtwiItem);
}


void IComWdgtMetaExif::PopulateTableAddItem(IComMetaExif & rmexExifMeta, const ExifIfd kexidID, const ExifTag kextaTag)
{
    QString qstrValue;
    rmexExifMeta.GetValue(qstrValue, kexidID, kextaTag);
    if (m_bSkipEmptyRows && qstrValue.isEmpty())
        return;

    const int kiRowCount = rowCount();
    insertRow(kiRowCount);

    QTableWidgetItem* pqtwiItem;
    pqtwiItem = new QTableWidgetItem(rmexExifMeta.GetDescription(kexidID, kextaTag));
    pqtwiItem->setFlags(pqtwiItem->flags() ^ (Qt::ItemIsEditable | Qt::ItemIsSelectable));
    setItem(kiRowCount, 0, pqtwiItem);

    pqtwiItem = new QTableWidgetItem(qstrValue);
    pqtwiItem->setFlags(pqtwiItem->flags() ^ (Qt::ItemIsEditable | Qt::ItemIsSelectable));
    setItem(kiRowCount, 1, pqtwiItem);
}


void IComWdgtMetaExif::DisplayTags(const QString & krqstrFilePath)
{
    IComWdgtMetaExif* pemwExifMetaWidget = IComWdgtMetaExif::CreateWidget(krqstrFilePath);
    if (pemwExifMetaWidget == nullptr)
        return;

    QString qstrFileName = QDir::fromNativeSeparators(krqstrFilePath);
    qstrFileName = qstrFileName.mid(qstrFileName.lastIndexOf('/')+1);

    QDialog* pqdlgDialog = new QDialog(IUIMainWindow::GetMainWindow());
    pqdlgDialog->setWindowTitle(qstrFileName + tr(" - Exif Info"));
    pqdlgDialog->setWindowFlags(pqdlgDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    pqdlgDialog->setModal(true);
    pqdlgDialog->resize(400, 600);

    QPushButton* pqpbOkay = new QPushButton(tr("OK"), pqdlgDialog);
    pqpbOkay->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    pqdlgDialog->connect(pqpbOkay, SIGNAL(clicked()), pqdlgDialog, SLOT(close()));

    QVBoxLayout* pqvblLayout = new QVBoxLayout;
    pqvblLayout->addWidget(pemwExifMetaWidget);
    pqvblLayout->addWidget(pqpbOkay);
    pqvblLayout->setAlignment(pqpbOkay, Qt::AlignCenter);
    pqdlgDialog->setLayout(pqvblLayout);

    pqdlgDialog->exec();
    delete pqdlgDialog;
}
