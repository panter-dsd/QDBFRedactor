#ifndef TRANSLATIONMANAGER_H
#define TRANSLATIONMANAGER_H

class QTranslator;

#include <QtCore/QMap>

class TranslationManager
{

public:
	static TranslationManager* instance();
	virtual ~TranslationManager();

	void addTranslation(const QString& fileName);
	QList<QString> translations()
	{return translatoinsList.keys();}
	void removeTranslator(const QString& fileName);

protected:
	explicit TranslationManager();

private:
	static TranslationManager* pInstance;
	QMap<QString, QTranslator*> translatoinsList;
};

#endif // TRANSLATIONMANAGER_H
