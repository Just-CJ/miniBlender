#include "attributewidget.h"
#include "ui_attributewidget.h"

attributeWidget::attributeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::attributeWidget)
{
    ui->setupUi(this);
    button = new QPushButton(tr("Reshape"));
    mainLayout = new QVBoxLayout;

    noSelect = new QLabel(tr("未选中物体，请双击选中物体"));
    selectObj = new QLabel;
    mainLayout->addWidget(noSelect);
    mainLayout->addWidget(selectObj);

    topRLabel = new QLabel(tr("顶面半径"));
    topR = new QDoubleSpinBox;
    layout1 = new QHBoxLayout;
    layout1->addWidget(topRLabel);
    layout1->addWidget(topR);

    bottomRLabel = new QLabel(tr("底面半径"));
    botR = new QDoubleSpinBox;
    layout2 = new QHBoxLayout;
    layout2->addWidget(bottomRLabel);
    layout2->addWidget(botR);

    heightLabel = new QLabel(tr("高"));
    height = new QDoubleSpinBox;
    layout3 = new QHBoxLayout;
    layout3->addWidget(heightLabel);
    layout3->addWidget(height);

    faceNumLabel = new QLabel(tr("侧面数"));
    faceNum = new QDoubleSpinBox;
    layout4 = new QHBoxLayout;
    layout4->addWidget(faceNumLabel);
    layout4->addWidget(faceNum);

    RLabel = new QLabel(tr("半径"));
    R = new QDoubleSpinBox;
    layout5 = new QHBoxLayout;
    layout5->addWidget(RLabel);
    layout5->addWidget(R);

    densityLabel = new QLabel(tr("线密度"));
    density = new QDoubleSpinBox;
    layout6 = new QHBoxLayout;
    layout6->addWidget(densityLabel);
    layout6->addWidget(density);

    mainLayout->addLayout(layout1);
    mainLayout->addLayout(layout2);
    mainLayout->addLayout(layout3);
    mainLayout->addLayout(layout4);
    mainLayout->addLayout(layout5);
    mainLayout->addLayout(layout6);
    mainLayout->addWidget(button);
    setLayout(mainLayout);

    viewable(1, 0, 0, 0, 0, 0, 0, 0, 0);

    connect(button, SIGNAL(clicked()), this, SLOT(sendReshape()));
}

void attributeWidget::sendReshape()
{
    emit reshape();
}

void attributeWidget::viewable(bool noSel, bool selObj, bool btn, bool top, bool bot, bool h, bool facen, bool r, bool dens)
{
    if(noSel)
        noSelect->setVisible(true);
    else
        noSelect->setVisible(false);
    if(selObj)
        selectObj->setVisible(true);
    else
        selectObj->setVisible(false);
    if(btn)
        button->setVisible(true);
    else
        button->setVisible(false);
    if(top){
        topRLabel->setVisible(true);
        topR->setVisible(true);
    }
    else{
        topRLabel->setVisible(false);
        topR->setVisible(false);
    }
    if(bot){
        bottomRLabel->setVisible(true);
        botR->setVisible(true);
    }
    else{
        bottomRLabel->setVisible(false);
        botR->setVisible(false);
    }
    if(h){
        heightLabel->setVisible(true);
        height->setVisible(true);
    }
    else{
        heightLabel->setVisible(false);
        height->setVisible(false);
    }
    if(facen){
        faceNumLabel->setVisible(true);
        faceNum->setVisible(true);
    }
    else{
        faceNumLabel->setVisible(false);
        faceNum->setVisible(false);
    }
    if(r){
        RLabel->setVisible(true);
        R->setVisible(true);
    }
    else{
        RLabel->setVisible(false);
        R->setVisible(false);
    }
    if(dens){
        densityLabel->setVisible(true);
        density->setVisible(true);
    }
    else{
        densityLabel->setVisible(false);
        density->setVisible(false);
    }
}

attributeWidget::~attributeWidget()
{
    delete ui;
}
