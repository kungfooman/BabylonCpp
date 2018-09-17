#include <babylon/babylon_version.h>
#include <babylon/core/delegates/delegate.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include "sample_launcher.h"
#include <babylon/samples/samples_index.h>
#include <sstream>
#include "ccall.h"

struct ConsoleLogger {

  using log_message_t = BABYLON::delegate_t<void(const BABYLON::LogMessage&)>;

  static void onLogMessage(const BABYLON::LogMessage& logMessage)
  {
    printf("%s\n", logMessage.toString().c_str());
  }

  static log_message_t logListenerDelegate;

}; // end of struct ConsoleLogger

ConsoleLogger::log_message_t ConsoleLogger::logListenerDelegate
  = BABYLON::delegate_t<void(
    const BABYLON::LogMessage&)>::create<&ConsoleLogger::onLogMessage>();

/**
 * @brief Initializes the logging.
 */
void initializeLogging()
{
  static_assert(
    std::is_same<BABYLON::delegate_t<void(const BABYLON::LogMessage&)>,
                 decltype(ConsoleLogger::logListenerDelegate)>::value,
    "!");
  // Intialize log levels
  std::vector<std::pair<unsigned int, std::string>> _logLevels;
  for (auto& logLevel : BABYLON::LogLevels::Levels) {
    unsigned int logType = logLevel.first;
    if ((logType != BABYLON::LogLevels::LEVEL_QUIET)
        && (logType != BABYLON::LogLevels::LEVEL_TRACE)) {
      _logLevels.emplace_back(logLevel);
    }
  }
  // Subscribe to channels
  for (auto& logLevel : _logLevels) {
    unsigned int logType = logLevel.first;
    if (logType != BABYLON::LogLevels::LEVEL_QUIET) {
      BABYLON::Logger::Instance().registerLogMessageListener(
        logType, ConsoleLogger::logListenerDelegate);
    }
  }
}

/**
 * @brief Runs a sample with the specified name
 * @param samples list with available samples
 * @param sampleName name of the sample to run
 * @param showInspectorWindow whether or not to shoz the inspector window
 * @return exit code
 */
int runSample(const BABYLON::Samples::SamplesIndex& samples,
              const ::std::string& sampleName, bool showInspectorWindow,
              long runTimeMillis = 0)
{
  using namespace BABYLON::Samples;
  int exitcode = 0;
  // Create the sample launcher
  SampleLauncherOptions options;
  options.showInspectorWindow = showInspectorWindow;
  SampleLauncher sampleLauncher{options};
  if (sampleLauncher.intialize()) {
    // Create the renderable scene
    auto canvas = sampleLauncher.getRenderCanvas();
    auto scene  = samples.createRenderableScene(sampleName, canvas);
    sampleLauncher.setRenderableScene(scene);
    // Run the example
    exitcode = sampleLauncher.run(runTimeMillis);
  }
  return exitcode;
}

/**
 * @brief The sample launcher.
 * @param l list samples if l > 0
 * @param v enable verbose mode if v > 0
 * @param sample name of the sample to run
 * @return exit code
 */
int sampleLauncherMain(int l, int v, int i, const char* sampleGroup,
                       const char* sample)
{
  using namespace BABYLON::Samples;
  SamplesIndex samples;
  int exitcode = 0;
  if (l > 0) {
    ::std::ostringstream oss;
    // Get sample names and category names
    const auto sampleNames   = samples.getSampleNames();
    const auto categoryNames = samples.getCategoryNames();
    oss << "Found " << sampleNames.size() << " in " << categoryNames.size()
        << " categories:\n";
    // Print categorized samples
    for (const auto& categoryName : categoryNames) {
      const auto sampleNames = samples.getSampleNamesInCategory(categoryName);
      oss << "       |- ";
      if (sampleNames.size() < 10) {
        oss << " " << sampleNames.size();
      }
      else if (sampleNames.size() < 100) {
        oss << sampleNames.size();
      }
      oss << " sample(s) in category \"" << categoryName.c_str() << "\"\n";
      for (const auto& sampleName : sampleNames) {
        oss << "           |- " << sampleName.c_str() << "\n";
      }
    }
    printf("%s", oss.str().c_str());
  }
  else {
    if (v > 0) {
      // Enable console logging
      initializeLogging();
    }
    // Check for rendering of sample group
    const std::string categoryName{sampleGroup};
    if (!categoryName.empty()
        && (samples.categoryExists(categoryName) || categoryName == "All")) {
      ::std::vector<::std::string> categoryNames;
      if (categoryName == "All") {
        categoryNames = samples.getCategoryNames();
      }
      else {
        categoryNames = {categoryName};
      }
      for (const auto& categoryName : categoryNames) {
        const auto sampleNames = samples.getSampleNamesInCategory(categoryName);
        for (const auto& sampleName : sampleNames) {
          exitcode = runSample(samples, sampleName, i > 0, 10000);
          if (exitcode == 0) {
            break;
          }
        }
      }
    }
    else {
      // Check for rendering of sample
      const std::string sampleName{sample};
      if (!samples.sampleExists(sampleName)) {
        printf("Sample with name \"%s\" does not exists.\n", sample);
        return 1;
      }
      if (!samples.isSampleEnabled(sampleName)) {
        printf("Sample with name \"%s\" is not enabled.\n", sample);
        return 1;
      }
      // Run the sample
      exitcode = runSample(samples, sampleName, i > 0);
    }
  }
  return exitcode;
}

CCALL int launcher_main() {
	int exitcode = 0;
	//exitcode = sampleLauncherMain(0, 1, 0, "", "BasicScene");
	exitcode = sampleLauncherMain(0, 1, 0, "", "ImportMeshesSkullScene");
	return exitcode;
}
