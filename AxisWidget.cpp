#include "AxisWidget.h"
#include <QDebug>
#include <Windows.h>

AxisWidget::AxisWidget(QWidget* parent, int axis)
	: QWidget(parent = nullptr), m_axis(axis)
{
	ui.setupUi(this);
	timer = new QTimer(this);
	timer->start(100);
	connect(timer, &QTimer::timeout, this, [=]() {
		double pos;
		result = dmc_get_position_unit(m_cardID, m_axis, &pos);
		if (result != 0)
			ui.resultEdit->appendPlainText("dmc_get_position_unit  return:" + QString::number(result));
		ui.positionUnit->setText(QString::number(pos));
		result = dmc_get_encoder_unit(m_cardID, m_axis, &pos);
		if (result != 0)
			ui.resultEdit->appendPlainText("dmc_get_encoder_unit  return:" + QString::number(result));
		ui.encoderUnit->setText(QString::number(pos));
		initStatus();
	});
	
}

AxisWidget::~AxisWidget()
{
	timer->stop();
}


void AxisWidget::initStatus(){
	status = dmc_axis_io_status(m_cardID, m_axis);
	binaryString = QString::number(status, 2).rightJustified(12, '0');
	QString str;
	for (int i = binaryString.length() - 1; i >= 0; --i) {
		str += binaryString[i];
	}
	if (str[0] == '0')	// alm
		ui.cBox_alm->setCurrentIndex(0);
	else
		ui.cBox_alm->setCurrentIndex(1);
	if (str[1] == '0' && str[2] == '0')	// el
		ui.cBox_el->setCurrentIndex(0);
	else if (str[1] == '1' && str[2] == '1')
		ui.cBox_el->setCurrentIndex(1);
	else if (str[1] == '0' && str[2] == '1')
		ui.cBox_el->setCurrentIndex(2);
	else if (str[1] == '1' && str[2] == '0')
		ui.cBox_el->setCurrentIndex(3);
	if (str[4] == '0')		// org
		ui.cBox_org->setCurrentIndex(0);
	else
		ui.cBox_org->setCurrentIndex(1);
	m_sevon = dmc_read_sevon_pin(m_cardID, m_axis);
	if (m_sevon == 0)
		ui.sevonBtn->setText(QString::fromLocal8Bit("关"));
	else
		ui.sevonBtn->setText(QString::fromLocal8Bit("开"));
	m_erc = dmc_read_erc_pin(m_cardID, m_axis);

}
void AxisWidget::on_cBox_alm_currentIndexChanged(int index) {
	result = dmc_set_alm_mode(m_cardID, m_axis, 1, index, 0);
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_set_alm_mode()  return:" + QString::number(result));
	}
}
void AxisWidget::on_cBox_el_currentIndexChanged(int index) {
	result = dmc_set_el_mode(m_cardID, m_axis, 1, index, 0);
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_set_el_mode()  return:" + QString::number(result));
	}
}
void AxisWidget::on_cBox_org_currentIndexChanged(int index) {
	result = dmc_set_home_pin_logic(m_cardID, m_axis, index, 0);  // 设置原点信号  3:0=低电平 1=高电平   4:0=保留参数 固定值
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_set_home_pin_logic()  return:" + QString::number(result));
	}
}

