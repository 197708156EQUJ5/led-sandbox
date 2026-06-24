#pragma once

#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <map>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

namespace sandbox
{
class SceneFolderMonitor
{
public:
    explicit SceneFolderMonitor(
        std::filesystem::path folder,
        std::chrono::milliseconds pollInterval = std::chrono::milliseconds(500));

    ~SceneFolderMonitor();

    SceneFolderMonitor(const SceneFolderMonitor&) = delete;
    SceneFolderMonitor& operator=(const SceneFolderMonitor&) = delete;

    void start();
    void stop();

    // Thread-safe snapshot of every regular file currently in the folder.
    std::vector<std::filesystem::path> getFiles() const;

    // Returns all current files when something changed since the last call.
    // Returns std::nullopt when nothing has changed.
    std::optional<std::vector<std::filesystem::path>> consumeChanges();

private:
    struct FileState
    {
        std::filesystem::file_time_type lastWriteTime;
        std::uintmax_t size{0};

        bool operator==(const FileState& other) const;
        bool operator!=(const FileState& other) const;
    };

    using FileSnapshot = std::map<std::filesystem::path, FileState>;

    FileSnapshot scanFolder() const;
    std::vector<std::filesystem::path> getFilesLocked() const;
    void monitorLoop();

    std::filesystem::path mFolder;
    std::chrono::milliseconds mPollInterval;

    mutable std::mutex mMutex;
    std::condition_variable mStopCondition;

    std::thread mMonitorThread;

    FileSnapshot mFiles;
    bool mRunning{false};
    bool mHasChanges{false};
    bool mStopRequested{false};
};
}