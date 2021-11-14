#include "ui/enviroment_set_widget.h"
namespace infrared{
    EnvSetWidget::EnvSetWidget(QWidget* parent):QWidget(parent){
        setWindowFlags(Qt::Window);
        setWindowTitle("参数设置");
        resize(120, parent->height());

        QGridLayout* grid = new QGridLayout(this);
        grid->setContentsMargins(5, 10, 5, 5);
        grid->addWidget(new QLabel(tr("时间")), 0, 0);

        QHBoxLayout* layout1 = new QHBoxLayout();
        linetext1_1 = new QLineEdit();
        linetext1_2 = new QLineEdit();
        linetext1_3 = new QLineEdit();
        linetext1_4 = new QLineEdit();
        layout1->addWidget(linetext1_1);
        layout1->addWidget(new QLabel(tr("年")));
        layout1->addWidget(linetext1_2);
        layout1->addWidget(new QLabel(tr("月")));
        layout1->addWidget(linetext1_3);
        layout1->addWidget(new QLabel(tr("日")));
        layout1->addWidget(linetext1_4);
        layout1->addWidget(new QLabel(tr("时"))); 
        grid->addLayout(layout1, 1, 0);

        grid->addWidget(new QLabel(tr("位置")), 2, 0);
        QHBoxLayout* layout2 = new QHBoxLayout();
        linetext2_1 = new QLineEdit();
        linetext2_2 = new QLineEdit();
        //layout2->addWidget(linetext1_1);
        layout2->addWidget(new QLabel(tr("纬度")));
        layout2->addWidget(linetext2_1);
        layout2->addWidget(new QLabel(tr("经度")));
        layout2->addWidget(linetext2_2);
        

        grid->addLayout(layout2, 3, 0);
        linetext3_1 = new QLineEdit();
        linetext3_2 = new QLineEdit();
        linetext3_3 = new QLineEdit();
        grid->addWidget(new QLabel(tr("环境参数")), 4, 0);
        QHBoxLayout* layout3 = new QHBoxLayout();
        layout3->addWidget(new QLabel(tr("最高温度：")));
        layout3->addWidget(linetext3_1);
        grid->addLayout(layout3, 5, 0);
        QHBoxLayout* layout4 = new QHBoxLayout();
        layout4->addWidget(new QLabel(tr("最低温度：")));
        layout4->addWidget(linetext3_2);
        grid->addLayout(layout4, 6, 0);

        QHBoxLayout* layout5 = new QHBoxLayout();

        layout5->addWidget(new QLabel(tr("大气透射率：")));

        //linetext3_1 = new QLineEdit();
        layout5->addWidget(linetext3_3);
        grid->addLayout(layout5, 7, 0);    

        grid->addWidget(new QLabel(tr("天气参数")), 8, 0);
        QHBoxLayout* layout6 = new QHBoxLayout();
        linetext4_1 = new QLineEdit();
        layout6->addWidget(new QLabel(tr("云遮系数")));
        layout6->addWidget(linetext4_1);
        grid->addLayout(layout6, 9, 0);

        qbutton_ = new QAction(QIcon(":media/project-edit.png"), tr("Apply"), this);
        connect(qbutton_, &QAction::triggered, this, &EnvSetWidget::Apply);
    



        //resize(120, parent->height());

        //grid->addWidget(new QLabel(tr("环境参数")), 4, 0)；
        

    }
    void EnvSetWidget::Apply(){
        int year = linetext1_1->text().toInt();
        int moon = linetext1_2->text().toInt();
        int day  = linetext1_3->text().toInt();
        int hour = linetext1_4->text().toInt();

        double lon = linetext2_1->text().toDouble();
        double lat = linetext2_2->text().toDouble();

        double maxT = linetext3_1->text().toDouble();
        double minT = linetext3_2->text().toDouble();

        double tran = linetext3_3->text().toDouble();
        double ccf = linetext4_1->text().toDouble();
        envopt_ = std::shared_ptr<EnviromentOptions>(new EnviromentOptions(
         year, moon, day, hour, lon, lat, maxT, minT, tran, ccf
        ));
        std::cout<<envopt_->year<<std::endl;






        
    }
}