#ifndef DVAFOURPANEVIEW_H
#define DVAFOURPANEVIEW_H

#include "dvafourpaneview_global.h"
#include "../DynamicVisuAnalyzer/dvaPluginInterface.h"

#include <QObject>
#include <QDebug>

#include <vtkSmartPointer.h>
#include <vtkResliceImageViewer.h>
#include <vtkImageShiftScale.h>
#include <vtkImagePlaneWidget.h>

namespace Ui {
	class dvaFourPaneView;
}

class DVAFOURPANEVIEW_EXPORT dvaFourPaneView : public dvaPluginInterface
{
	Q_OBJECT
		Q_INTERFACES(dvaPluginInterface)
		Q_PLUGIN_METADATA(IID dvaPluginInterface_iid)

public:
	dvaFourPaneView(QWidget *parent = 0);
	~dvaFourPaneView();

	QString getPluginName();
	void setDataObject(dvaData*);
	dvaData* getDataObject();

private:
	Ui::dvaFourPaneView *ui;
	QString pluginName = QString("Four Pane Viewer 2");
	dvaData* imageData;

	void setupViews();
	void renderEverything();

protected:
	vtkSmartPointer<vtkResliceImageViewer> resliceViewerArr[3];
	vtkSmartPointer<vtkImagePlaneWidget> planeWidgetArr[3];
	//vtkSmartPointer<vtkDistanceWidget> distanceWidgetArr[3];
	vtkSmartPointer<vtkResliceImageViewerMeasurements> resliceMeasurements;

public slots:
	void on_neurotoxin_emitted();
	void on_imageData_updated();
	void on_viewResetBtn_clicked();

	//virtual void resliceMode(int);
	//virtual void thickMode(int);
	//virtual void SetBlendModeToMaxIP();
	//virtual void SetBlendModeToMinIP();
	//virtual void SetBlendModeToMeanIP();
	//virtual void SetBlendMode(int);
	//virtual void Render();
	//virtual void AddDistanceMeasurementToView1();
	//virtual void AddDistanceMeasurementToView(int);
};
#endif // DVAFOURPANEVIEW_H
