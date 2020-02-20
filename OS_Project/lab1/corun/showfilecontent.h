#ifndef SHOWFILECONTENT_H
#define SHOWFILECONTENT_H

#include <QDialog>
#include <fstream>

namespace Ui {
class ShowFileContent;
}

class ShowFileContent : public QDialog {
    Q_OBJECT

  public:
    explicit ShowFileContent(QWidget *parent = nullptr);
    ~ShowFileContent();

  private slots:
    void printFile();

  private:
    std::ifstream fp;
    Ui::ShowFileContent *ui;
};

#endif // SHOWFILECONTENT_H
