#include "weighui.h"
#include "ui_weighui.h"
#include"mainwindow.h"
#include<QMessageBox>
#include<QDebug>
#include<QFileDialog>
#include<QDataStream>
#include"mytablemodel.h"
#include"QExcel.h"
#include<QDate>
#include<QInputDialog>
#include<dbmanager.h>
#include"openfiledialog.h"
#include<QRandomGenerator>
WeighUI::WeighUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WeighUI),
    m_isSaved(false),
    excelOpened(false)
{

    ui->setupUi(this);
    m_status=new QLabel("",this);
    this->statusBar()->addWidget(m_status);
    connect((MyTableModel*)ui->tableView->model(),&MyTableModel::checkMediumNum,this,[this](const QModelIndex &index,const QVariant&v){
        if(m_testItem!="称量介质准备"){
            QSqlQuery query;
            query.exec(QString("select * from records where mediumNum='%1' and sampleNum is null and mediumType=%2").arg(v.toString()).arg(ui->mediumTypeBox->currentIndex()));
            static bool nocheck=false;
            if(!nocheck&&!query.next()){
                nocheck=true;
                ui->tableView->setData(index.row(),index.column(),v.toString()+"*");
                nocheck=false;
                if(m_status) m_status->setText("警告：标*项目未找到初重结果！");
            }
        }
    });
//    connect(ui->lineEdit,&QLineEdit::editingFinished,this,[&](){
//        readData(("0.123"));
//    });
}

WeighUI::~WeighUI()
{

    delete ui;
}

void WeighUI::closeEvent(QCloseEvent *event)
{
    if(!m_isSaved){
        //save("./temp");
        QMessageBox::StandardButton bt=QMessageBox::question(this,"","当前任务未保存，是否现在保存？");
        if(bt==QMessageBox::Yes){
            on_saveBtn_clicked();
        }
    }

    ((MainWindow*)(this->parent()))->enable(true);
}

void WeighUI::reset()
{
    ui->tableView->clearAll();
    ui->sampleTypeLine->setText("");
    ui->mediumTypeBox->setCurrentIndex(-1);
    ui->tempratureLine->setText("");
    ui->humidityLine->setText("");
    m_status->setText("");
}

void WeighUI::setTestItem(const QString &item,const QString &userName)
{
    m_testItem=item;
    m_userName=userName;
    setWindowTitle("当前测试项目："+item);
    if(item=="水质悬浮物"){
        m_constLimit=0.0004;
        ui->mediumTypeBox->setCurrentText("水质SS");
    }
    else if(item=="海水悬浮物"){
        ui->mediumTypeBox->setCurrentText("海水SS");
        ui->sampleTypeLine->setText("海水");
    }
    else if(item=="环境空气PM10"||item=="环境空气PM2.5"){
        m_constLimit=0.00004;
        ui->mediumTypeBox->setCurrentText("颗粒物");
        ui->sampleTypeLine->setText("环境空气");
    }
    else {
        m_constLimit=0.0001;
        ui->mediumTypeBox->setCurrentText("颗粒物");
        ui->sampleTypeLine->setText("环境空气");
        if(m_testItem=="无组织颗粒物") ui->sampleTypeLine->setText("无组织废气");
    }
}

