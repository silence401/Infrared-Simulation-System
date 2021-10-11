#ifndef MAIN_
#define MAIN_

#include<iostream>
#include<QApplication>

#include "ui/main_window.h"

using namespace infrared;
int main(int argc, char** argv){
    QApplication app(argc, argv);
    OptionManager options;
    MainWindow main_window(options);
   // QMainWindow w;
    //w.show();
    main_window.show();
    return app.exec();
} 
#endif