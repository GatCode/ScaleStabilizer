#ifndef SCALESTABILIZER_H
#define SCALESTABILIZER_H

#include "Arduino.h"

class ScaleStabilizer
{
private:
  double *_window;
  int _windowSize;
  double _weightThreshold;
  int _forceOverwrite;
  int _currentReadingIdx;
  double _lastOutputValue;
  double _lastCodeValue;
  int _abortCounter;

  /*!
   * @brief Returns the M-2 average
   * @return Returns the M-2 average of the moving window
   */
  double getAvg();

public:
  /*!
   * @brief Construct a new Scale Stabilizer object
   */
  ScaleStabilizer();

  /*!
   * @brief Destroy the Scale Stabilizer object
   */
  ~ScaleStabilizer();

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
