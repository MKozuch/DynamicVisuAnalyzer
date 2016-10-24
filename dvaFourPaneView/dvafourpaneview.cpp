#include "dvafourpaneview.h"
#include "ui_dvafourpaneview.h"

#include <QMessageBox>
#include <QDebug>

#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageAlgorithm.h>
#include <vtkImageData.h>
#include <vtkResliceCursorLineRepresentation.h>
#include <vtkResliceCursorThickLineRepresentation.h>
#include <vtkCamera.h>
#include "vtkResliceCursorWidget.h"
#include "vtkResliceCursorActor.h"
#include "vtkResliceCursorPolyDataAlgorithm.h"
#include "vtkResliceCursor.h"
#include "vtkDICOMImageReader.h"
#include "vtkCellPicker.h"
#include "vtkProperty.h"
#include "vtkPlane.h"
#include "vtkImageData.h"
#include "vtkCommand.h"
#include "vtkPlaneSource.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkInteractorStyleImage.h"
#include "vtkImageSlabReslice.h"
#include "vtkBoundedPlanePointPlacer.h"
#include "vtkDistanceWidget.h"
#include "vtkDistanceRepresentation.h"
#include "vtkHandleRepresentation.h"
#include "vtkResliceImageViewerMeasurements.h"
#include "vtkDistanceRepresentation2D.h"
#include "vtkPointHandleRepresentation3D.h"
#include "vtkPointHandleRepresentation2D.h"

class vtkResliceCursorCallback : public vtkCommand
{
public: 
	static vtkResliceCursorCallback *New() {
		return new vtkResliceCursorCallback;
	}

	void Execute(vtkObject *caller, unsigned long eventId, void *callData) {
		if (eventId == vtkResliceCursorWidget::WindowLevelEvent ||
			eventId == vtkCommand::WindowLevelEvent ||
			eventId == vtkResliceCursorWidget::ResliceThicknessChangedEvent) {

			// render everything
			for (int i = 0; i < 3; ++i) this->resliceCursorWidgetArr[i]->Render();
			this->imagePlaneWidgetArr[0]->GetInteractor()->GetRenderWindow()->Render();
			return;
		}
		else {
			vtkImagePlaneWidget* imagePlaneWidget = dynamic_cast<vtkImagePlaneWidget*>(caller);
			if (imagePlaneWidget) {
				double *wl = static_cast<double*>(callData);
				
				if (imagePlaneWidget == imagePlaneWidgetArr[0]) {
					this->imagePlaneWidgetArr[1]->SetWindowLevel(wl[0], wl[1], 1);
					this->imagePlaneWidgetArr[2]->SetWindowLevel(wl[0], wl[1], 1);
				}
				else if(imagePlaneWidget == imagePlaneWidgetArr[1]) {
					this->imagePlaneWidgetArr[0]->SetWindowLevel(wl[0], wl[1], 1);
					this->imagePlaneWidgetArr[2]->SetWindowLevel(wl[0], wl[1], 1);
				}
				else if (imagePlaneWidget == imagePlaneWidgetArr[2]) {
					this->imagePlaneWidgetArr[0]->SetWindowLevel(wl[0], wl[1], 1);
					this->imagePlaneWidgetArr[1]->SetWindowLevel(wl[0], wl[1], 1);
				}
			}

			vtkResliceCursorWidget *resliceCursorWidget = dynamic_cast<vtkResliceCursorWidget*>(caller);
			if (resliceCursorWidget) {
				vtkResliceCursorLineRepresentation *cursorRepresentation = dynamic_cast<vtkResliceCursorLineRepresentation*>(resliceCursorWidget->GetRepresentation());
				cursorRepresentation->GetResliceCursorActor()->GetCursorAlgorithm();

				for (int i = 0; i < 3; i++) {
					vtkPlaneSource *planeSource =
						static_cast<vtkPlaneSource*>(this->imagePlaneWidgetArr[i]->GetPolyDataAlgorithm());
					planeSource->SetOrigin(this->resliceCursorWidgetArr[i]->GetResliceCursorRepresentation()->GetPlaneSource()->GetOrigin());
					planeSource->SetPoint1(this->resliceCursorWidgetArr[i]->GetResliceCursorRepresentation()->GetPlaneSource()->GetPoint1());
					planeSource->SetPoint2(this->resliceCursorWidgetArr[i]->GetResliceCursorRepresentation()->GetPlaneSource()->GetPoint2());
					this->imagePlaneWidgetArr[i]->UpdatePlacement();
				}
			}

			// render everything
			for (int i = 0; i < 3; ++i) this->resliceCursorWidgetArr[i]->Render();
			this->imagePlaneWidgetArr[0]->GetInteractor()->GetRenderWindow()->Render();
			return;
		}
	}

