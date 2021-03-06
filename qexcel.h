#ifndef QEXCEL_H
#define QEXCEL_H

#include <QString>
#include <QVariant>

class QAxObject;

class QExcel : public QObject
{
    Q_OBJECT
public:
    explicit QExcel(QString xlsFilePath, QObject *parent = 0);
    ~QExcel();

public:
    QAxObject * getWorkBooks();
    QAxObject * getWorkBook();
    QAxObject * getWorkSheets();
    QAxObject * getWorkSheet();

public:
    /**************************************************************************/
    /* 工作表                                                                 */
    /**************************************************************************/
    void selectSheet(const QString& sheetName);
    //sheetIndex 起始于 1
    void selectSheet(int sheetIndex);
    void deleteSheet(const QString& sheetName);
    void deleteSheet(int sheetIndex);
    void insertSheet(QString sheetName);
    int getSheetsCount();
    //在 selectSheet() 之后才可调用
    QString getSheetName();
    QString getSheetName(int sheetIndex);

    /**************************************************************************/
    /* 单元格                                                                 */
    /**************************************************************************/
    void setCellVariant(int row, int column, const QVariant& value);
    //cell 例如 "A7"
    void setCellVariant(const QString& cell, const QVariant &value);
    //range 例如 "A5:C7"
    void mergeCells(const QString& range);
    void mergeCells(int topLeftRow, int topLeftColumn, int bottomRightRow, int bottomRightColumn);
    QVariant getCellValue(int row, int column);
    QVariant getCellValue(const QString &cell);
    void clearCell(int row, int column);
    void clearCell(const QString& cell);

    /**************************************************************************/
    /* 布局格式                                                               */
    /**************************************************************************/
    void getUsedRange(int *topLeftRow, int *topLeftColumn, int *bottomRightRow, int *bottomRightColumn);
    void setColumnWidth(int column, int width);
    void setColumnWidth(QString column, int width);//add by atlantis
    void setRowHeight(int row, int height);
    void setCellTextCenter(int row, int column);
    void setCellTextCenter(const QString& cell);
    void setCellTextWrap(int row, int column, bool isWrap);
    void setCellTextWrap(const QString& cell, bool isWrap);
    void setAutoFitRow(int row);
    void setAutoFitColumn(int column);
    void mergeSerialSameCellsInAColumn(int column, int topRow);
    int getUsedRowsCount();
    void setCellFontBold(int row, int column, bool isBold);
    void setCellFontBold(const QString& cell, bool isBold);
    void setCellFontSize(int row, int column, int size);
    void setCellFontSize(const QString& cell, int size);

    /**************************************************************************/
    /* 文件                                                                   */
    /**************************************************************************/
    void save();
    void saveAs(const QString& fileName, const QString &fileFormat);

private:
    QAxObject * m_excel;
    QAxObject * m_workBooks;
    QAxObject * m_workBook;
    QAxObject * m_sheets;
    QAxObject * m_sheet;

    void close();

private slots:
    void onQExcelException(int code,const QString &source,const QString &desc,const QString &help);
};

#endif