void WeighUI::readData(const QString &data)
{
    int t=ui->radioGroup->checkedId();
    if(t==-1){
        QMessageBox::information(this,"error","请选择当前测量步骤！");
        return;
    }
    int row=ui->tableView->currentRow();
    int column=2-t;
    qDebug()<<t<<column;

    //判定恒重情况
    double weight=data.toDouble();
    if(column==5||column==7){
        if(!m_constLimit) m_constLimit=0.0002;
        QVariant v=ui->tableView->data(row,column-1);
        if(!v.isValid()){//第一次没称
            double k=QRandomGenerator::global()->bounded(m_constLimit);
            k=round(k-m_constLimit/3,5);
            ui->tableView->setData(row,column-1,weight+k);
        }
        else{
            double x=ui->tableView->data(row,column-1).toDouble();
            if((weight-x)<-m_constLimit||(weight-x)>m_constLimit){
                QMessageBox::information(this,"error:","未达到恒重要求，请重新称量。");
                return;
            }
        }
    }

    if(!ui->tableView->data(row,column).isNull()){
        QMessageBox::StandardButton bt=QMessageBox::question(this,"","检测到当前编号已经有检测数据，是否覆盖？");
        if(bt==QMessageBox::Yes){
            ui->tableView->setData(row,column,data);
        }
    }
    else ui->tableView->setData(row,column,data);


    ui->tableView->selectRow(row+1);
    m_date=QDate::currentDate().toString("yyyy年M月dd日");
    m_endTime=QDateTime::currentDateTime();
    if(m_startTime.isNull()) m_startTime=m_endTime;
}

