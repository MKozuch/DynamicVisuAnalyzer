#ifndef DVADATA_H
#define DVADATA_H

#include <QObject>

#include <vtkDICOMImageReader.h>
#include <vtkSmartPointer.h>
#include <vtkImageAlgorithm.h>

class dvaData : public QObject
{
    Q_OBJECT

public:
    explicit dvaData(QObject *parent = 0);
    ~dvaData();

    vtkDICOMImageReader *GetReader();
	QString getStudyPath();

signals:
    void PathChanged(const char*);
    void DataUpdated();

public slots:
    void SetStudyPath(const char*);

private:
	vtkSmartPointer <vtkDICOMImageReader> reader;
	int* volumeOfInterest[6];
	QString studyPath;
};

#endif // DVADATA_H
