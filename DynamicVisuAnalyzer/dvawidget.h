#ifndef DVAWIDGET_H
#define DVAWIDGET_H

#include <QWidget>
#include <QString>
#include <vtkImageAlgorithm.h>

class dvaWidget : public QWidget
{
    Q_OBJECT

public:    
    explicit dvaWidget(QWidget *parent = 0, vtkImageAlgorithm *inputalgo=NULL);
    QString GetWidgetName();
    void SetInputAlgorithm(vtkImageAlgorithm *);
    bool IsInitialized();
    virtual void InitializeWidget();

private:

protected:
    char *widgetName;
    vtkImageAlgorithm *inputAlgo;
    bool widgetInitialized;

signals:

public slots:
    virtual void DataUpdated();

};

#endif // DVAWIDGET_H
