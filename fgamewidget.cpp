#include "fgamewidget.h"
#include "ui_fgamewidget.h"

FGameWidget::FGameWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FGameWidget)
{
    ui->setupUi(this);
}

FGameWidget::~FGameWidget()
{    
    delete pix_Cover;
    delete ui;
}

void FGameWidget::setGame(FGame *g) {
    game = g;
    ui->fgwDialog_GameTitle->setText(game->getName());

    pix_Cover = new QPixmap("D:\\temp\\187-1.jpg");
    ui->fgwDialog_Cover->setPixmap( pix_Cover->scaled(90,110,Qt::KeepAspectRatioByExpanding));

    if(g->getType()==Steam) {
        pix_Type = new QPixmap(":/gfx/FGameType_Steam.png");
        ui->fgwDialog_TypeIcon->setPixmap( pix_Type->scaled(50,50,Qt::KeepAspectRatioByExpanding));
    } else if(g->getType()==Origin) {
        pix_Type = new QPixmap(":/gfx/FGameType_Origin.png");
        ui->fgwDialog_TypeIcon->setPixmap( pix_Type->scaled(50,50,Qt::KeepAspectRatioByExpanding));
    }

}


void FGameWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked(game, this);
        qDebug() << "Clicked";
    } else if (event->button() == Qt::RightButton)
    {
        emit rightClicked(game, this);
        qDebug() << "RightClicked";
    }
}

void FGameWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit doubleClicked(game, this);
    qDebug() << "doubleClicked";
}