void WeighUI::print(bool selectedOnly)
{
    excelOpened=true;
    EXCEL.setScreenUpdating(false);
    int startData,endData;
    MyTableModel*model=(MyTableModel*)(ui->tableView->model());
    if(selectedOnly){
        startData=ui->tableView->selectedIndexes().first().row();
        endData=ui->tableView->selectedIndexes().last().row();
    }
    else{
        startData=0;
        endData=model->getUsedRow();
    }
    QAxObject *book=EXCEL.Open(QString("./%1.xlsm").arg(m_testItem));
    if(!book){
        QMessageBox::information(this,"error",QString("无法打开文件：%1.xlsm；错误：%2").arg(m_testItem).arg(EXCEL.LastError()));
        return;
    }
    QAxObject *sheet=book->querySubObject("sheets(int)",1);
    if(!sheet){
        QMessageBox::information(this,"error","查询表格失败：水质悬浮物.xlsm");
        return;
    }
    QAxObject* usedRange=sheet->querySubObject("UsedRange()");


    int startRow=0,page=0,sampleNumColumn=0,sampleAmountColumn=0,mediumNumColumn=0,mediumWeightColumn=0,timeThreeColumn=0,timeFourColumn=0;


    QAxObject* range= EXCEL.find(usedRange,"[样品编号]");
    if(range){
        sampleNumColumn=range->dynamicCall("column()").toInt();
        startRow=range->dynamicCall("row()").toInt();
        range->clear();
    }


    range= EXCEL.find(usedRange,"[取样量]");
    if(range){
        sampleAmountColumn=range->dynamicCall("column()").toInt();
        range->clear();
    }


    range= EXCEL.find(usedRange,"[滤膜编号]");
    if(range){
        mediumNumColumn=range->dynamicCall("column()").toInt();
        range->clear();
        if(!startRow) startRow=range->dynamicCall("row()").toInt();//介质称量的情况，在此初始化开始行
    }


    range= EXCEL.find(usedRange,"[滤膜初重]");
    if(range){
        mediumWeightColumn=range->dynamicCall("column()").toInt();
        range->clear();
    }


    range= EXCEL.find(usedRange,"[第三次]");
    if(range){
        timeThreeColumn=range->dynamicCall("column()").toInt();
        range->clear();
    }


    range= EXCEL.find(usedRange,"[第四次]");
    if(range){
        timeFourColumn=range->dynamicCall("column()").toInt();
        range->clear();
    }


    //填写温湿度
        EXCEL.replace(usedRange,"[温度]",ui->tempratureLine->text());
        EXCEL.replace(usedRange,"[湿度]",ui->humidityLine->text());
        EXCEL.replace(usedRange,"[样品类型]",ui->sampleTypeLine->text());
        EXCEL.replace(usedRange,"[分析日期]",m_date);
    //填入分析数据
    for(int i=startData;i<endData+1;i++){

        range=EXCEL.selectRow(startRow,sheet);
        range->dynamicCall("copy()");
        range->clear();
        range=EXCEL.selectRow(startRow+1,sheet);
        range->dynamicCall("insert()");
        range->clear();

        if(sampleNumColumn) EXCEL.writeCell(startRow,sampleNumColumn,sheet,model->data(i,0));
        if(sampleAmountColumn) EXCEL.writeCell(startRow,sampleAmountColumn,sheet,model->data(i,1));
        if(mediumNumColumn) EXCEL.writeCell(startRow,mediumNumColumn,sheet,model->data(i,2));
        //                EXCEL.writeCell(i+10,timeOneColumn,sheet,model->data(i,4));
        //                EXCEL.writeCell(i+10,timeTwoColumn,sheet,model->data(i,5));
        if(mediumWeightColumn){
            QSqlQuery query;
            query.exec(QString("select result1,result2 from records where mediumNum='%1' and sampleNum is null and mediumType =%2").arg(model->data(i,2).toString()).arg(ui->mediumTypeBox->currentIndex()));
            if(query.next()){
                double r1=query.value(0).toDouble();
                double r2=query.value(1).toDouble();
                if(query.value(1).isValid()) EXCEL.writeCell(startRow,mediumWeightColumn,sheet,round((r1+r2)/2,5));
                else EXCEL.writeCell(startRow,mediumWeightColumn,sheet,r1);//只有一次称量（海水SS）
                QSqlQuery query2;
                if(!query2.exec(QString("UPDATE records set mediumState=true WHERE mediumNum='%1' and sampleNum is null and mediumType =%2").arg(model->data(i,2).toString()).arg(ui->mediumTypeBox->currentIndex()))){
                    qDebug()<<"update data error"<<query2.lastError().text();
                }
            }
        }
        EXCEL.writeCell(startRow,timeThreeColumn,sheet,model->data(i,6));
        if(timeFourColumn) EXCEL.writeCell(startRow,timeFourColumn,sheet,model->data(i,7));
        startRow++;
    }
    range=EXCEL.selectRow(startRow,sheet);
    range->dynamicCall("delete()");
    page=EXCEL.pages(sheet);
    int page2=page;
    while(page2==page){
        range=EXCEL.selectRow(startRow-1,sheet);
        range->dynamicCall("copy()");
        range->clear();
        range=EXCEL.selectRow(startRow,sheet);
        range->dynamicCall("insert()");
        range->clear();
        range=EXCEL.selectRow(startRow,sheet);
        range->dynamicCall("ClearContents()");
        range->clear();
        startRow++;
        page2=EXCEL.pages(sheet);
        qDebug()<<"page2"<<page2;
    }
    range=EXCEL.selectRow(startRow-1,sheet);
    range->dynamicCall("delete()");
    //海水SS的空白滤膜，空白放在前两行
    if(m_testItem=="海水悬浮物"){
        double k,f1,f2,l1,l2;
        l1=ui->tableView->data(0,6).toDouble();
        l2=ui->tableView->data(1,6).toDouble();
        QSqlQuery q;
        q.exec(QString("select result1 from records where mediumNum='%1'").arg(ui->tableView->data(0,2).toString()));
        q.next();
        f1=q.value(0).toDouble();
        q.exec(QString("select result1 from records where mediumNum='%1'").arg(ui->tableView->data(1,2).toString()));
        q.next();
        f2=q.value(0).toDouble();
        k=(l1-f1+l2-f2)/2;
        EXCEL.replace(usedRange,"[空白滤膜校正值]",QString::number(k));
    }
//签名：
    range=EXCEL.find(usedRange,"[分析人]");
    range->dynamicCall("ClearContents()");
    QString file=QDir::currentPath()+"/"+m_userName+".png";
    file.replace("/","\\");
    //QMessageBox::information(this,"",file);
    EXCEL.insertPic(sheet,range,file);
    range->clear();
    //释放资源
    usedRange->clear();
    sheet->clear();
    book->clear();
    QString str=QString("操作完成，请填写天平使用记录。");
    if((endData-startData+1))str.append(QString("本次完成称量样品%1个，耗时%2秒，平均单个样品耗时%3秒。").arg(endData-startData+1).arg(m_endTime.toSecsSinceEpoch()-m_startTime.toSecsSinceEpoch()).arg((m_endTime.toSecsSinceEpoch()-m_startTime.toSecsSinceEpoch())/(endData-startData+1)));
    QMessageBox::information(this,"友情提醒",str);


    EXCEL.setScreenUpdating(true);
    EXCEL.DisplayAlerts(false);
}

