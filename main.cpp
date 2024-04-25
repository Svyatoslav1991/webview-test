#include "mainwindow.h"

#include <QApplication>
#include <QWebEngineView>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QObject>
#include <QTabWidget>
#include <QPushButton>



BrowserTabWidget* createNewBrowserWindow(){
    auto *browserWindow = new BrowserTabWidget;
    auto *urlLineEdit = new QLineEdit;
    auto* webView = new QWebEngineView;
    auto *vbox = new QVBoxLayout(browserWindow);

    vbox->addWidget(urlLineEdit);
    vbox->addWidget(webView);

    QObject::connect(urlLineEdit, &QLineEdit::returnPressed, [webView, urlLineEdit](){
        auto urlToOpen = urlLineEdit->text();
        webView->load(QUrl(urlToOpen));
    });

    //установим название вкладки
    QObject::connect(webView, &QWebEngineView::loadFinished, [webView, browserWindow](bool isOk){
        if(isOk)
        {
            emit browserWindow->tabLoadingFinished(webView->title());
        }
    });

    urlLineEdit->setText("http://google.com");
    emit urlLineEdit->returnPressed();

    return browserWindow;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPushButton newTabButton("+");

    QTabWidget tabs;
    tabs.setTabsClosable(true);

    tabs.setCornerWidget(&newTabButton, Qt::BottomLeftCorner);

    QObject::connect(&newTabButton, &QPushButton::clicked, [&tabs](){
        auto newTab = createNewBrowserWindow();
        QObject::connect(newTab, &BrowserTabWidget::tabLoadingFinished, [&tabs](QString tabTitle){
            tabs.setTabText(tabs.currentIndex(), tabTitle);
            tabs.setWindowTitle(tabTitle);
        });

        tabs.addTab(newTab, "tab " + QString::number(tabs.count() + 1));
    });

    newTabButton.click();//добавляем сразу первое окно

    QObject::connect(&tabs, &QTabWidget::tabCloseRequested, [&tabs](int indexToRemove){
       if(tabs.count() > 1){
           auto widgetToDelete = tabs.widget(indexToRemove);
           delete widgetToDelete;
       }
    });

    QObject::connect(&tabs, &QTabWidget::currentChanged, [&tabs](int currentIndex){
       tabs.setWindowTitle(tabs.tabText(currentIndex));
    });

    tabs.showMaximized();

    return a.exec();
}
