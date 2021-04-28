#include "ScaleStabilizer.h"

/*!
 * @brief construct a new ScaleStabilizer:: Scale Stabilizer object
 */
ScaleStabilizer::ScaleStabilizer() {}

/*!
 * @brief deconstruct a ScaleStabilizer object
 */
ScaleStabilizer::~ScaleStabilizer()
{
  delete _window;
}

/*!
 * @brief M-2 average calculation
 */
double ScaleStabilizer::getAvg()
{
  // find min and max values
  int minIdx = 0;
  double minReading = _window[minIdx];
  int maxIdx = 0;
  double maxReading = _window[maxIdx];
  for (int i = 0; i < _windowSize; i++)
  {
    if (_window[i] < minReading)
    {
      minIdx = i;
      minReading = _window[i];
    }
    if (_window[i] > maxReading)
    {
      maxIdx = i;
      maxReading = _window[i];
    }
  }

  // remove min and max from measurements
  double sum = 0;
  for (int i = 0; i < _windowSize; i++)
  {
    if (i == minIdx || i == maxIdx)
    {
      continue;
    }
    sum += _window[i];
  }

  return sum / (_windowSize - 2);
}

/*!
 * @brief init of the stabilizer
 */
void ScaleStabilizer::begin(int windowSize, double weightThreshold)
{
  _windowSize = windowSize;
  _weightThreshold = weightThreshold;
  _window = new double[_windowSize];
  _forceOverwrite = _windowSize;
  _currentReadingIdx = 0;
  _lastOutputValue = 0.0;
  _abortCounter = _windowSize / 2;

  for (int i = 0; i < _windowSize; i++)
  {
    _window[i] = 0.0;
  }
}

/*!
 * @brief add a new ADC reading to the moving _window by replacing the olest one
 * but if the moving _window is empty or needs to be overwritten, force overwrite is executed
 */
void ScaleStabilizer::add(double reading)
{
  if (_forceOverwrite > 0)
  {
    int oldestReadingIdx = (_currentReadingIdx + 1) % _windowSize;
    _window[oldestReadingIdx] = reading;
    _currentReadingIdx = oldestReadingIdx;
    _forceOverwrite--;
    return;
  }

  if (abs(reading - getAvg()) < _weightThreshold)
  {
    int oldestReadingIdx = (_currentReadingIdx + 1) % _windowSize;
    _window[oldestReadingIdx] = reading;
    _currentReadingIdx = oldestReadingIdx;
    if (_abortCounter < _windowSize / 2)
    {
      _abortCounter++;
    }
    return;
  }

  // reading index just before the last one
  int nextToLastReadingIdx = _currentReadingIdx - 1 < 0 ? _windowSize : (_currentReadingIdx - 1) % _windowSize;

  if (abs(_window[nextToLastReadingIdx] - getAvg()) > _weightThreshold)
  {
    // replace all the data in the filter _window with fresh readings for the next six cycles
    _forceOverwrite = _windowSize * 6;
    return;
  }

  // everything else is probably just noise but abort if stuck
  if (_abortCounter <= 0)
  {
    _forceOverwrite = _windowSize;
    _abortCounter = _windowSize / 2;
  }
  _abortCounter--;
}

/*!
 * @brief returns the stabilized average reading from the moving _window
 */
double ScaleStabilizer::getStablilizedReading(double displayResolution, int decimalPlaces)
{
  double codeValue = getAvg();
  double outputValue = codeValue;

  // remove negative numbers and set to 0 if close to 0
  if (outputValue < 0.5)
  {
    outputValue = 0.0;
  }

  // round to given decimal places
  float scale = pow(10, decimalPlaces);
  outputValue = round(outputValue * scale) / scale;

  // perform the output flicker reduction
  if (_lastOutputValue == outputValue)
  {
    _lastCodeValue = codeValue;
    return _lastOutputValue;
  }

  if (abs(codeValue - _lastCodeValue) > displayResolution)
  {
    _lastCodeValue = codeValue;
    _lastOutputValue = outputValue;
  }

  return _lastOutputValue;
}