	vtkResliceCursorCallback() {};
	vtkResliceCursorWidget *resliceCursorWidgetArr[3];
	vtkImagePlaneWidget *imagePlaneWidgetArr[3];
};


dvaFourPaneView::dvaFourPaneView(QWidget *parent):
	ui(new Ui::dvaFourPaneView)
{
	ui->setupUi(this);

	this->ui->view1->setVisible(false);
	this->ui->view2->setVisible(false);
	this->ui->view3->setVisible(false);
	this->ui->view4->setVisible(false);
}

void dvaFourPaneView::setupViews() {
	int imageDims[3];
	this->imageData->GetReader()->GetOutput()->GetDimensions(imageDims);

	// make viewers
	for (int i = 0; i < 3; i++) this->resliceViewerArr[i] = vtkSmartPointer<vtkResliceImageViewer>::New();

	// attach rendering windows to viewers
	this->ui->view1->SetRenderWindow(resliceViewerArr[0]->GetRenderWindow());
	this->ui->view2->SetRenderWindow(resliceViewerArr[1]->GetRenderWindow());
	this->ui->view3->SetRenderWindow(resliceViewerArr[2]->GetRenderWindow());

	// attach interactors to viewers
	resliceViewerArr[0]->SetupInteractor(this->ui->view1->GetInteractor());
	resliceViewerArr[1]->SetupInteractor(this->ui->view2->GetInteractor());
	resliceViewerArr[2]->SetupInteractor(this->ui->view3->GetInteractor());

	// set input for viewers
	for (int i = 0; i < 3; i++) this->resliceViewerArr[i]->SetInputData(this->imageData->GetReader()->GetOutput());

	// share the same reslice cursor object between viewers
	for (int i = 0; i < 3; ++i) {
		vtkResliceCursorLineRepresentation *lineRepresentation =
			vtkResliceCursorLineRepresentation::SafeDownCast(
				resliceViewerArr[i]->GetResliceCursorWidget()->GetRepresentation());
		resliceViewerArr[i]->SetResliceCursor(resliceViewerArr[0]->GetResliceCursor());
		lineRepresentation->GetResliceCursorActor()->GetCursorAlgorithm()->SetReslicePlaneNormal(i);
	}

	// set proper orientation for slices
	for (int i=0; i<3; i++) {
		this->resliceViewerArr[i]->SetSliceOrientation(i);
		this->resliceViewerArr[i]->SetResliceModeToAxisAligned();
	}


	// setup 3d viewer

	auto cellPicker = vtkSmartPointer<vtkCellPicker>::New();
	cellPicker->SetTolerance(0.005);

	auto ipwProp = vtkSmartPointer<vtkProperty>::New();

	auto renderer = vtkSmartPointer<vtkRenderer>::New();
	this->ui->view4->GetRenderWindow()->AddRenderer(renderer);
	vtkRenderWindowInteractor *interactor = this->ui->view4->GetInteractor();

	for (int i=0; i<3; ++i) {
		planeWidgetArr[i] = vtkSmartPointer<vtkImagePlaneWidget>::New();
		planeWidgetArr[i]->SetInteractor(interactor);
		planeWidgetArr[i]->SetPicker(cellPicker);

		planeWidgetArr[i]->RestrictPlaneToVolumeOn();
		double color[3] = { 0, 0, 0 };
		color[i] = 1;
		planeWidgetArr[i]->GetPlaneProperty()->SetColor(color);
		
		color[0] /= 4;
		color[1] /= 4;
		color[2] /= 4;
		resliceViewerArr[i]->GetRenderer()->SetBackground(color);

		planeWidgetArr[i]->SetTexturePlaneProperty(ipwProp);
		planeWidgetArr[i]->TextureInterpolateOff();
		planeWidgetArr[i]->SetResliceInterpolateToLinear();
		planeWidgetArr[i]->SetInputConnection(imageData->GetReader()->GetOutputPort());
		planeWidgetArr[i]->SetPlaneOrientation(i);
		planeWidgetArr[i]->SetSliceIndex(imageDims[i] / 2);
		planeWidgetArr[i]->DisplayTextOff();
		planeWidgetArr[i]->SetDefaultRenderer(renderer);
		planeWidgetArr[i]->On();
		planeWidgetArr[i]->InteractionOn();
	}

	auto resliceCallback = vtkSmartPointer<vtkResliceCursorCallback>::New();

	for (int i = 0; i < 3; ++i) {
		resliceCallback->imagePlaneWidgetArr[i] = planeWidgetArr[i];
		resliceCallback->resliceCursorWidgetArr[i] = resliceViewerArr[i]->GetResliceCursorWidget();
		resliceViewerArr[i]->GetResliceCursorWidget()->AddObserver(vtkResliceCursorWidget::ResliceAxesChangedEvent, resliceCallback);
		resliceViewerArr[i]->GetResliceCursorWidget()->AddObserver(vtkResliceCursorWidget::WindowLevelEvent, resliceCallback);
		resliceViewerArr[i]->GetResliceCursorWidget()->AddObserver(vtkResliceCursorWidget::ResliceThicknessChangedEvent, resliceCallback);
		resliceViewerArr[i]->GetResliceCursorWidget()->AddObserver(vtkResliceCursorWidget::ResetCursorEvent, resliceCallback);
		resliceViewerArr[i]->GetInteractorStyle()->AddObserver(vtkCommand::WindowLevelEvent, resliceCallback);

		resliceViewerArr[i]->SetLookupTable(resliceViewerArr[0]->GetLookupTable());
		planeWidgetArr[i]->GetColorMap()->SetLookupTable(resliceViewerArr[0]->GetLookupTable());
		planeWidgetArr[i]->SetColorMap(resliceViewerArr[i]->GetResliceCursorWidget()->GetResliceCursorRepresentation()->GetColorMap());
	}

	this->ui->view1->setVisible(true);
	this->ui->view2->setVisible(true);
	this->ui->view3->setVisible(true);
	this->ui->view4->setVisible(true);
}

