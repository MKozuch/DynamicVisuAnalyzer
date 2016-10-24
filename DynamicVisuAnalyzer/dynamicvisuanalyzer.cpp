#include "dynamicvisuanalyzer.h"
#include "ui_dynamicvisuanalyzer.h"

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

	QStringList startupPlugins;
	//startupPlugins << QString("\\dvaFourPaneView.dll");
	//startupPlugins << QString("\\dvaDicomInfo.dll");
	startupPlugins << QString("\\dvaGaussBlur.dll");
	startupPlugins << QString("\\dvaExampleAlgo1.dll");

	foreach(QString pluginName, startupPlugins) {
		try {
			this->loadPlugin((qApp->applicationDirPath() + pluginName));
		}
		catch (QString e) {
			QMessageBox::critical(this, "Error", QString("Error loading ") + pluginName + "\n" + e);
		}
	}
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

//void DynamicVisuAnalyzer::pluginLoadTest() {
//	QDir pluginsDir(qApp->applicationDirPath());
//	pluginsDir.setNameFilters(QStringList() << "*.dll");
//	for each (QString fileName in pluginsDir.entryList(QDir::Files)){
//		QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
//		QObject *plugin = pluginLoader.instance();
//		if (plugin) {
//			dvaPluginInterface *pluginInterface = qobject_cast<dvaPluginInterface*>(plugin);
//			if (pluginInterface) {
//				qDebug() << pluginInterface->getPluginName();
//				this->ui->tabWidget->addTab(pluginInterface, pluginInterface->getPluginName());
//				pluginInterface->setDataObject(this->imageData);
//				QObject::connect(this->ui->neurotoxinEmitter, SIGNAL(clicked()), pluginInterface, SLOT(on_neurotoxin_emitted()));
//				QObject::connect(this->imageData, SIGNAL(DataUpdated()), pluginInterface, SLOT(on_imageData_updated()));
//			}
//			else qDebug() << "fail";
//		}
//		else qDebug() << "fail loading plugin " << fileName << " - " << pluginLoader.errorString();
//	}
//}

void DynamicVisuAnalyzer::loadPlugin(QString pluginpath){
	QPluginLoader pluginLoader(pluginpath);
	QObject *plugin = pluginLoader.instance();
	if (plugin) {
		dvaPluginInterface *pluginInterface = qobject_cast<dvaPluginInterface*>(plugin);
		if (pluginInterface) {
			this->ui->tabWidget->addTab(pluginInterface, pluginInterface->getPluginName());
			pluginInterface->setDataObject(this->imageData);
			QObject::connect(this->imageData, SIGNAL(DataUpdated()), pluginInterface, SLOT(on_imageData_updated()));
		}
		else {
			throw("The file is not a valid DynamicVisuAnalyzer plugin");
		}
	}
	else {
		throw(pluginLoader.errorString());
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
		try {
			loadPlugin(path);
		}
		catch (QString e) {
			QMessageBox::critical(this, "Error", QString("Error loading ") + path + "\n" + e);
		}
	}		
}