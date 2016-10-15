#include "dynamicvisuanalyzer.h"
#include <QApplication>

#include <QtGui>
#include <iostream>

int main(int argc, char *argv[])
{

	/*QLibrary helloLib("HelloLibrary.dll");
	if (!helloLib.load()) qDebug() << helloLib.errorString();
	if (helloLib.load()) qDebug() << "helloLib loaded";

	typedef void(*dllFun)();
	dllFun helloFun = (dllFun)helloLib.resolve("sayHello");
	if (helloFun) helloFun();
	else qDebug() << "Could not say hi from library";*/
    QApplication a(argc, argv);
    DynamicVisuAnalyzer w;
    w.show();
    return a.exec();
}
