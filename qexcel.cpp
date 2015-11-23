#include <QAxObject>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QDebug>

#include "qexcel.h"

QExcel::QExcel(QString xlsFilePath, QObject *parent)
{
    m_excel = 0;
    m_workBooks = 0;
    m_workBook = 0;
    m_sheets = 0;
    m_sheet = 0;

    m_excel = new QAxObject("Excel.Application", parent);
    m_workBooks = m_excel->querySubObject("Workbooks");
    QFile file(xlsFilePath);
    if (file.exists())
    {
        m_workBooks->dynamicCall("Open(const QString&)", xlsFilePath);
        m_workBook = m_excel->querySubObject("ActiveWorkBook");
        connect(m_workBook,SIGNAL(exception(int,QString,QString,QString)),
                this,SLOT(onQExcelException(int,QString,QString,QString)));

        m_sheets = m_workBook->querySubObject("WorkSheets");
        connect(m_workBook,SIGNAL(exception(int,QString,QString,QString)),
                this,SLOT(onQExcelException(int,QString,QString,QString)));
    }
}

QExcel::~QExcel()
{
    close();

    if (0 != m_sheet)
    {
        delete m_sheet;
        m_sheet = 0;
    }

    if (0 != m_sheets)
    {
        delete m_sheets;
        m_sheets = 0;
    }

    if (0 != m_workBook)
    {
        delete m_workBook;
        m_workBook = 0;
    }

    if (0 != m_workBooks)
    {
        delete m_workBooks;
        m_workBooks = 0;
    }

    if (0 != m_excel)
    {
        delete m_excel;
        m_excel = 0;
    }

}

void QExcel::close()
{
    m_excel->dynamicCall("Quit()");
}

QAxObject *QExcel::getWorkBooks()
{
    return m_workBooks;
}

QAxObject *QExcel::getWorkBook()
{
    return m_workBook;
}

QAxObject *QExcel::getWorkSheets()
{
    return m_sheets;
}

QAxObject *QExcel::getWorkSheet()
{
    return m_sheet;
}

void QExcel::selectSheet(const QString& sheetName)
{
    m_sheet = m_sheets->querySubObject("Item(const QString&)", sheetName);
}

void QExcel::deleteSheet(const QString& sheetName)
{
    QAxObject * a = m_sheets->querySubObject("Item(const QString&)", sheetName);
    a->dynamicCall("delete");
}

void QExcel::deleteSheet(int sheetIndex)
{
    QAxObject * a = m_sheets->querySubObject("Item(int)", sheetIndex);
    a->dynamicCall("delete");
}

void QExcel::selectSheet(int sheetIndex)
{
    m_sheet = m_sheets->querySubObject("Item(int)", sheetIndex);
}

void QExcel::setCellString(int row, int column, const QString& value)
{
    QAxObject *range = m_sheet->querySubObject("Cells(int,int)", row, column);
    range->dynamicCall("SetValue(const QString&)", value);
}

void QExcel::setCellVariant(int row, int column, const QVariant& value)
{
    QAxObject *range = m_sheet->querySubObject("Cells(int,int)", row, column);
    range->dynamicCall("SetValue(const QVariant&)", value);
}

void QExcel::setCellFontBold(int row, int column, bool isBold)
{
    QString cell;
    cell.append(QChar(column - 1 + 'A'));
    cell.append(QString::number(row));

    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", cell);
    range = range->querySubObject("Font");
    range->setProperty("Bold", isBold);
}

void QExcel::setCellFontSize(int row, int column, int size)
{
    QString cell;
    cell.append(QChar(column - 1 + 'A'));
    cell.append(QString::number(row));

    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", cell);
    range = range->querySubObject("Font");
    range->setProperty("Size", size);
}

void QExcel::mergeCells(const QString& cell)
{
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", cell);
    range->setProperty("VerticalAlignment", -4108);//xlCenter
    range->setProperty("WrapText", true);
    range->setProperty("MergeCells", true);
}

void QExcel::mergeCells(int topLeftRow, int topLeftColumn, int bottomRightRow, int bottomRightColumn)
{
    QString cell;
    cell.append(QChar(topLeftColumn - 1 + 'A'));
    cell.append(QString::number(topLeftRow));
    cell.append(":");
    cell.append(QChar(bottomRightColumn - 1 + 'A'));
    cell.append(QString::number(bottomRightRow));

    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", cell);
    range->setProperty("VerticalAlignment", -4108);//xlCenter
    range->setProperty("WrapText", true);
    range->setProperty("MergeCells", true);
}

QVariant QExcel::getCellValue(int row, int column)
{
    QAxObject *range = m_sheet->querySubObject("Cells(int,int)", row, column);
    return range->property("Value");
}

void QExcel::save()
{
    m_workBook->dynamicCall("Save()");
}

void QExcel::saveAs(const QString& fileName,const QString& fileFormat)
{
    m_workBook->dynamicCall("SaveAs(const QString&,const QString&)",fileName,fileFormat);
}

int QExcel::getSheetsCount()
{
    return m_sheets->property("Count").toInt();
}

QString QExcel::getSheetName()
{
    return m_sheet->property("Name").toString();
}

