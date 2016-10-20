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
	const char *filename_c = firstfile.toStdString().c_str();

	this->dcmReader.SetFileName(filename_c);

	gdcm::File &dcmFile = dcmReader.GetFile();
	gdcm::DataSet &dcmDataSet = dcmFile.GetDataSet();

	const gdcm::Global &gdcmglobal = gdcm::Global::GetInstance();
	const gdcm::Dicts &gdcmdicts = gdcmglobal.GetDicts();
	const gdcm::Dict &gdcmpubdict = gdcmdicts.GetPublicDict();

	gdcm::StringFilter gdcmstringfilt;
	gdcmstringfilt.SetFile(dcmFile);

	gdcm::Tag patname;
	gdcmpubdict.GetDictEntryByName("Patient's Name", patname);
	
	std::pair<std::string, std::string> record = gdcmstringfilt.ToStringPair(patname);
	std::cout << "Attribute Name Checked: " << record.first << std::endl;
	std::cout << "Attribute Value (string): " << record.second << std::endl;


	for (gdcm::Dict::ConstIterator i = gdcmpubdict.Begin(); i != gdcmpubdict.End(); ++i) {
		std::pair<gdcm::Tag, gdcm::DictEntry> dentry = *i;
		std::pair<std::string, std::string> record = gdcmstringfilt.ToStringPair(dentry.first);



	}

}