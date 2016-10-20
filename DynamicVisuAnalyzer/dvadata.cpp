#include "dvadata.h"



dvaData::dvaData(QObject *parent) : QObject(parent)
{
    this->reader = vtkSmartPointer<vtkDICOMImageReader>::New();

    //reader->SetDirectoryName("C:/DICOM_resources/MyHead");
    //reader->Update();
    //emit(this->DataUpdated());
}

dvaData::~dvaData()
{
    ;
}

void dvaData::SetStudyPath(const char *path){
	this->studyPath = path;
    this->reader->SetDirectoryName(path);
    this->reader->SetUpdateExtentToWholeExtent();
    this->reader->Update();
    //this->reader->UpdateWholeExtent();
    int extent[6];
    this->reader->GetDataExtent(extent);



    emit this->DataUpdated();
}

vtkDICOMImageReader *dvaData::GetReader()
{
	return this->reader;
}

QString dvaData::getStudyPath() {
	return this->studyPath;
}