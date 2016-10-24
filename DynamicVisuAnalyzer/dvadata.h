#ifndef DVADATA_H
#define DVADATA_H

#include <QObject>
#include <QMap>

#include <vtkDICOMImageReader.h>
#include <vtkSmartPointer.h>
#include <vtkImageAlgorithm.h>

#include <vtkGDCMImageReader.h>

class dvaData : public QObject
{
    Q_OBJECT

public:
    explicit dvaData(QObject *parent = 0);
    ~dvaData();

	QMap<QString, vtkSmartPointer<vtkImageData>> *preProcData;
    vtkGDCMImageReader *GetReader();
	QString getStudyPath();


signals:
    void PathChanged(const char*);
    void DataUpdated();

public slots:
    void SetStudyPath(const char*);

private:
	vtkSmartPointer <vtkGDCMImageReader> reader;
	int* volumeOfInterest[6];
	QString studyPath;
};

#endif // DVADATA_H
