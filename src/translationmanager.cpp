#include <QtCore/QCoreApplication>
#include <QtCore/QTranslator>

#include "translationmanager.h"

TranslationManager* TranslationManager::pInstance = 0;

TranslationManager* TranslationManager::instance()
{
	if(!TranslationManager::pInstance)
		pInstance = new TranslationManager();

	return pInstance;
}

TranslationManager::TranslationManager()
{
}

TranslationManager::~TranslationManager()
{
	if(pInstance == this)
		pInstance = 0;
}

void TranslationManager::addTranslation(const QString& fileName)
{
	if (fileName.isEmpty())
		return;

	QTranslator *t = new QTranslator();
	t->load(fileName);
	translatoinsList.insert(fileName, t);
	QCoreApplication::installTranslator(t);
}

void TranslationManager::removeTranslator(const QString& fileName)
{
	QTranslator *t = translatoinsList.value(fileName);
	if (t) {
		QCoreApplication::removeTranslator(t);
		translatoinsList.remove(fileName);
	}
}
