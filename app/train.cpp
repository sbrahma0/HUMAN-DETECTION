/*
 * @file train.cpp
 *
 * Created on: Oct 17, 2019
 * @author: sayan
 * @copyright [2019] Sayan Brahma
 * @brief Train class implementation for training the SVM parameters using HOG descriptors
 */

#include <train.hpp>
/*
 * @brief Constructor for the class; Initialisation
 */
Train::Train() {
    //  Initialize classifier
    classifier = cv::ml::SVM::create();
    // values to train svm
    classifier->setCoef0(0.0);
    classifier->setDegree(3);
    classifier->setTermCriteria(cv::TermCriteria(
    cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 1000, 1e-3));
    classifier->setGamma(0.0);
    classifier->setKernel(cv::ml::SVM::LINEAR);
    classifier->setNu(0.5);
    classifier->setP(0.1);  // for EPSILON_SVR
    classifier->setC(0.01);  //  soft classifier
    classifier->setType(cv::ml::SVM::EPS_SVR);
}
/*
 * @brief This function outputs the support vectors of the classifier.
 * @return A double vector containing the support vectors.
 */
std::vector<double> Train::GetClassifier() {
    //  get support vectors
    cv::Mat svm = classifier->getSupportVectors();
    //  get decision function
    cv::Mat alpha, svidx;
    double rho = classifier->getDecisionFunction(0, alpha, svidx);
    //  create variable to return
    std::vector<double> hogDetector(svm.cols+1);
    memcpy(&hogDetector[0], svm.ptr(), svm.cols*sizeof(hogDetector[0]));
    hogDetector[svm.cols] = static_cast<float>(-rho);
    return hogDetector;
}
/*
 * @brief This function extracts HOG descriptors from an image
 * @param It is the window size used for HOG feature extraction
 * @param Vector containing the images whose features need to be extracted
 */
void Train::GetHogFeatures(const cv::Size windowSize,
        const std::vector<cv::Mat> & imgList) {
    cv::HOGDescriptor hog;
    hog.winSize = windowSize;
    hog.cellSize = cv::Size(4, 4);
    cv::Mat gray;
    std::vector<float> descriptors;
    for ( auto data : imgList )
        if (data.cols >= windowSize.width && data.rows >=windowSize.height) {
            // convert image to grayscale
            cvtColor(data, gray, cv::COLOR_BGR2GRAY);
            // extract hog features
            hog.compute(gray, descriptors);
            // store hog features
            gradList.push_back(cv::Mat(descriptors).clone());
            // flip the images
            cv::flip(gray, gray, 1);
            hog.compute(gray, descriptors);
            gradList.push_back(cv::Mat(descriptors).clone());
         }
}
/**
 * @brief This function trains the SVM Classifier
 * @param saveClassifier Whether to save the trained classifier or not
 * @param classifierName Name of the classifer to be saved
 */
void Train::TrainSVM(const bool saveClassifier = false,
        const cv::String classifierName = "") {
    // check if HOG features are row vectors
    const int rows = static_cast<int>(gradList.size());
    const int cols = static_cast<int>(std::max(gradList[0].cols,
        gradList[0].rows));
    cv::Mat temp(1, cols, CV_32FC1), trainData(rows, cols, CV_32FC1);
    // auto index = 0;
    for (auto index = 0; index < gradList.size(); index++) {
        if (gradList[index].cols == 1) {
                    transpose(gradList[index], temp);
                    temp.copyTo(trainData.row(static_cast<int>(index)));
                  } else if (gradList[index].rows == 1) {
        gradList[index].copyTo(trainData.row(static_cast<int>(index)));
                }
           }
    }
/**
 * @brief Destructor of the train class
 */
Train::~Train() {
}

