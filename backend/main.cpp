#include <QtCore>

#include "backend.h"

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);

	Backend backend(2748);

	return app.exec();
}