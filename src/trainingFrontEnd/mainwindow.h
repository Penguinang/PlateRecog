#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidgetItem>
#include <QMainWindow>

#include <memory>
#include <vector>

#include <opencv2/core.hpp>
using cv::Mat;
#include <opencv2/ml.hpp>
using cv::ml::SVM;

/*--------  Forward declarations  --------*/
namespace Doit {
namespace CV {
namespace PlateRecogn {
enum class PlateChar_t;
} // namespace PlateRecogn
} // namespace CV
} // namespace Doit
using Doit::CV::PlateRecogn::PlateChar_t;

namespace Ui {
class MainWindow;
}

class TrainWorker;
class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:
    void on_openFolder();

    void on_allFiles_currentItemChanged(QListWidgetItem *current,
                                        QListWidgetItem *previous);

    void on_startTrainButton_clicked();
    void after_trainingCompleting();
    bool showOverwriteModel_messageBox();

    void on_kernel_comboBox_currentIndexChanged(int index);
	// void showValidations();
	void prediction_Completed();

    // void on_allTestFiles_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_filesSelection_comboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;

    // images data
  private:
    std::vector<Mat> images;
    std::vector<QString> paths;
    std::vector<PlateChar_t> tags;
    std::vector<std::vector<float>> Hogs;

    void showLoadedImages();
    QImage Mat2QImage(const cv::Mat &mat, QImage::Format format);
    enum DisplayContent{
        ALL_IMAGES,
        TRAINING_IMAGES,
        VALIDATION_IMAGES,
        CORRETC_PREDICTED_IMAGES,
        WRONG_PREDICTED_IMAGES
    };
    void showAllImages();
    void showTrainingImages();
    void showValidationImages();
    void showCorrectValidationImages();
    void showWrongValidationImages();

    void showAllImagesDetail(int index);
    void showTrainingImagesDetail(int index);
    void showValidationImagesDetail(int index);
    void showCorrectValidationImagesDetail(int index);
    void showWrongValidationImagesDetail(int index);



    // SVM parameters
  private:
    double C = 1;
    double gamma = 1;
    SVM::KernelTypes kernel = SVM::KernelTypes::RBF;
    float degree = 1;

    std::vector<SVM::KernelTypes> candidateKernels = {
        SVM::KernelTypes::LINEAR, SVM::KernelTypes::RBF,
        SVM::KernelTypes::POLY,   SVM::KernelTypes::CHI2,
        SVM::KernelTypes::INTER,  SVM::KernelTypes::SIGMOID};
    std::vector<QString> kernelStrings = {
        "LINEAR", "RBF", "POLY", "CHI2", "INTER", "SIGMOID",
    };

    static QString modelFileName;
    friend class TrainWorker;
    TrainWorker *worker = nullptr;
    QThread *trainingThread = nullptr;

	void splitDataSet(float trainingRatio = 0.9);
	std::vector<size_t> trainingIndices;
	std::vector<size_t> validationIndices;
	std::vector<int> validationPrediction;
    std::vector<int> correctValidationIndices;
    std::vector<int> wrongValidationIndices;
};

// auto friend
class TrainWorker : public QObject {
    Q_OBJECT
  public:
    TrainWorker(MainWindow *mainWindow) : mainWindow(mainWindow){};

  private:
    MainWindow *mainWindow = nullptr;

  public slots:
    void process();
  signals:
    void finished();
    void error(QString err);
    bool showOverwriteModel_messageBox();
	void prediction_Completed();

  private:
    void train(MainWindow *mainWindow);
};

#endif // MAINWINDOW_H
