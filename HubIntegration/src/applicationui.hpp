#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/Page>
#include <bb/cascades/QmlDocument>
#include <bb/data/JsonDataAccess>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include <bb/system/InvokeTargetReply>

using namespace bb::cascades;
using namespace bb::data;
using namespace bb::system;

namespace bb
{
    namespace cascades
    {
        class Application;
        class LocaleHandler;
    }
}

class QTranslator;

/*!
 * @brief Application object
 *
 *
 */

class ApplicationUI : public QObject
{
    Q_OBJECT
public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI() { }


	Q_SLOT void onInvoked(const bb::system::InvokeRequest& request);

	Q_SLOT void cardResizeRequested(const bb::system::CardResizeMessage& resizeMessage);
	Q_SLOT void closeCard();

private slots:
    void onSystemLanguageChanged();

private:
    QTranslator* _pTranslator;
    bb::cascades::LocaleHandler* _pLocaleHandler;
	QSettings _settings;
	InvokeManager* _invokeManager;
	QmlDocument *_qml;
	NavigationPane *_root;
	QmlDocument *_pageQml;
	Page* _page;
	bool _isCard;
	bb::cascades::Application *_app;
};

#endif /* ApplicationUI_HPP_ */
