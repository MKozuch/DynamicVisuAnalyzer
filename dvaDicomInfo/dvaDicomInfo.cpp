#include "dvaDicomInfo.h"
#include "ui_dvaDicomInfo.h"


#include "gdcmDict.h"
#include "gdcmDicts.h"
#include "gdcmAttribute.h"
#include "gdcmStringFilter.h"
#include "gdcmGlobal.h"

#include <QDir>
#include <QDebug>


dvaDicomInfo::dvaDicomInfo(QWidget *parent) :
	ui(new Ui::Form)
{
	ui->setupUi(this);

	QStringList tabHeader;
	this->ui->tableWidget->setColumnCount(3);
	tabHeader <<"Attribute code" << "Attribute name" << "Attribute value";
	this->ui->tableWidget->setHorizontalHeaderLabels(tabHeader);	
}

dvaDicomInfo::~dvaDicomInfo(){
	delete this->ui;
}

QString dvaDicomInfo::getPluginName() {
	return this->pluginName;
}

void dvaDicomInfo::setDataObject(dvaData* d) {
	this->imageData = d;
	this->on_imageData_updated();
}

dvaData* dvaDicomInfo::getDataObject() {
	return this->imageData;
}

void dvaDicomInfo::on_imageData_updated() {
	auto path = this->imageData->getStudyPath();
	QDir dir = QDir(path);
	dir.setNameFilters(QStringList() << "*.dcm");
	dir.setSorting(QDir::Name);
	QStringList filenames = dir.entryList();
	QString firstfile = dir.absoluteFilePath(filenames[0]);
	const char *filename_c = firstfile.toLatin1().data();
	//const char *filename_c = "F:\\DICOM_resources\\golonka\\E\\Z00";
	//const char *filename_c = "F:\\DICOM_resources\\BRAINIX\\T1-SE-extrp - 601\\IM-0001-0001.dcm";
	//const char *filename_c = "F:\\DICOM_resources\\1.3.12.2.1107.5.2.32.35376.201309051427478535825107.0.0.0\\0001.dcm";

	this->dcmReader.SetFileName(firstfile.toLatin1().data());

	if (!(this->dcmReader.Read())) {
		return;
	}

	gdcm::File &dcmFile = dcmReader.GetFile();
	gdcm::DataSet &dcmDataSet = dcmFile.GetDataSet();

	const gdcm::Global &gdcmglobal = gdcm::Global::GetInstance();
	const gdcm::Dicts &gdcmdicts = gdcmglobal.GetDicts();
	const gdcm::Dict &gdcmpubdict = gdcmdicts.GetPublicDict();

	gdcm::StringFilter gdcmstringfilt;
	gdcmstringfilt.SetFile(dcmFile);

	gdcm::Tag patnametag;
	gdcmpubdict.GetDictEntryByName("Patient's Name", patnametag);
	
	std::pair<std::string, std::string> namerecord = gdcmstringfilt.ToStringPair(patnametag);
	std::cout << "Attribute Name Checked: " << namerecord.first << std::endl;
	std::cout << "Attribute Value (string): " << namerecord.second << std::endl;

	for (gdcm::Dict::ConstIterator i = gdcmpubdict.Begin(); i != gdcmpubdict.End(); ++i) {
		std::pair<gdcm::Tag, gdcm::DictEntry> dentry = *i;
		std::pair<std::string, std::string> record = gdcmstringfilt.ToStringPair(dentry.first);

		if (record.second != "") {
			std::cout << "Attribute Name Checked: " << record.first << std::endl;
			std::cout << "Attribute Value (string): " << record.second << std::endl;

			int rowcount = this->ui->tableWidget->rowCount();
			this->ui->tableWidget->insertRow(rowcount);

			QString attrcode = QString::fromStdString(dentry.first.PrintAsPipeSeparatedString());
			QString attrname = QString::fromStdString(record.first);
			QString attrvalue = QString::fromStdString(record.second);

			this->ui->tableWidget->setItem(rowcount, 0, new QTableWidgetItem(attrcode));
			this->ui->tableWidget->setItem(rowcount, 1, new QTableWidgetItem(attrname));
			this->ui->tableWidget->setItem(rowcount, 2, new QTableWidgetItem(attrvalue));
		}


	}

}