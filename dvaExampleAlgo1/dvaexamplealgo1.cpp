#include "dvaexamplealgo1.h"
#include "ui_dvaexamplealgo1.h"

#include <QDebug>
#include <QAction>

#include <vtkImageAlgorithm.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkImageShiftScale.h>
#include <vtkImageReslice.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToColors.h>

#include <itkGradientMagnitudeRecursiveGaussianImageFilter.h>
#include <itkWatershedImageFilter.h>
#include <itkScalarToRGBPixelFunctor.h>
#include <itkUnaryFunctorImageFilter.h>
#include <itkGradientMagnitudeImageFilter.h>
#include <itkScalarImageKmeansImageFilter.h>
#include <itkDiscreteGaussianImageFilter.h>

#include <QuickView.h>

dvaExampleAlgo1::dvaExampleAlgo1(QWidget *parent):
	ui(new Ui::Form)
{
	ui->setupUi(this);
	currentSliceRaw = vtkSmartPointer<vtkImageData>::New();
	currentSliceSegmented = vtkSmartPointer<vtkImageData>::New();
	this->resliceView = vtkSmartPointer<vtkImageViewer2>::New();
	this->resliceView->SetRenderWindow(this->ui->qvtkWidget->GetRenderWindow());
	this->resliceView->SetupInteractor(this->ui->qvtkWidget->GetInteractor());
	this->ui->sliceOrientComboBox->setCurrentIndex(0);
}

dvaExampleAlgo1::~dvaExampleAlgo1()
{
	delete ui;
}

QString dvaExampleAlgo1::getPluginName() {
	return this->pluginName;
}

void dvaExampleAlgo1::setDataObject(dvaData* dataobj) {
	this->imageData = dataobj;
	this->imageData->GetReader()->Update();
	this->on_imageData_updated();

	this->resliceView->SetInputData(this->currentSliceSegmented);
	this->resliceView->GetRenderer()->ResetCamera();

	QMap<QString, vtkSmartPointer<vtkImageData>> *datamap = this->imageData->preProcData;
	QMapIterator<QString, vtkSmartPointer<vtkImageData>> i(*datamap);
	while(i.hasNext()){
		i.next();
		this->ui->inputDataBox->addItem(i.key());
	}
}

dvaData* dvaExampleAlgo1::getDataObject() {
	return this->imageData;
}

void dvaExampleAlgo1::on_sliceOrientComboBox_currentIndexChanged(int index) {
	QString datainkey = this->ui->inputDataBox->currentText();
	vtkImageData *datain = this->imageData->preProcData->value(datainkey);
	
	extractSlice(datain, index, this->ui->sliceSlider->value());
	if (!this->ui->showOriginalBtn->isChecked()) {
		this->recalcSegmentation(this->ui->groupsNumberSpinBox->value());
	}
	this->resliceView->GetRenderer()->ResetCamera();
	this->resliceView->Render();
}

void dvaExampleAlgo1::on_sliceSlider_valueChanged(int value) {
	if (!this->ui->showOriginalBtn->isChecked()) {
		this->recalcSegmentation(this->ui->groupsNumberSpinBox->value());
	}
	QString datainkey = this->ui->inputDataBox->currentText();
	vtkImageData *datain = this->imageData->preProcData->value(datainkey);
	extractSlice(datain, this->ui->sliceOrientComboBox->currentIndex(), value);
	this->resliceView->Render();
}

void dvaExampleAlgo1::on_imageData_updated() {
	this->imageData->GetReader()->Update();
	this->extractSlice(this->imageData->GetReader()->GetOutput(), 0, 0);
	this->ui->groupsNumberSpinBox->setValue(10);
	this->ui->sliceSlider->setValue(0);
	if (!this->ui->showOriginalBtn->isChecked()) {
		this->recalcSegmentation(this->ui->groupsNumberSpinBox->value());
		this->resliceView->SetInputData(this->currentSliceSegmented);
	}
	else {
		this->resliceView->SetInputData(this->currentSliceRaw);
	}

	this->resliceView->GetRenderer()->ResetCamera();
}

void dvaExampleAlgo1::on_showOriginalBtn_clicked(bool state) {
	if (state) this->resliceView->SetInputData(this->currentSliceRaw);
	else {
		this->recalcSegmentation(this->ui->groupsNumberSpinBox->value());
		this->resliceView->SetInputData(this->currentSliceSegmented);
	}
	this->resliceView->GetRenderer()->ResetCamera();
	this->resliceView->Render();
}

void dvaExampleAlgo1::extractSlice(vtkImageData* dataIn, int orientation, int slice){

	int dataextent[6];
	dataIn->GetExtent(dataextent);

	int imagedims[3];
	imagedims[0] = dataextent[1] - dataextent[0];
	imagedims[1] = dataextent[3] - dataextent[2];
	imagedims[2] = dataextent[5] - dataextent[4];

	auto reslicer = vtkSmartPointer<vtkImageReslice>::New();

	switch (orientation){
	case 0:
		reslicer->SetResliceAxesOrigin(0, 0, slice);
		reslicer->SetResliceAxesDirectionCosines(
			1, 0, 0, 
			0, 1, 0, 
			0, 0, 1);
		break;
	case 1:
		reslicer->SetResliceAxesOrigin(slice, 0, 0);
		reslicer->SetResliceAxesDirectionCosines(
			0, 1, 0, 
			0, 0, 1, 
			1, 0, 0);
		break;
	case 2:
		reslicer->SetResliceAxesOrigin(0, slice, 0);
		reslicer->SetResliceAxesDirectionCosines(
			1, 0, 0, 
			0, 0, 1, 
			0, -1, 0);
		break;
	default:
		reslicer->SetResliceAxesOrigin(slice, slice, slice);
		reslicer->SetResliceAxesDirectionCosines(
			1, 0, 0,
			0, 1, 0,
			0, 0, 1);
		break;
	}

	reslicer->SetOutputDimensionality(2);
	reslicer->SetInputData(dataIn);
	reslicer->SetOutputScalarType(VTK_INT);
	reslicer->Update();

	int extent[6];
	this->currentSliceRaw->GetExtent(extent);
	double range[2];
	this->currentSliceRaw->GetScalarRange(range);


	this->currentSliceRaw->DeepCopy(reslicer->GetOutput());	
}

