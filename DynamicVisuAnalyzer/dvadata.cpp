#include "dvadata.h"

#include <QDir>
#include <QDebug>

#include <vtkStringArray.h>

dvaData::dvaData(QObject *parent) : QObject(parent)
{
	this->reader = vtkSmartPointer<vtkGDCMImageReader>::New();
	this->reader->SetDataScalarTypeToInt();
	//this->reader->FileLowerLeftOn();

	this->preProcData = new QMap < QString, vtkSmartPointer<vtkImageData>>();
	this->preProcData->insert(QString("DICOM reader"), this->reader->GetOutput());
}

dvaData::~dvaData()
{
    delete this->preProcData;
}

void dvaData::SetStudyPath(const char *path){
	this->studyPath = path;
	
	auto dicomfilearr = vtkSmartPointer<vtkStringArray>::New();
	auto dcmdir = QDir(path);
	dcmdir.setNameFilters(QStringList() << "*.dcm");
	dcmdir.setSorting(QDir::Name);
	dcmdir.makeAbsolute();
	dicomfilearr->Allocate(dcmdir.count());

	for (unsigned int i = 0; i < dcmdir.count(); ++i) {
		dicomfilearr->InsertValue(i, dcmdir.absoluteFilePath(dcmdir[i]).toLatin1().data());
	}
	this->reader->SetFileNames(dicomfilearr);
    this->reader->SetUpdateExtentToWholeExtent();
    this->reader->Update();

    int extent[6];
    this->reader->GetDataExtent(extent);
	double range[2];
	this->reader->GetOutput()->GetScalarRange(range);

    emit this->DataUpdated();
}

vtkGDCMImageReader *dvaData::GetReader()
{
	return this->reader;
}

QString dvaData::getStudyPath() {
	return this->studyPath;
}
