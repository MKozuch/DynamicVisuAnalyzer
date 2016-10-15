#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include "testplugin_global.h"
#include "../DynamicVisuAnalyzer/dvaPluginInterface.h"

#include <QString>

namespace Ui {
	class TestPlugin;
}

class TESTPLUGIN_EXPORT TestPlugin : public dvaPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(dvaPluginInterface)
	Q_PLUGIN_METADATA(IID dvaPluginInterface_iid)

public:
	TestPlugin(QWidget *parent = 0);
	~TestPlugin();

	QString getPluginName();
	void setDataObject(dvaData*);
	dvaData* getDataObject();

private:
	Ui::TestPlugin *ui;
	dvaData* imageData;

public slots:
	void on_pushButton_clicked();
	void on_neurotoxin_emitted();
	void on_imageData_updated();

};

//Q_EXPORT_PLUGIN2(TestPlugin)

#endif // TESTPLUGIN_H