void AxisWidget::on_equivBtn_clicked() {
	result = dmc_set_equiv(m_cardID, m_axis, ui.pauseUnit->text().toDouble());
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_set_equiv()  return:" + QString::number(result));
	}
}
void AxisWidget::on_goHomeBtn_clicked() {
	result = dmc_set_homemode(m_cardID, m_axis, ui.h_dir->currentIndex(), 0, 2, 0);  // 设置回原点方式  3(回零方向)：0=负向 1=正向   4(回零速度模式)：0=低速回零 1=高速回零  5(回零模式):二次回零 6保留参数 固定值为0
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_set_homemode()  return:" + QString::number(result));
	}
	result = dmc_set_profile_unit(m_cardID, m_axis, ui.h_minV->text().toDouble(), ui.h_maxV->text().toDouble(), ui.h_accT->text().toDouble(), ui.h_dccT->text().toDouble(), ui.h_stopV->text().toDouble());
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_set_profile_unit()  return:" + QString::number(result));
	}
	result = dmc_home_move(m_cardID, m_axis);    // 执行回零运动
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_home_move()  return:" + QString::number(result));
	}
	//while (dmc_check_done(m_cardID, 0) == 0)	continue;	
	WORD statex;
	result = dmc_get_home_result(m_cardID, m_axis, &statex);
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_get_home_result()  return:" + QString::number(result));
	}
	if (statex == 1) {
		result = dmc_set_position_unit(m_cardID, m_axis, 0);
		if (result != 0) {
			ui.resultEdit->appendPlainText("dmc_set_position_unit()  return:" + QString::number(result));
		}
		result = dmc_set_encoder_unit(m_cardID, m_axis, 0);
		if (result != 0) {
			ui.resultEdit->appendPlainText("dmc_set_encoder_unit()  return:" + QString::number(result));
		}
	}
}
void AxisWidget::on_startBtn_clicked() {
	result = dmc_set_profile_unit(m_cardID, m_axis, ui.d_minV->text().toDouble(), ui.d_maxV->text().toDouble(), ui.d_accT->text().toDouble(), ui.d_dccT->text().toDouble(), ui.d_stopV->text().toDouble());
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_set_profile_unit()  return:" + QString::number(result));
	}
	result = dmc_pmove_unit(m_cardID, m_axis, ui.d_dis->text().toDouble(), ui.d_disDir->currentIndex());
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_pmove_unit()  return:" + QString::number(result));
	}
}
void AxisWidget::on_lineCmpBtn_clicked() {
	result = dmc_hcmp_set_mode(m_cardID, ui.c_cmp->currentIndex(), 5);
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_hcmp_set_mode()  return:" + QString::number(result));
	}
	result = dmc_hcmp_set_config(m_cardID, ui.c_cmp->currentIndex(), m_axis, ui.c_souce->currentIndex(), ui.c_logic->currentIndex(), ui.c_time->text().toDouble()); // 1ms 的低电平脉冲宽度
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_hcmp_set_config()  return:" + QString::number(result));
	}
	result = dmc_hcmp_clear_points(m_cardID, ui.c_cmp->currentIndex());
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_hcmp_clear_points()  return:" + QString::number(result));
	}
	result = dmc_hcmp_set_liner_unit(m_cardID, ui.c_cmp->currentIndex(), ui.c_incPos->text().toDouble(), ui.c_count->text().toDouble());
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_hcmp_set_liner_unit()  return:" + QString::number(result));
	}
	result = dmc_hcmp_add_point_unit(m_cardID, ui.c_cmp->currentIndex(), ui.c_startPos->text().toDouble());
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_hcmp_add_point_unit()  return:" + QString::number(result));
	}
	result = dmc_set_profile_unit(m_cardID, m_axis, ui.c_minV->text().toDouble(), ui.c_maxV->text().toDouble(), ui.c_accT->text().toDouble(), ui.c_dccT->text().toDouble(), ui.c_stopV->text().toDouble());
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_set_profile_unit()  return:" + QString::number(result));
	}
	result = dmc_pmove_unit(m_cardID, m_axis, ui.c_startPos->text().toDouble() + ui.c_count->text().toDouble() * ui.c_incPos->text().toDouble(), 1);
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_pmove_unit()  return:" + QString::number(result));
	}
}
void AxisWidget::on_moveLMaxBtn_clicked() {
	result = dmc_set_profile_unit(m_cardID, m_axis, ui.d_minV->text().toDouble(), ui.d_maxV->text().toDouble(), ui.d_accT->text().toDouble(), ui.d_dccT->text().toDouble(), ui.d_stopV->text().toDouble());
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_set_profile_unit()  return:" + QString::number(result));
	}
	result = dmc_vmove(m_cardID, m_axis, 0);
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_vmove()  return:" + QString::number(result));
	}
}
void AxisWidget::on_moveLMinBtn_clicked() {
	result = dmc_set_profile_unit(m_cardID, m_axis, 0, ui.d_minV->text().toDouble(), ui.d_accT->text().toDouble(), ui.d_dccT->text().toDouble(), ui.d_stopV->text().toDouble());
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_set_profile_unit()  return:" + QString::number(result));
	}
	result = dmc_vmove(m_cardID, m_axis, 0);
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_vmove()  return:" + QString::number(result));
	}
}
void AxisWidget::on_moveRMinBtn_clicked() {
	result = dmc_set_profile_unit(m_cardID, m_axis, 0, ui.d_minV->text().toDouble(), ui.d_accT->text().toDouble(), ui.d_dccT->text().toDouble(), ui.d_stopV->text().toDouble());
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_set_profile_unit()  return:" + QString::number(result));
	}
	result = dmc_vmove(m_cardID, m_axis, 1);
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_vmove()  return:" + QString::number(result));
	}
}
void AxisWidget::on_moveRMaxBtn_clicked() {
	result = dmc_set_profile_unit(m_cardID, m_axis, ui.d_minV->text().toDouble(), ui.d_maxV->text().toDouble(), ui.d_accT->text().toDouble(), ui.d_dccT->text().toDouble(), ui.d_stopV->text().toDouble());
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_set_profile_unit()  return:" + QString::number(result));
	}
	result = dmc_vmove(m_cardID, m_axis, 1);
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_vmove()  return:" + QString::number(result));
	}
}
void AxisWidget::on_stopBtn_clicked() {
	result = dmc_stop(m_cardID, m_axis, 0);
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_stop()  return:" + QString::number(result));
	}
}
void AxisWidget::on_resetBtn_clicked() {
	result = dmc_set_position_unit(m_cardID, m_axis, 0);
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_set_position_unit()  return:" + QString::number(result));
	}
	result = dmc_set_encoder_unit(m_cardID, m_axis, 0);
	if (result != 0) {
		ui.resultEdit->appendPlainText("dmc_set_encoder_unit()  return:" + QString::number(result));
	}
}
void AxisWidget::on_sevonBtn_clicked() {
	if (ui.sevonBtn->text() == QString::fromLocal8Bit("开")) {
		result = dmc_write_sevon_pin(m_cardID, m_axis, 0);
		if (result != 0) {
			ui.resultEdit->appendPlainText("dmc_write_sevon_pin()  return:" + QString::number(result));
		}
		ui.sevonBtn->setText(QString::fromLocal8Bit("关"));
	}
	else {
		result = dmc_write_sevon_pin(m_cardID, m_axis, 1);
		if (result != 0) {
			ui.resultEdit->appendPlainText("dmc_write_sevon_pin()  return:" + QString::number(result));
		}
		ui.sevonBtn->setText(QString::fromLocal8Bit("开"));
	}
}
void AxisWidget::on_modeBanBtn_clicked(){
	for (int i = 0; i < 4; i++) {
		result = dmc_hcmp_set_mode(m_cardID, i, 0);
		if (result != 0) {
			ui.resultEdit->appendPlainText("dmc_hcmp_set_mode()  return:" + QString::number(result));
		}
	}
}

void AxisWidget::recvError(QString error){
	ui.resultEdit->appendPlainText(error);
}
