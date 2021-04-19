#include "ScaleStabilizer.h"

/*!
 * @brief Construct a new Scale Stabilizer:: S Cale Stabilizer object
 */
ScaleStabilizer::SCaleStabilizer()
{
}

ScaleStabilizer::~ScaleStabilizer()
{
  delete _window;
}

/*!
 * @brief simple average calculation
 */
double ScaleStabilizer::getAvg()
{
  double sum = 0;

  for (int i = 0; i < _windowSize; i++)
  {
    sum += _window[i];
  }

  return sum / _windowSize;
}

/*!
 * @brief checks if all the elements in the moving _window are the same
 */
bool ScaleStabilizer::isBufferMonotone()
{
  for (int i = 0; i < _windowSize; i++)
  {
    if (_window[i] != _window[0])
    {
      break;
    }
  }
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
}

/*!
 * @brief add a new ADC reading to the moving _window by replacing the olest one
 * but if the moving _window is empty or needs to be overwritten, force overwrite is executed
 */
void ScaleStabilizer::add(double reading)
{
  if (_forceOverwrite > 0 || isBufferMonotone())
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
    return;
  }

  // reading index just before the last one
  int nextToLastReadingIdx = _currentReadingIdx - 1 < 0 ? _windowSize : (_currentReadingIdx - 1) % _windowSize;

  if (abs(_window[nextToLastReadingIdx] - getAvg()) > _weightThreshold)
  {
    // replace 75% of the data in the filter _window with fresh readings
    _forceOverwrite = _windowSize * 0.75;
  }

  // everything else is probably just noise
}

/*!
 * @brief returns the stabilized average reading from the moving _window
 */
double ScaleStabilizer::getStablilizedReading(double displayResolution, int decimalPlaces)
{
  // find min and max values
  int minIdx = 0;
  int maxIdx = 0;
  for (int i = 0; i < _windowSize; i++)
  {
    if (_window[i] < minIdx)
    {
      minIdx = i;
    }
    if (_window[i] > maxIdx)
    {
      maxIdx = i;
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

  // calc cleaned average
  double cleanedAverage = sum / (_windowSize - 2);

  // remove negative numbers and set to 0 if close to 0
  if (cleanedAverage < 0.5)
  {
    cleanedAverage = 0.0;
  }

  // round to given decimal places
  float scale = pow(10, decimalPlaces);
  cleanedAverage = round(cleanedAverage * scale) / scale;

  // perform the output flicker reduction
  if (_lastOutputValue == cleanedAverage)
  {
    return cleanedAverage;
  }

  if (abs(cleanedAverage - _lastOutputValue) < displayResolution)
  {
    return _lastOutputValue; // probably was just noise - return the old measurement
  }

  _lastOutputValue = cleanedAverage;
  return cleanedAverage;
}