dvaFourPaneView::~dvaFourPaneView()
{
	delete ui;
}

QString dvaFourPaneView::getPluginName() {
	return this->pluginName;
}

void dvaFourPaneView::setDataObject(dvaData* imagedata) {
	this->imageData = imagedata;
	this->setupViews();

	QMap<QString, vtkSmartPointer<vtkImageData>> *datamap = this->imageData->preProcData;
	QMapIterator<QString, vtkSmartPointer<vtkImageData>> i(*datamap);
	while (i.hasNext()) {
		i.next();
		this->ui->inputDataBox->addItem(i.key());
	}
}

dvaData* dvaFourPaneView::getDataObject(){
	return this->imageData;
}

void dvaFourPaneView::on_imageData_updated() {
	double scalarrange[2];
	this->imageData->GetReader()->GetOutput()->GetScalarRange(scalarrange);

	for each(vtkResliceImageViewer *view in this->resliceViewerArr){
		int slicerange[2];
		view->GetSliceRange(slicerange);
		view->SetSlice((slicerange[1] - slicerange[0]) / 2);
		view->GetRenderer()->ResetCamera();
		view->SetColorWindow(scalarrange[1] - scalarrange[0]);
		view->SetColorLevel((scalarrange[0] + scalarrange[1]) / 2.0);
		view->GetRenderer()->ResetCamera();
		view->Render();
	}

}

void dvaFourPaneView::on_viewResetBtn_clicked() {
	int imagedims[3];
	this->imageData->GetReader()->GetOutput()->GetDimensions(imagedims);

	for (int i = 0; i < 3; ++i) {
		this->planeWidgetArr[i]->SetSliceIndex(imagedims[i] / 2);
		this->resliceViewerArr[i]->Reset();
	}

	for (int i = 0; i < 3; ++i) {
		vtkPlaneSource *planesource = static_cast<vtkPlaneSource*>(this->planeWidgetArr[i]->GetPolyDataAlgorithm());
		planesource->SetNormal(this->resliceViewerArr[0]->GetResliceCursor()->GetPlane(i)->GetNormal());
		planesource->SetCenter(this->resliceViewerArr[0]->GetResliceCursor()->GetPlane(i)->GetOrigin());
		this->planeWidgetArr[i]->UpdatePlacement();
	}

	this->renderEverything();
}

void dvaFourPaneView::renderEverything() {
	for (int i = 0; i < 3; ++i) this->resliceViewerArr[i]->Render();
	this->ui->view4->GetRenderWindow()->Render();
}

void dvaFourPaneView::on_inputDataBox_currentIndexChanged(int i) {
	QString datainkey = this->ui->inputDataBox->currentText();
	vtkImageData *datain = this->imageData->preProcData->value(datainkey);

	for (int i = 0; i < 3; i++) {
		this->resliceViewerArr[i]->SetInputData(datain);
		this->resliceViewerArr[i]->Render();
	}

}