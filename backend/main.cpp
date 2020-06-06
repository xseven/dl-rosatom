#include <QtCore>

#include "backend.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include <QDebug>

#if 0
namespace {

	void getjson() {
		QFile file;
		file.setFileName("ver4.json");
		file.open(QIODevice::ReadOnly | QIODevice::Text);
		auto data = file.readAll();
		file.close();

		auto doc = QJsonDocument::fromJson(data);

		const auto& entries = doc.array();

		for (const auto& entry : entries) {
			auto entryItem = entry.toObject();

			int id = entryItem.value("id").toInt();
			QString position = entryItem.value("Position").toString();
			QString path = entryItem.value("path").toString();

			qDebug() << id << " " << position << " " << path;
		}
	}
}
#endif

int main(int argc, char** argv)
{
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

	QCoreApplication app(argc, argv);	

	Backend backend(2748);

	return app.exec();
}