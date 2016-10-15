#include "dynamicvisuanalyzer.h"
#include "ui_dynamicvisuanalyzer.h"

//#include "Widgets/dvafourpaneview.h"
//#include "Widgets/dvawidget.h"
//#include "Widgets/skullwidget.h"
//#include "Widgets/dvaprogrammablewidget.h"
#include "dvaPluginInterface.h"

#include <QPointer>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QPluginLoader>
#include <QStringList>

DynamicVisuAnalyzer::DynamicVisuAnalyzer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DynamicVisuAnalyzer)
{
    this->ui->setupUi(this);
    this->imageData = new dvaData(this);
	

	//this->imageData->SetStudyPath("F:\\DICOM_resources\\VHM_Head");
	this->imageData->SetStudyPath("F:\\DICOM_resources\\1.3.12.2.1107.5.2.32.35376.201309051427478535825107.0.0.0");
	this->pluginLoad((qApp->applicationDirPath() + QString("\\dvaFourPaneView.dll")));
	this->pluginLoad((qApp->applicationDirPath() + QString("\\dvaExampleAlgo1.dll")));
	this->pluginLoad((qApp->applicationDirPath() + QString("\\TestPlugin.dll")));
	


	//this->pluginLoadTest();

  /*  QPointer<dvaWidget> viewer = new dvaFourPaneView(this, imageData->GetReader());
    QPointer<dvaWidget> prog = new  dvaProgrammableWidget(this, imageData->GetReader());

    this->availableWidgets.append(viewer);
    this->availableWidgets.append(prog);

    dvaWidget *item;
    foreach(item, availableWidgets){
        QObject::connect(this->imageData, SIGNAL(DataUpdated()), item, SLOT(DataUpdated()));
        this->hide();
    }

    this->activeWidgetsList.append(viewer);
    ui->tabWidget->addTab(viewer, viewer->GetWidgetName());

    QObject::connect(this->imageData, SIGNAL(DataUpdated()), viewer, SLOT(DataUpdated()));*/
}

DynamicVisuAnalyzer::~DynamicVisuAnalyzer()
{
    delete ui;
	qDebug() << "DynamicVisuAnalyzer destroyed"; 
    delete imageData;
}

void DynamicVisuAnalyzer::on_actionOpen_triggered()
{
    //TODO: zweryfikj czy folder zawiera pliki dicom
    QFileDialog fileDialog;
    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setOption(QFileDialog::ShowDirsOnly);
    fileDialog.setViewMode(QFileDialog::List);
    fileDialog.setDirectory(QString("F:\\DICOM_resources"));
    int dialogExitCode = fileDialog.exec();
    if(dialogExitCode){
        QString path = fileDialog.selectedFiles()[0];
        this->imageData->SetStudyPath(path.toUtf8().constData());
    }
}

void DynamicVisuAnalyzer::on_actionAbout_triggered()
{
    QString aboutMsg = QString("DICOM DynamicVisuAnalyzer \n by: MKozuch");
    QMessageBox aboutDialog;
    aboutDialog.setText(aboutMsg);
    aboutDialog.setIcon(QMessageBox::Information);
    aboutDialog.exec();
}

void DynamicVisuAnalyzer::on_actionQuit_triggered()
{
    QApplication::quit();
}

void DynamicVisuAnalyzer::on_tabWidget_tabCloseRequested(int index) {
	QWidget* selectedWidget = this->ui->tabWidget->widget(index);
	dvaPluginInterface* widgetCasted = qobject_cast<dvaPluginInterface*>(selectedWidget);

	QObject* rodzic = selectedWidget->parent();
	qDebug() << rodzic->objectName();

	if (!widgetCasted) qDebug() << "Something went wrong while closing tab";
	else {
		qDebug() << "Widget " << widgetCasted->getPluginName() << " in tab number " << index << " close requesed";
		this->ui->tabWidget->removeTab(index);
		delete selectedWidget;
	}

}

void DynamicVisuAnalyzer::pluginLoadTest() {
	QDir pluginsDir(qApp->applicationDirPath());
	pluginsDir.setNameFilters(QStringList() << "*.dll");
	for each (QString fileName in pluginsDir.entryList(QDir::Files)){
		QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
		QObject *plugin = pluginLoader.instance();
		if (plugin) {
			dvaPluginInterface *pluginInterface = qobject_cast<dvaPluginInterface*>(plugin);
			if (pluginInterface) {
				qDebug() << pluginInterface->getPluginName();
				this->ui->tabWidget->addTab(pluginInterface, pluginInterface->getPluginName());
				pluginInterface->setDataObject(this->imageData);
				QObject::connect(this->ui->neurotoxinEmitter, SIGNAL(clicked()), pluginInterface, SLOT(on_neurotoxin_emitted()));
				QObject::connect(this->imageData, SIGNAL(DataUpdated()), pluginInterface, SLOT(on_imageData_updated()));
			}
			else qDebug() << "fail";
		}
		else qDebug() << "fail loading plugin " << fileName << " - " << pluginLoader.errorString();
	}
}

void DynamicVisuAnalyzer::pluginLoad(QString pluginpath) {
	// TODO: przesunac obsluge zdarzenia po stronie gui do on_actionLoad_plug_in_triggered()

	QPluginLoader pluginLoader(pluginpath);
	QObject *plugin = pluginLoader.instance();
	if (plugin) {
		dvaPluginInterface *pluginInterface = qobject_cast<dvaPluginInterface*>(plugin);
		if (pluginInterface) {
			this->ui->tabWidget->addTab(pluginInterface, pluginInterface->getPluginName());
			pluginInterface->setDataObject(this->imageData);
			QObject::connect(this->imageData, SIGNAL(DataUpdated()), pluginInterface, SLOT(on_imageData_updated()));
			//QObject::connect(this->ui->neurotoxinEmitter, SIGNAL(clicked()), pluginInterface, SLOT(on_neurotoxin_emitted()));
			//QMessageBox::information(this, "Success", QString("Plugin") + pluginInterface->getPluginName() + "loaded successfully");
		}
		else {
			qDebug() << "fail";
			QMessageBox::critical(this, "Error", QString("File ") + pluginpath + " is not valid");
		}
	}
	else {
		qDebug() << "Fail loading plugin " << pluginpath << " - " << pluginLoader.errorString();
		QMessageBox::critical(this, "Error", QString("File ") + pluginpath + " is not a DynamicVisuAnalyzer plugin");
	}
}

void DynamicVisuAnalyzer::on_actionLoad_plug_in_triggered() {
	QFileDialog fileDialog;
	fileDialog.setFileMode(QFileDialog::ExistingFile);
	fileDialog.setViewMode(QFileDialog::List);
	fileDialog.setNameFilter("Plugins(*.dll)");
	fileDialog.setDirectory(qApp->applicationDirPath());
	int dialogExitCode = fileDialog.exec();
	if (dialogExitCode) {
		QString path = fileDialog.selectedFiles()[0];
		this->pluginLoad(path);
	}		
}