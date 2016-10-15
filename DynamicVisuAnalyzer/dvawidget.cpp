#include "dvawidget.h"


dvaWidget::dvaWidget(QWidget *parent, vtkImageAlgorithm *inputalgo) : QWidget(parent)
{
    this->widgetName = "Base";
    this->inputAlgo = inputalgo;
    this->widgetInitialized = false;
}

QString dvaWidget::GetWidgetName(){
    return QString(this->widgetName);
}

void dvaWidget::SetInputAlgorithm(vtkImageAlgorithm *inputalgo)
{
    this->inputAlgo = inputalgo;
}

void dvaWidget::InitializeWidget(){
    this->widgetInitialized = true;
}

bool dvaWidget::IsInitialized(){
    return widgetInitialized;
}

void dvaWidget::DataUpdated(){
}
