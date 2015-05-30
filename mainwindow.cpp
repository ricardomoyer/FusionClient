#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <fgame.h>
#include <fdb.h>
#include "addgamedialog.h"
#include "fgamewidget.h"
#include "gameinfodialog.h"
#include "watchedfoldersdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if(!db.init())
    {
        return;
    }

    reloadStylesheet();
    game = new FGame();

    //Scan for New Games First
    crawler.scanAllFolders();

    gameScrollLayout = new QVBoxLayout;

    ui->gameScrollArea->widget()->setLayout(gameScrollLayout);
    ui->gameScrollArea->setStyleSheet("background: transparent");


    refreshList();
    //ui->gameIdBox->setMaximum(db.getGameCount());

  //  QString homeLocation = QStandardPaths::locate(QStandardPaths::AppDataLocation, QString(), QStandardPaths::LocateDirectory);
}

void MainWindow::reloadStylesheet()
{
    QString stylesheet = db.getTextPref("stylesheet");
    if(!stylesheet.isNull())
    {
        qDebug("Stylesheet set. Trying to load... ("+stylesheet.toLatin1()+")");
        QFile stylesheetFile(stylesheet);
        stylesheetFile.open(QFile::ReadOnly);
        currentStyle = QLatin1String(stylesheetFile.readAll());
        if(!currentStyle.isNull())
        {
            qDebug("Stylesheet loaded.");
            qApp->setStyleSheet(currentStyle);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_launchGameButton_clicked()
{
    if(gameList.isEmpty() || ui->gameListWidget->currentRow() == -1 || !ui->gameListWidget->currentItem()->isSelected())
    {
        return;
    }
    qDebug("Launching game!");
    game = new FGame(gameList.at(ui->gameListWidget->currentRow()));
    game->execute();
}

void MainWindow::resetDatabase()
{
    db.resetDatabase();
    refreshList();
}

void MainWindow::on_addGameButton_clicked()
{
    AddGameDialog* dialog = new AddGameDialog(this);
    connect(dialog, SIGNAL(gameSet(FGame)), this, SLOT(addGame(FGame)));
    dialog->exec();
}

void MainWindow::addGame(FGame game)
{
    //qDebug("Game added, YAY!");
    db.addGame(game);
    refreshList();
}

void MainWindow::on_addLibraryButton_clicked()
{
    WatchedFoldersDialog* dialog =  new WatchedFoldersDialog(this);
    connect(dialog, SIGNAL(folderSet(QList<QDir>)), this, SLOT(setWatchedFolders(QList<QDir>)));
    dialog->exec();
}


void MainWindow::setWatchedFolders(QList<QDir> folders)
{
    db.updateWatchedFolders(folders);
}

void MainWindow::refreshList()
{
    ui->gameListWidget->clear();
    gameList = db.getGameList();
    //FGame game = list.first();
    if(gameList.isEmpty())
    {
        ui->gameListWidget->addItem("NOTHING TO SEE HERE. Use the \"Add game\" button to add a new game.");
    }
    else
    {
        for(int i = 0; i < gameList.length(); i++)
        {
            ui->gameListWidget->addItem(gameList[i].getName());


                FGameWidget *gw = new FGameWidget(ui->gameScrollArea);
                gw->setGame(&gameList[i]);
                connect(gw, SIGNAL(clicked(FGame*, QObject*)), this, SLOT(onGameClick(FGame*, QObject*)));
                connect(gw, SIGNAL(doubleClicked(FGame*,QObject*)), this, SLOT(onGameDoubleClicked(FGame*, QObject*)));
                connect(gw, SIGNAL(rightClicked(FGame*,QObject*)), this, SLOT(onGameRightClicked(FGame*, QObject*)));

                gameScrollLayout->addWidget(gw);

        }
    }
}



void MainWindow::onGameRightClicked(FGame *game, QObject *sender)
{
    GameInfoDialog *dialog = new GameInfoDialog(*game);
    dialog->exec();
}

void MainWindow::onGameDoubleClicked(FGame *game, QObject *sender)
{
    game->execute();
}

void MainWindow::onGameClick(FGame *game, QObject *sender)
{
    if(qobject_cast<FGameWidget*>(sender)) {
       FGameWidget *widget = (FGameWidget*)sender;
       qDebug() << "is FGameWidget";
       this->setWindowTitle("FusionLauncher - " + game->getName());
    }
}

void MainWindow::on_removeGameButton_clicked()
{
    if(gameList.isEmpty() || ui->gameListWidget->currentRow() == -1 || !ui->gameListWidget->currentItem()->isSelected())
    {
        return;
    }
    db.removeGameById(gameList.at(ui->gameListWidget->currentRow()).dbId);
    refreshList();
}

void MainWindow::on_removeDatabaseAction_triggered()
{
    resetDatabase();
}

void MainWindow::on_refreshUIAction_triggered()
{
    refreshList();
}

void MainWindow::on_setStylesheetAction_triggered()
{
    QString stylesheetFile = QFileDialog::getOpenFileName(this, "Choose stylesheet", QDir::homePath(), "*.qss");
    if(QDir(stylesheetFile).exists())
    {
        if(db.getTextPref("stylesheet").isNull())
        {
            db.addTextPref("stylesheet", stylesheetFile);
        }
        else
        {
            db.updateTextPref("stylesheet", stylesheetFile);
        }
        reloadStylesheet();
    }
}

void MainWindow::on_libAddGameAction_triggered()
{
    on_addGameButton_clicked();
}

void MainWindow::on_libAddLibAction_triggered()
{
    on_addLibraryButton_clicked();
}

void MainWindow::on_actionGet_Info_triggered()
{
    if(gameList.isEmpty() || ui->gameListWidget->currentRow() == -1 || !ui->gameListWidget->currentItem()->isSelected())
    {
        return;
    }
    qDebug("Launching game!");
    game = new FGame(gameList.at(ui->gameListWidget->currentRow()));

    GameInfoDialog *dialog = new GameInfoDialog(*game);
    dialog->exec();
}

