/******************************************************************************\

  CAMotics is an Open-Source simulation and CAM software.
  Copyright (C) 2011-2019 Joseph Coffland <joseph@cauldrondevelopment.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

\******************************************************************************/

#include "CAMLayerDialog.h"

#include "ui_cam_layer_dialog.h"

using namespace CAMotics;
using namespace cb;
using namespace std;


#define UI() Dialog::getUI<Ui::CAMLayerDialog>()


CAMLayerDialog::CAMLayerDialog(QWidget *parent) :
  Dialog(parent, new UI<Ui::CAMLayerDialog>), metric(true) {}


void CAMLayerDialog::setLayers(const vector<string> &layers) {
  UI().layerComboBox->clear();
  for (unsigned i = 0; i < layers.size(); i++)
    UI().layerComboBox->addItem(QString::fromUtf8(layers[i].c_str()));
}


void CAMLayerDialog::setUnits(GCode::Units units) {
  metric = units == GCode::Units::METRIC;

  UI().feedSpinBox->setSuffix(metric ? " mm/min" : " in/min");
  UI().offsetDoubleSpinBox->setSuffix(metric ? " mm" : " in");
  UI().startDepthDoubleSpinBox->setSuffix(metric ? " mm" : " in");
  UI().endDepthDoubleSpinBox->setSuffix(metric ? " mm" : " in");
  UI().stepDoubleSpinBox->setSuffix(metric ? " mm" : " in");
}


string CAMLayerDialog::getOffsetType(int index) const {
  return String::toLower(UI().offsetComboBox->itemText(index).toUtf8().data());
}


string CAMLayerDialog::getOffsetType() const {
  return getOffsetType(UI().offsetComboBox->currentIndex());
}


void CAMLayerDialog::setOffsetType(const string &offset) {
  for (int i = 0; i < UI().offsetComboBox->count(); i++)
    if (offset == getOffsetType(i)) UI().offsetComboBox->setCurrentIndex(i);
}


void CAMLayerDialog::update() {
  UI().offsetDoubleSpinBox->setEnabled(getOffsetType() == "custom");
}


int CAMLayerDialog::exec() {
  update();
  return QDialog::exec();
}


void CAMLayerDialog::read(const JSON::Value &value) {
  const double scale = metric ? 1 : 25.4;

  UI().toolSpinBox->setValue(value.getNumber("tool", 0));
  UI().feedSpinBox->setValue(value.getNumber("feed", 0) * scale);
  UI().speedSpinBox->setValue(value.getNumber("speed", 0));
  setOffsetType(value.getString("offset-type", "none"));
  UI().offsetDoubleSpinBox->setValue(value.getNumber("offset", 0));
  UI().startDepthDoubleSpinBox->
    setValue(value.getNumber("start-depth", 0) * scale);
  UI().endDepthDoubleSpinBox->setValue(value.getNumber("end-depth", 0) * scale);
  UI().stepDoubleSpinBox->setValue(value.getNumber("max-step-down", 0) * scale);
}


void CAMLayerDialog::write(JSON::Sink &sink) const {
  const double scale = metric ? 1 : 25.4;

  sink.beginDict();
  sink.insert("name", UI().layerComboBox->currentText().toUtf8().data());
  sink.insert("tool", (int)UI().toolSpinBox->value());
  sink.insert("feed", UI().feedSpinBox->value() / scale);
  sink.insert("speed", UI().speedSpinBox->value());
  sink.insert("offset-type", getOffsetType());
  if (getOffsetType() == "custom")
    sink.insert("offset", UI().offsetDoubleSpinBox->value());
  sink.insert("start-depth", UI().startDepthDoubleSpinBox->value() / scale);
  sink.insert("end-depth", UI().endDepthDoubleSpinBox->value() / scale);
  sink.insert("max-step-down", UI().stepDoubleSpinBox->value() / scale);
  sink.endDict();
}


void CAMLayerDialog::on_startDepthDoubleSpinBox_valueChanged(double x) {
  if (x < UI().endDepthDoubleSpinBox->value())
    UI().endDepthDoubleSpinBox->setValue(x);
}


void CAMLayerDialog::on_endDepthDoubleSpinBox_valueChanged(double x) {
  if (UI().startDepthDoubleSpinBox->value() < x)
    UI().startDepthDoubleSpinBox->setValue(x);
}