QString QExcel::getSheetName(int sheetIndex)
{
    QAxObject * a = m_sheets->querySubObject("Item(int)", sheetIndex);
    return a->property("Name").toString();
}

void QExcel::getUsedRange(int *topLeftRow, int *topLeftColumn, int *bottomRightRow, int *bottomRightColumn)
{
    QAxObject *usedRange = m_sheet->querySubObject("UsedRange");
    *topLeftRow = usedRange->property("Row").toInt();
    *topLeftColumn = usedRange->property("Column").toInt();

    QAxObject *rows = usedRange->querySubObject("Rows");
    *bottomRightRow = *topLeftRow + rows->property("Count").toInt() - 1;

    QAxObject *columns = usedRange->querySubObject("Columns");
    *bottomRightColumn = *topLeftColumn + columns->property("Count").toInt() - 1;
}

void QExcel::setColumnWidth(int column, int width)
{
    QString columnName;
    columnName.append(QChar(column - 1 + 'A'));
    columnName.append(":");
    columnName.append(QChar(column - 1 + 'A'));

    QAxObject * col = m_sheet->querySubObject("Columns(const QString&)", columnName);
    col->setProperty("ColumnWidth", width);
}

void QExcel::setColumnWidth(QString column, int width)
{
        column = column + ":" + column;

        QAxObject * col = m_sheet->querySubObject("Columns(const QString&)", column);
        col->setProperty("ColumnWidth", width);
}

void QExcel::setCellTextCenter(int row, int column)
{
    QString cell;
    cell.append(QChar(column - 1 + 'A'));
    cell.append(QString::number(row));

    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", cell);
    range->setProperty("HorizontalAlignment", -4108);//xlCenter
}

void QExcel::setCellTextWrap(int row, int column, bool isWrap)
{
    QString cell;
    cell.append(QChar(column - 1 + 'A'));
    cell.append(QString::number(row));

    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", cell);
    range->setProperty("WrapText", isWrap);
}

void QExcel::setAutoFitRow(int row)
{
    QString rowsName;
    rowsName.append(QString::number(row));
    rowsName.append(":");
    rowsName.append(QString::number(row));

    QAxObject * rows = m_sheet->querySubObject("Rows(const QString &)", rowsName);
    rows->dynamicCall("AutoFit()");
}

void QExcel::insertSheet(QString sheetName)
{
    m_sheets->querySubObject("Add()");
    QAxObject * a = m_sheets->querySubObject("Item(int)", 1);
    a->setProperty("Name", sheetName);
}

void QExcel::mergeSerialSameCellsInAColumn(int column, int topRow)
{
    int a,b,c,rowsCount;
    getUsedRange(&a, &b, &rowsCount, &c);

    int aMergeStart = topRow, aMergeEnd = topRow + 1;

    QString value;
    while(aMergeEnd <= rowsCount)
    {
        value = getCellValue(aMergeStart, column).toString();
        while(value == getCellValue(aMergeEnd, column).toString())
        {
            clearCell(aMergeEnd, column);
            aMergeEnd++;
        }
        aMergeEnd--;
        mergeCells(aMergeStart, column, aMergeEnd, column);

        aMergeStart = aMergeEnd + 1;
        aMergeEnd = aMergeStart + 1;
    }
}

void QExcel::clearCell(int row, int column)
{
    QString cell;
    cell.append(QChar(column - 1 + 'A'));
    cell.append(QString::number(row));

    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", cell);
    range->dynamicCall("ClearContents()");
}

void QExcel::clearCell(const QString& cell)
{
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", cell);
    range->dynamicCall("ClearContents()");
}

int QExcel::getUsedRowsCount()
{
    QAxObject *usedRange = m_sheet->querySubObject("UsedRange");
    int topRow = usedRange->property("Row").toInt();
    QAxObject *rows = usedRange->querySubObject("Rows");
    int bottomRow = topRow + rows->property("Count").toInt() - 1;
    return bottomRow;
}

void QExcel::setCellString(const QString& cell, const QString& value)
{
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", cell);
    range->dynamicCall("SetValue(const QString&)", value);
}

void QExcel::setCellFontSize(const QString &cell, int size)
{
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", cell);
    range = range->querySubObject("Font");
    range->setProperty("Size", size);
}

void QExcel::setCellTextCenter(const QString &cell)
{
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", cell);
    range->setProperty("HorizontalAlignment", -4108);//xlCenter
}

void QExcel::setCellFontBold(const QString &cell, bool isBold)
{
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", cell);
    range = range->querySubObject("Font");
    range->setProperty("Bold", isBold);
}

void QExcel::setCellTextWrap(const QString &cell, bool isWrap)
{
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", cell);
    range->setProperty("WrapText", isWrap);
}

void QExcel::setRowHeight(int row, int height)
{
    QString rowsName;
    rowsName.append(QString::number(row));
    rowsName.append(":");
    rowsName.append(QString::number(row));

    QAxObject * r = m_sheet->querySubObject("Rows(const QString &)", rowsName);
    r->setProperty("RowHeight", height);
}


void QExcel::onQExcelException(int code, const QString &source, const QString &desc, const QString &help)
{
    qDebug() << code;
    qDebug() << source;
    qDebug() << desc;
    qDebug() << help;
}
