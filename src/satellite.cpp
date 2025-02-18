#include "satellite.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QFile>
#include <QTextStream>

Satellite::Satellite(QWidget *parent) : QWidget(parent) {
  auto *layout = new QVBoxLayout(this);

  fileButton = new QPushButton("Load from File", this);
  urlButton = new QPushButton("Load from URL", this);
  saveButton = new QPushButton("Save to File", this);
  consoleOutput = new QPlainTextEdit(this);
  consoleOutput->setReadOnly(true);

  layout->addWidget(fileButton);
  layout->addWidget(urlButton);
  layout->addWidget(saveButton);
  layout->addWidget(consoleOutput);

  networkManager = new QNetworkAccessManager(this);

  connect(fileButton, &QPushButton::clicked, this, &Satellite::loadFromFile);
  connect(urlButton, &QPushButton::clicked, this, &Satellite::loadFromURL);
  connect(saveButton, &QPushButton::clicked, this, &Satellite::saveToFile);
  connect(networkManager, &QNetworkAccessManager::finished, this, &Satellite::handleNetworkReply);
}

void Satellite::loadFromFile() {
  if (const QString filePath = QFileDialog::getOpenFileName(this, "Select TLE File", "", "TLE Files (*.txt *.tle)");
    !filePath.isEmpty()) {
    if (QFile file(filePath);
      file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      const QString tleData = file.readAll();
      file.close();
      consoleOutput->appendPlainText("[FILE] Loaded from: " + filePath);
      currentSource = "file";
      processTLEData(tleData);
    } else {
      consoleOutput->appendPlainText("[ERROR] Unable to open file.");
    }
  }
}

void Satellite::loadFromURL() {
  bool ok;

  if (const QString url = QInputDialog::getText(this, "Enter URL", "Enter the URL for TLE data:",
                                                QLineEdit::Normal, "", &ok);
    ok && !url.isEmpty()) {
    consoleOutput->appendPlainText("[URL] Fetching data from: " + url);
    currentSource = "url";
    networkManager->get(QNetworkRequest(QUrl(url)));
  } else {
    consoleOutput->appendPlainText("[ABORTED] URL input cancelled.");
  }
}

void Satellite::handleNetworkReply(QNetworkReply *reply) const {
  if (currentSource != "url") {
    consoleOutput->appendPlainText("[ERROR] Expected source was different.");
    reply->deleteLater();
    return;
  }

  if (reply->error() == QNetworkReply::NoError) {
    const QString tleData = reply->readAll();
    consoleOutput->appendPlainText("[URL] Successfully loaded data.");
    processTLEData(tleData);
  } else {
    consoleOutput->appendPlainText("[ERROR] Failed to fetch: " + reply->errorString());
  }

  reply->deleteLater();
}

void Satellite::processTLEData(const QString &data) const {
  if (data.isEmpty()) {
    consoleOutput->appendPlainText("[ERROR] No valid data to process.");
    return;
  }

  QStringList lines = data.split("\n", Qt::SkipEmptyParts);
  const int32_t totalSatellites = lines.size() / FRAME_SIZE;

  if (totalSatellites == 0) {
    consoleOutput->appendPlainText("[ERROR] No valid satellite data found.");
    return;
  }

  QMap<int32_t, int32_t> launchesByYear;
  QMap<int32_t, int32_t> inclinationMap;
  int32_t oldestYear = INT_MAX;

  for (int32_t i = 0; i < lines.size(); i += FRAME_SIZE) {
    if (i + 2 >= lines.size()) continue;

    QString line1 = lines[i + 1];
    QString line2 = lines[i + 2];

    if (line1.length() < MIN_LINE1_LENGTH || line2.length() < MIN_LINE2_LENGTH) continue;

    int32_t launchYear = line1.midRef(9, 2).toInt();
    launchYear = (launchYear < MIN_YEAR_CUTOFF) ? (BASE_YEAR_2000 + launchYear) : (BASE_YEAR_1900 + launchYear);
    launchesByYear[launchYear]++;
    if (launchYear < oldestYear) {
      oldestYear = launchYear;
    }

    bool converted;
    const double inclination = line2.midRef(8, 8).toDouble(&converted);
    if (converted) {
      auto roundedInclination = static_cast<int32_t>(inclination);
      inclinationMap[roundedInclination]++;
    }
  }

  QString result;
  result += "Total satellites: " + QString::number(totalSatellites) + "\n\n";
  result += "Oldest launch year: " + QString::number(oldestYear) + "\n\n";

  result += "Distribution by year:\n";
  for (auto it = launchesByYear.cbegin(); it != launchesByYear.cend(); ++it) {
    result += QString::number(it.key()) + ": " + QString::number(it.value()) + "\n";
  }

  result += "\nDistrbution by incline:\n";
  for (auto it = inclinationMap.cbegin(); it != inclinationMap.cend(); ++it) {
    result += QString::number(it.key()) + "°: " + QString::number(it.value()) + "\n";
  }

  consoleOutput->appendPlainText(result);
}

void Satellite::saveToFile() {
  const QString filePath = QFileDialog::getSaveFileName(this, "Save Output", "", "Text Files (*.txt);;All Files (*)");
  if (filePath.isEmpty()) {
    consoleOutput->appendPlainText("[ABORTED] Save operation cancelled.");
    return;
  }

  QFile file(filePath);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&file);
    out << consoleOutput->toPlainText();
    file.close();
    consoleOutput->appendPlainText("[SUCCESS] Output saved to: " + filePath);
  } else {
    consoleOutput->appendPlainText("[ERROR] Unable to save file.");
  }
}