void dvaExampleAlgo1::recalcSegmentation(int groupsNo) {

	// export vtk to itk
	typedef itk::Image<int, 2> ImageType;
	typedef itk::VTKImageToImageFilter<ImageType> VTKImageToImageType;
	VTKImageToImageType::Pointer vtkImageToImageFilter;
	vtkImageToImageFilter = VTKImageToImageType::New();
	vtkImageToImageFilter->SetInput(this->currentSliceRaw);
	//itk magic goes here

		//// gaussian blur
	//typedef itk::DiscreteGaussianImageFilter<ImageType, ImageType >  GaussianFilterType;
	//GaussianFilterType::Pointer gaussianfilter = GaussianFilterType::New();
	//gaussianfilter->SetVariance(1);
	//gaussianfilter->SetMaximumKernelWidth(5);
	//gaussianfilter->SetInput(vtkImageToImageFilter->GetOutput());

	// k-means classifier
	typedef itk::ScalarImageKmeansImageFilter< ImageType, ImageType > KMeansFilterType;
	KMeansFilterType::Pointer kmeansFilter = KMeansFilterType::New();
	kmeansFilter->SetInput(vtkImageToImageFilter->GetOutput());
	kmeansFilter->UseNonContiguousLabelsOn();
	int numberOfClasses = groupsNo;

	// initialize groups
	double scalarRange[2];
	this->currentSliceRaw->GetScalarRange(scalarRange);
	int range = scalarRange[1] - scalarRange[0];

	for (int i = 0; i < numberOfClasses; i++) {
		kmeansFilter->AddClassWithInitialMean(scalarRange[0] + i*(range / numberOfClasses));
	}

	typedef itk::Functor::ScalarToRGBPixelFunctor<int> ColorMapFunctorType;
	typedef KMeansFilterType::OutputImageType LabeledImageType;
	typedef itk::Image<itk::RGBPixel<unsigned char>, 2 > RGBImageType;
	typedef itk::UnaryFunctorImageFilter<LabeledImageType, RGBImageType, ColorMapFunctorType> ColorMapFilterType;
	ColorMapFilterType::Pointer colorMapFilter = ColorMapFilterType::New();
	colorMapFilter->GetFunctor().UseMSBForHashingOn();
	colorMapFilter->SetInput(kmeansFilter->GetOutput());

	// export from itk back to vtk
	typedef itk::ImageToVTKImageFilter<RGBImageType> ConnectorType;
	ConnectorType::Pointer connector = ConnectorType::New();
	connector->SetInput(colorMapFilter->GetOutput());
	connector->Update();

	this->currentSliceSegmented->DeepCopy(connector->GetOutput());
}

void dvaExampleAlgo1::on_groupsNumberSpinBox_valueChanged(int i) {
	if (!this->ui->showOriginalBtn->isChecked()) {
		this->recalcSegmentation(i);
		this->resliceView->GetRenderer()->ResetCamera();
	}
	this->resliceView->Render();
}

void dvaExampleAlgo1::on_inputDataBoc_valueChanged(int i) {
	
}

////// gradient filter
//typedef itk::GradientMagnitudeImageFilter<ImageType, InternalImageType> GradientMagnitudeFilterType;
//GradientMagnitudeFilterType::Pointer gradienMagnitudeFilter = GradientMagnitudeFilterType::New();
//gradienMagnitudeFilter->SetInput(vtkImageToImageFilter->GetOutput());
//
//// watershed algo 
//typedef itk::WatershedImageFilter<InternalImageType> WatershedFilterType;
//WatershedFilterType::Pointer watershedFilter = WatershedFilterType::New();
//watershedFilter->SetInput(gradienMagnitudeFilter->GetOutput());
//watershedFilter->SetThreshold(0.5);
//watershedFilter->SetLevel(0.01);
//
//// mapping LabeledImageType to RGB
//typedef itk::Functor::ScalarToRGBPixelFunctor<unsigned long> ColorMapFunctorType;
//typedef WatershedFilterType::OutputImageType LabeledImageType;
//typedef itk::UnaryFunctorImageFilter<LabeledImageType, RGBImageType, ColorMapFunctorType> ColorMapFilterType;
//ColorMapFilterType::Pointer colorMapFilter = ColorMapFilterType::New();
//colorMapFilter->SetInput(watershedFilter->GetOutput());

//// mapping LabeledImageType to RGB
//typedef itk::Functor::ScalarToRGBPixelFunctor<unsigned long> ColorMapFunctorType;
//typedef KMeansFilterType::OutputImageType LabeledImageType;
//typedef itk::Image<itk::RGBPixel<unsigned char>, 2 > RGBImageType;
//typedef itk::UnaryFunctorImageFilter<LabeledImageType, RGBImageType, ColorMapFunctorType> ColorMapFilterType;
//ColorMapFilterType::Pointer colorMapFilter = ColorMapFilterType::New();
//colorMapFilter->GetFunctor().UseMSBForHashingOn();
//colorMapFilter->SetInput(kmeansFilter->GetOutput());