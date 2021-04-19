#ifndef SCALESTABILIZER_H
#define SCALESTABILIZER_H

#include "Arduino.h"

class ScaleStabilizer {
private:
  double* _window;
  int _windowSize;
  double _weightThreshold;
  int _forceOverwrite;
  int _currentReadingIdx;
  double _lastOutputValue;

  /*!
   * @brief Returns the average
   * @return Returns the average of the moving window
   */
  double getAvg();

  /*!
   * @brief Returns true if moving window contains monotone elements 
   * @return True if all elements have the same value
   */
  bool isBufferMonotone();

public:
  /*!
   * @brief ScaleStabilizer begin initializer
   * @param windowSize represents the size of the moving window
   * @param weightThreshold represents the weight/noise threshold - e.g. 0.1g
   */
  void begin(int windowSize, double weightThreshold);

  /*!
   * @brief Adds the given reading to the moving window
   * @param reading ADC value/reading
   */
  void add(double reading);

  /*!
   * @brief Returns the stabilized load cell reading
   * @param displayResolution represents the output resolution - e.g. 0.1g accuracy
   * @param decimalPlaces represents the decimal places present in the output
   */
  double getStablilizedReading(double displayResolution = 0.1, int decimalPlaces = 1);
};

#endif
