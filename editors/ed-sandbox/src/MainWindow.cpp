#include "MainWindow.h"

#include "ui_MainWindow.h"

#include <QLabel>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create the dock manager. Because the parent parameter is a QMainWindow
    // the dock manager registers itself as the central widget.
	m_DockManager = new ads::CDockManager(this);

	// Create example content label - this can be any application specific
	// widget
	QLabel* l = new QLabel();
	l->setWordWrap(true);
	l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	l->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ");

	QLabel* l2 = new QLabel();
	l2->setWordWrap(true);
	l2->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	l2->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ");

	// Create a dock widget with the title Label 1 and set the created label
	// as the dock widget content
	ads::CDockWidget* DockWidget = new ads::CDockWidget("Label 1");
	DockWidget->setWidget(l);

	ads::CDockWidget* DockWidget2 = new ads::CDockWidget("Label 2");
	DockWidget2->setWidget(l2);

	// Add the toggleViewAction of the dock widget to the menu to give
	// the user the possibility to show the dock widget if it has been closed
	ui->menuView->addAction(DockWidget->toggleViewAction());
	ui->menuView->addAction(DockWidget2->toggleViewAction());

	// Add the dock widget to the top dock widget area
	m_DockManager->addDockWidget(ads::TopDockWidgetArea, DockWidget);
	m_DockManager->addDockWidget(ads::TopDockWidgetArea, DockWidget2);
}

MainWindow::~MainWindow()
{
    delete ui;
}
