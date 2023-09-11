#pragma once

#include <QWidget>
#include "ui_AxisWidget.h"
#include <QTimer>
#include "LTDMC.h"
#include <QString>

class AxisWidget : public QWidget
{
	Q_OBJECT

public:
	AxisWidget(QWidget *parent, int axis);
	~AxisWidget();

private:
	Ui::AxisWidgetClass ui;
	QTimer* timer;
	WORD m_cardID = 0;
	short result;
	int m_axis;
	DWORD status;
	short m_sevon, m_erc;
	QString binaryString;

private:
	void initStatus();

public slots:
	void on_cBox_alm_currentIndexChanged(int index);	// 报警
	void on_cBox_el_currentIndexChanged(int index);		// 正负限位
	void on_cBox_org_currentIndexChanged(int index);	// 原点
	void on_equivBtn_clicked();	// 脉冲当量
	void on_goHomeBtn_clicked();	// 回原点
	void on_startBtn_clicked();	// 启动(移动)
	void on_lineCmpBtn_clicked();	// 启动(位置触发)
	void on_moveLMaxBtn_clicked();	// <<
	void on_moveLMinBtn_clicked();	// <
	void on_moveRMinBtn_clicked();	// >>
	void on_moveRMaxBtn_clicked();	// >
	void on_stopBtn_clicked();		// 强制停止 
	void on_resetBtn_clicked();	// 寄存器清零
	void on_sevonBtn_clicked();	// 驱动器使能
	void on_modeBanBtn_clicked(); // 高速比较模式设为禁止
	void recvError(QString error);
};