//void WeighUI::save(const QString &fileName)
//{
//    QFile file(fileName);
//    if (!file.open(QIODevice::WriteOnly)) {
//        return;
//    }
//    QDataStream s(&file);
//    s<<m_testItem<<ui->lineEdit->text()<<ui->lineEdit_2->text();
//    MyTableModel*model=(MyTableModel*)(ui->tableView->model());
//    int r=model->getUsedRow();
//    s<<r;
//    for(int i=0;i<8;i++){
//        for(int j=0;j<r+1;j++){
//            s<<model->data(j,i);
//        }
//    }
//    s<<ui->sampleTypeLine->text();
//    file.close();

//}

void WeighUI::on_saveBtn_clicked()
{
    if(ui->sampleTypeLine->text().isEmpty()&&m_testItem!="称量介质准备"){
        QMessageBox::information(this,"error","样品类型不能为空");
        return;
    }
    if(ui->mediumTypeBox->currentText().isEmpty()){
        QMessageBox::information(this,"error","滤膜类型不能为空");
        return;
    }
    if(ui->tempratureLine->text().isEmpty()){
        QMessageBox::information(this,"error","环境温度不能为空");
        return;
    }
    if(ui->humidityLine->text().isEmpty()){
        QMessageBox::information(this,"error","环境湿度不能为空");
        return;
    }
    QSqlQuery query;
    //处理TSP、PM10、PM2.5的特殊情况
    if(ui->tableView->data(0,3).isValid()&&(m_testItem=="环境空气TSP"||m_testItem=="环境空气PM10"||m_testItem=="环境空气PM2.5")){
        //先处理滤膜
        QDate date=QDate::currentDate();
        date=date.addDays(-14);//称量滤膜的时间
        //检查是否冲突
        while(true){
            if(!query.exec(QString("select * from records where weightDate='%1' and sampleNum is null and mediumType=%2").arg(date.toString("yyyy年M月dd日")).arg(ui->mediumTypeBox->currentIndex()))){
                QMessageBox::information(this,"error","检查冲突时错误："+query.lastError().text());
                return;
            }
            if(!query.next()) break;
            else{
               date= date.addDays(-1);
            }
        }
        //滤膜编号
        QString mediumNum=date.toString("yyMMdd");
        int count=((MyTableModel*)ui->tableView->model())->getUsedRow();
        QStringList mediumNums;
        for(int i=0;i<count+5;i++){
            mediumNums.append(mediumNum+QString::number(i+1).rightJustified(2, '0'));
        }
        double tempratrue;
        int humidity;//称量滤膜时的条件;
        tempratrue =round(ui->tempratureLine->text().toDouble()+QRandomGenerator::global()->bounded(1.0)-0.5,1);
        humidity=ui->humidityLine->text().toInt()+QRandomGenerator::global()->bounded(-3,3);
        //保存记录
        foreach (auto medium, mediumNums) {
            //滤膜质量
            double r1,r2;
            r1=round(QRandomGenerator::global()->bounded(0.035)+0.4,5);
            double k=QRandomGenerator::global()->bounded(m_constLimit);
            k=round(k-m_constLimit/2,5);
            r2=r1+k;
            //保存滤膜称量信息
            query.prepare("INSERT INTO records (mediumNum, result1, result2, mediumType, temperature, humidity, operator, saveMark, "
                          "weightDate) VALUES (:mediumNum, :result1, :result2, :mediumType, :temperature, :humidity, :operator, :saveMark, "
                          ":weightDate)");
            query.bindValue(":mediumNum",medium);
            query.bindValue(":result1",r1);
            query.bindValue(":result2",r2);
            query.bindValue(":mediumType",ui->mediumTypeBox->currentIndex());
            query.bindValue(":temperature",tempratrue);
            query.bindValue(":humidity",humidity);
            query.bindValue(":operator",m_userName);
            query.bindValue(":saveMark",QString("颗粒物滤膜准备（%1至%2）").arg(mediumNums.first()).arg(mediumNums.last()));
            query.bindValue(":weightDate",date.toString("yyyy年M月dd日"));
            if(!query.exec()){
                QMessageBox::information(this,"error","保存滤膜信息时错误："+query.lastError().text());
                return;
            }
        }
        //开始处理称量结果
        for(int i=0;i<count+1;i++){
            double x=ui->tableView->data(i,3).toDouble();
            double V=ui->tableView->data(i,1).toDouble();
            int n=QRandomGenerator::global()->bounded(0,mediumNums.count());
            QString medium=mediumNums.at(n);
            mediumNums.removeAt(n);
            double r1;
            if(!query.exec(QString("select result1 from records where mediumNum='%1' and sampleNum is null and mediumType=%2").arg(medium).arg(ui->mediumTypeBox->currentIndex()))){
                QMessageBox::information(this,"error","获取滤膜初重时错误："+query.lastError().text());
                return;
            }
            query.next();
            r1=query.value(0).toDouble();
            double r3,r4;
            r3=round(x*V/1000000+r1,5);
            double k=QRandomGenerator::global()->bounded(m_constLimit);
            k=round(k-m_constLimit/2,5);
            r4=r3+k;
            ui->tableView->setData(i,2,medium);
            ui->tableView->setData(i,3,QVariant());
            ui->tableView->setData(i,6,r3);
            ui->tableView->setData(i,7,r4);

        }
        m_date=QDate::currentDate().toString("yyyy年M月dd日");
        on_saveBtn_clicked();
        return;
    }



    MyTableModel*model=(MyTableModel*)(ui->tableView->model());
    int r=model->getUsedRow();
    QString str;
    if(m_testItem=="称量介质准备"){
        str=QString("%1滤膜准备（%2至%3）").arg(ui->mediumTypeBox->currentText()).arg(ui->tableView->data(0,2).toString()).arg(ui->tableView->data(r,2).toString());
    }
    else{
        str=QString("%1（%2至%3）").arg(m_testItem).arg(ui->tableView->data(0,0).toString()).arg(ui->tableView->data(r,0).toString());
    }
    str=QInputDialog::getText(this,"保存文件","请输入文件名：",QLineEdit::Normal,str);
    if(str.isEmpty()) return;
    if(!query.exec(QString("select saveMark from records where saveMark='%1' and weightDate='%2'").arg(str).arg(m_date))){
        QMessageBox::information(this,"error",query.lastError().text());
        return;
    }
    if(query.next()){
        QMessageBox::information(this,"error","文件名已经存在");
        return;
    }
    //开始保存数据：

    for(int i=0;i<r+1;i++){        
        query.prepare("INSERT INTO records (sampleNum,sampleAmount,mediumNum,marks,result1,result2,result3,result4,"
                      "sampleType,mediumType,temperature,humidity,operator,saveMark,weightDate,startTime,endTime) VALUES ("
                      ":sampleNum,:sampleAmount,:mediumNum,:marks,:result1,:result2,:result3,:result4,"
                      ":sampleType,:mediumType,:temperature,:humidity,:operator,:saveMark,:weightDate,:startTime,:endTime)");
        query.bindValue(":sampleNum", ui->tableView->data(i,0).toString());
        query.bindValue(":sampleAmount", ui->tableView->data(i,1).toString());
        query.bindValue(":mediumNum", ui->tableView->data(i,2).toString());
        query.bindValue(":marks", ui->tableView->data(i,3).toString());
        query.bindValue(":result1", ui->tableView->data(i,4).toDouble());
        query.bindValue(":result2", ui->tableView->data(i,5).toDouble());
        query.bindValue(":result3", ui->tableView->data(i,6).toDouble());
        query.bindValue(":result4", ui->tableView->data(i,7).toDouble());
        query.bindValue(":sampleType", ui->sampleTypeLine->text());
        query.bindValue(":mediumType", ui->mediumTypeBox->currentIndex());
        query.bindValue(":temperature", ui->tempratureLine->text().toDouble());
        query.bindValue(":humidity", ui->humidityLine->text().toDouble());
        query.bindValue(":operator", m_userName);
        query.bindValue(":saveMark", str);
        query.bindValue(":weightDate", m_date);
        query.bindValue(":weightDate", m_date);
        query.bindValue(":startTime", m_startTime.toString(Qt::ISODate));
        query.bindValue(":endTime", m_endTime.toString(Qt::ISODate));
        if(!query.exec()){
            QMessageBox::information(this,"error",query.lastError().text());
            return;
        }
    }
    m_isSaved=true;


}


