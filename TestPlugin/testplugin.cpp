#include "testplugin.h"
#include "ui_testplugin.h"

#include <qdebug.h>

#include <gdcmReader.h>
#include <itkGDCMImageIO.h>

#include <vtkStringArray.h>
#include <vtkDICOMImageReader.h>

TestPlugin::TestPlugin(QWidget *parent): 
	ui(new Ui::TestPlugin)
{
	ui->setupUi(this);
}

TestPlugin::~TestPlugin(){
	delete ui;
	qDebug() << "Plugin " << this->getPluginName() << " has been destroyed";
}

QString TestPlugin::getPluginName()
{
	return QString("Test widget");
}

void TestPlugin::setDataObject(dvaData* dataobj){
	this->imageData = dataobj;
}

dvaData* TestPlugin::getDataObject() {
	return this->imageData;
}

void TestPlugin::on_pushButton_clicked() {
	this->ui->textBrowser->append(QString("Click!"));
}

void TestPlugin::on_neurotoxin_emitted() {
}

void TestPlugin::on_imageData_updated() {
}