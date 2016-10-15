#include "dvaexamplealgo1.h"
#include "ui_dvaexamplealgo1.h"

#include <QDebug>
#include <QAction>

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingOpenGL2);

#include <vtkImageAlgorithm.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkImageShiftScale.h>
#include <vtkImageReslice.h>

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

	double scalarRange[2];
	this->imageData->GetReader()->GetOutput()->GetScalarRange(scalarRange);

	int dataextent[6];
	this->imageData->GetReader()->GetDataExtent(dataextent);

	int imagedims[3];
	this->imageData->GetReader()->GetOutput()->GetDimensions();

	imagedims[0] = dataextent[1] - dataextent[0];
	imagedims[1] = dataextent[3] - dataextent[2];
	imagedims[2] = dataextent[5] - dataextent[4];

	auto reslicer = vtkSmartPointer<vtkImageReslice>::New();
	reslicer->SetOutputDimensionality(2);
	//reslicer->SetResliceAxesDirectionCosines(1, 0, 0, 0, 1, 0, 0, 0, 1);
	//reslicer->SetResliceAxesDirectionCosines(0, 1, 0, 0, 0, 1, 1, 0, 0);
	reslicer->SetResliceAxesDirectionCosines(1, 0, 0, 0, 0, 1, 0, -1, 0);
	reslicer->SetResliceAxesOrigin(imagedims[0]/2, imagedims[1] / 2, imagedims[2] / 2);
	reslicer->SetInputConnection(imageData->GetReader()->GetOutputPort());
	
	auto imgShiftScale = vtkSmartPointer<vtkImageShiftScale>::New();
	imgShiftScale->SetInputConnection(reslicer->GetOutputPort());
	imgShiftScale->SetOutputScalarTypeToInt();
	imgShiftScale->Update();

	// export vtk to itk
	vtkImageToImageFilter = VTKImageToImageType::New();
	vtkImageToImageFilter->SetInput(imgShiftScale->GetOutput());
	vtkImageToImageFilter->Update();

	//itk magic goes here
	typedef int InternalPixelType;
	typedef itk::Image< InternalPixelType, 2 >  InternalImageType;

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
	
	// gaussian blur
	typedef itk::DiscreteGaussianImageFilter<ImageType, ImageType >  GaussianFilterType;
	GaussianFilterType::Pointer gaussianfilter = GaussianFilterType::New();
	gaussianfilter->SetVariance(1);
	gaussianfilter->SetMaximumKernelWidth(5);
	gaussianfilter->SetInput(vtkImageToImageFilter->GetOutput());

	// k-means classifier
	typedef itk::ScalarImageKmeansImageFilter< ImageType, ImageType > KMeansFilterType;
	KMeansFilterType::Pointer kmeansFilter = KMeansFilterType::New();
	kmeansFilter->SetInput(gaussianfilter->GetOutput());
	kmeansFilter->UseNonContiguousLabelsOn();
	int numberOfClasses = 7;
	int range = scalarRange[1] - scalarRange[0];
	for (int i = 0; i < numberOfClasses; i++){
		kmeansFilter->AddClassWithInitialMean(scalarRange[0] + i*(range / numberOfClasses));}
	kmeansFilter->Update();

	// mapping LabeledImageType to RGB
	typedef itk::Functor::ScalarToRGBPixelFunctor<unsigned long> ColorMapFunctorType;
	typedef KMeansFilterType::OutputImageType LabeledImageType;
	typedef itk::Image<itk::RGBPixel<unsigned char>, 2 > RGBImageType;
	typedef itk::UnaryFunctorImageFilter<LabeledImageType, RGBImageType, ColorMapFunctorType> ColorMapFilterType;
	ColorMapFilterType::Pointer colorMapFilter = ColorMapFilterType::New();
	colorMapFilter->GetFunctor().UseMSBForHashingOn();
	colorMapFilter->SetInput(kmeansFilter->GetOutput()); 

	//QuickView qv;
	//qv.AddImage(colorMapFilter->GetOutput());
	//qv.Visualize();

	// export from itk back to vtk
	typedef itk::ImageToVTKImageFilter<RGBImageType> ConnectorType;
	ConnectorType::Pointer connector = ConnectorType::New();
	connector->SetInput(colorMapFilter->GetOutput());
	connector->Update();

	auto imageAfterConv = vtkSmartPointer <vtkImageData>::New();
	imageAfterConv->DeepCopy(connector->GetOutput());
	this->resliceView->SetInputData(imageAfterConv);
	this->resliceView->Render();
	
	//this->on_sliceOrientComboBox_currentIndexChanged(0);
}

dvaData* dvaExampleAlgo1::getDataObject() {
	return this->imageData;
}

void dvaExampleAlgo1::on_sliceOrientComboBox_currentIndexChanged(int index) {
	this->resliceView->SetSliceOrientation(index);

	int sliceRange[2];
	this->resliceView->GetSliceRange(sliceRange);
	this->ui->sliceSlider->setMinimum(sliceRange[0]);
	this->ui->sliceSlider->setMaximum(sliceRange[1]);
	this->ui->sliceSlider->setSliderPosition((sliceRange[1] - sliceRange[0]) / 2);
}

void dvaExampleAlgo1::on_sliceSlider_valueChanged(int value) {
	resliceView->SetSlice(value);
}

void dvaExampleAlgo1::on_neurotoxin_emitted() {}

void dvaExampleAlgo1::on_imageData_updated() {

	//for debug purpose- to be changed later
	this->setDataObject(this->imageData);

	if (this->ui->sliceOrientComboBox->currentIndex() == 0) {
		this->on_sliceOrientComboBox_currentIndexChanged(0);
	}
	else {
		this->ui->sliceOrientComboBox->setCurrentIndex(0);
	}
	this->resliceView->GetRenderer()->ResetCamera();
}

void dvaExampleAlgo1::setViewToMiddleSlice() {
	int sliceRange[2];
	this->resliceView->GetSliceRange(sliceRange);
}

void dvaExampleAlgo1::on_showOriginalBtn_clicked(bool state) {
	qDebug() << state;
}