#pragma once

#include <QWidget>
#include <cstdint>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QNetworkReply>

class Satellite final : public QWidget {
  Q_OBJECT

public:
  explicit Satellite(QWidget *parent = nullptr);

private slots:
  void loadFromFile();

  void loadFromURL();

  void handleNetworkReply(QNetworkReply *reply) const;

  void processTLEData(const QString &data) const;

  void saveToFile();

private:
  static constexpr int32_t FRAME_SIZE = 3;
  static constexpr int32_t MIN_YEAR_CUTOFF = 57;
  static constexpr int32_t BASE_YEAR_2000 = 2000;
  static constexpr int32_t BASE_YEAR_1900 = 1900;
  static constexpr int32_t MIN_LINE1_LENGTH = 20;
  static constexpr int32_t MIN_LINE2_LENGTH = 16;
  QPushButton *fileButton;
  QPushButton *urlButton;
  QPushButton *saveButton;
  QPlainTextEdit *consoleOutput;
  QNetworkAccessManager *networkManager;
  QString currentSource;
};
