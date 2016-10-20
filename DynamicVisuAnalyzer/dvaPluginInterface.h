#pragma once
 
#include <QString.h>
#include <QWidget>
#include "dvadata.h"

class dvaPluginInterface: public QWidget {
Q_OBJECT

public:
	virtual ~dvaPluginInterface() {}
	virtual QString getPluginName() = 0;
	virtual void setDataObject(dvaData*) = 0;
	virtual dvaData* getDataObject() = 0;

public slots:
virtual void on_neurotoxin_emitted() {};
	virtual void on_imageData_updated() {};
};

#define dvaPluginInterface_iid "dvaPluginInterface"
Q_DECLARE_INTERFACE(dvaPluginInterface, dvaPluginInterface_iid)