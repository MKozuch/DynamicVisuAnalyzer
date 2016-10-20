#ifndef DVADICOMINFO_H
#define DVADICOMINFO_H

#include <QObject>
#include <QWidget>

#include "gdcmReader.h"

#include "../DynamicVisuAnalyzer/dvaPluginInterface.h"


namespace Ui {
	class Form;
}

class Q_DECL_EXPORT dvaDicomInfo : public dvaPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(dvaPluginInterface)
	Q_PLUGIN_METADATA(IID dvaPluginInterface_iid)

public:
	dvaDicomInfo(QWidget *parent = 0);
	~dvaDicomInfo();

	QString getPluginName();
	void setDataObject(dvaData*);
	dvaData* getDataObject();

private:
	Ui::Form *ui;
	QString pluginName = QString("Dicom info");
	dvaData *imageData;

	gdcm::Reader dcmReader;

public slots:
	void on_imageData_updated();
};

#endif // !DVADICOMINFO_H
