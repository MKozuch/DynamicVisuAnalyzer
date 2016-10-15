#ifndef DVAEXAMPLEALGO1_H
#define DVAEXAMPLEALGO1_H

#include "dvaexamplealgo1_global.h"
#include "../DynamicVisuAnalyzer/dvaPluginInterface.h"

#include <vtkSmartPointer.h>
#include <vtkResliceImageViewer.h>

#include <QObject>
#include <QWidget>
#include <QString>

#include <itkImage.h>
#include <itkVTKImageToImageFilter.h>
#include <itkImageToVTKImageFilter.h>
#include <QuickView.h>

namespace Ui {
	class Form;
}

class DVAEXAMPLEALGO1_EXPORT dvaExampleAlgo1 : public dvaPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(dvaPluginInterface)
	Q_PLUGIN_METADATA(IID dvaPluginInterface_iid)

public:
	dvaExampleAlgo1(QWidget *parent=0);
	~dvaExampleAlgo1();

	QString getPluginName();
	void setDataObject(dvaData*);
	dvaData* getDataObject();

private:
	Ui::Form *ui;
	QString pluginName = QString("K-Means Segment");
	dvaData *imageData;

	vtkSmartPointer<vtkImageViewer2> resliceView;

	void setViewToMiddleSlice();

	typedef itk::Image<int, 2> ImageType;
	typedef itk::VTKImageToImageFilter<ImageType> VTKImageToImageType;
	VTKImageToImageType::Pointer vtkImageToImageFilter;

public slots:
	void on_sliceOrientComboBox_currentIndexChanged(int);
	void on_sliceSlider_valueChanged(int);
	void on_neurotoxin_emitted();
	void on_imageData_updated();
	void on_showOriginalBtn_clicked(bool);
};

#endif // DVAEXAMPLEALGO1_H