void WeighUI::on_openBtn_clicked()
{
    reset();
    QList<QVariant> data=OpenFileDialog::getSelectedData(m_userName);
    qDebug()<<data;
    if(!data.count()) return;
    QSqlQuery query;
//    if(!query.exec(QString("select * from records where saveMark='%1' and weightDate='%2'").arg(data.at(0).toString()).arg(data.at(1).toString()))){
    if(!query.exec(QString("select * from records where saveMark='%1'").arg(data.at(0).toString()))){
        qDebug()<<"select openfile error:"<<query.lastError().text();
        return;
    }

//    QSqlRecord rec = query.record();

//    qDebug() << "Number of columns: " << rec.count();

//    int nameCol = rec.indexOf("name"); // index of the field "name"
//    while (q.next())
//        qDebug() << q.value(nameCol).toString(); // output all names
    int r=0;
    while (query.next()){
        ui->tableView->setData(r,0,query.value(1).toString());
        ui->tableView->setData(r,1,query.value(2).toString());
        ui->tableView->setData(r,2,query.value(3).toString());
        ui->tableView->setData(r,3,query.value(5).toString());
        ui->tableView->setData(r,4,query.value(6).toString());
        ui->tableView->setData(r,5,query.value(7).toString());
        ui->tableView->setData(r,6,query.value(8).toString());
        ui->tableView->setData(r,7,query.value(9).toString());
        r++;
    }
    query.previous();
    ui->sampleTypeLine->setText(query.value(10).toString());
    ui->mediumTypeBox->setCurrentIndex(query.value(11).toInt());
    ui->tempratureLine->setText(query.value(12).toString());
    ui->humidityLine->setText(query.value(13).toString());
    m_date=query.value(16).toString();
    m_startTime=QDateTime::fromString(query.value(17).toString());
    m_endTime=QDateTime::fromString(query.value(18).toString());

//    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", QString("TEST Files (*.%1)").arg(m_userName));
//    if (!fileName.isEmpty()) {
//        m_isSaved=false;
//        QFile file(fileName);
//        if (!file.open(QIODevice::ReadOnly)) {
//            return;
//        }
//        QDataStream s(&file);
//        QString wd,sd,type;
//        s>>m_testItem>>wd>>sd;
//        ui->lineEdit->setText(wd);
//        ui->lineEdit_2->setText(sd);
//        MyTableModel*model=(MyTableModel*)(ui->tableView->model());
//        int r;
//        s>>r;
//        QVariant v;
//        for(int i=0;i<8;i++){
//            for(int j=0;j<r+1;j++){
//                s>>v;
//                model->setData(j,i,v);
//            }
//        }
//        s>>type;
//        ui->sampleTypeLine->setText(type);
//        file.close();
//    }
}


void WeighUI::on_printBtn_clicked()
{
    print();
}


void WeighUI::on_printPartBtn_clicked()
{
    print(true);
}

