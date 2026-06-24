#include "sandbox/SceneFolderMonitor.hpp"

#include <system_error>

namespace sandbox
{
SceneFolderMonitor::SceneFolderMonitor(
    std::filesystem::path folder,
    std::chrono::milliseconds pollInterval)
    : mFolder(std::move(folder)),
      mPollInterval(pollInterval)
{
}

SceneFolderMonitor::~SceneFolderMonitor()
{
    stop();
}

bool SceneFolderMonitor::FileState::operator==(const FileState& other) const
{
    return lastWriteTime == other.lastWriteTime &&
           size == other.size;
}

bool SceneFolderMonitor::FileState::operator!=(const FileState& other) const
{
    return !(*this == other);
}

void SceneFolderMonitor::start()
{
    std::lock_guard<std::mutex> lock(mMutex);

    if (mRunning)
    {
        return;
    }

    mFiles = scanFolder();

    // Treat startup as a change so Application can parse all existing scenes.
    mHasChanges = true;
    mStopRequested = false;
    mRunning = true;

    mMonitorThread = std::thread(&SceneFolderMonitor::monitorLoop, this);
}

void SceneFolderMonitor::stop()
{
    {
        std::lock_guard<std::mutex> lock(mMutex);

        if (!mRunning)
        {
            return;
        }

        mStopRequested = true;
    }

    mStopCondition.notify_one();

    if (mMonitorThread.joinable())
    {
        mMonitorThread.join();
    }

    std::lock_guard<std::mutex> lock(mMutex);
    mRunning = false;
}

std::vector<std::filesystem::path> SceneFolderMonitor::getFiles() const
{
    std::lock_guard<std::mutex> lock(mMutex);

    return getFilesLocked();
}

std::optional<std::vector<std::filesystem::path>> SceneFolderMonitor::consumeChanges()
{
    std::lock_guard<std::mutex> lock(mMutex);

    if (!mHasChanges)
    {
        return std::nullopt;
    }

    mHasChanges = false;

    return getFilesLocked();
}

SceneFolderMonitor::FileSnapshot SceneFolderMonitor::scanFolder() const
{
    FileSnapshot snapshot;

    std::error_code error;

    if (!std::filesystem::exists(mFolder, error) || !std::filesystem::is_directory(mFolder, error))
    {
        return snapshot;
    }

    for (const auto& entry : std::filesystem::directory_iterator(mFolder, error))
    {
        if (error)
        {
            break;
        }

        if (!entry.is_regular_file(error))
        {
            continue;
        }

        const auto last_write_time = entry.last_write_time(error);

        if (error)
        {
            continue;
        }

        const auto size = entry.file_size(error);

        if (error)
        {
            continue;
        }

        snapshot.emplace(entry.path(), FileState{last_write_time, size});
    }

    return snapshot;
}

std::vector<std::filesystem::path> SceneFolderMonitor::getFilesLocked() const
{
    std::vector<std::filesystem::path> files;
    files.reserve(mFiles.size());

    for (const auto& [path, state] : mFiles)
    {
        files.push_back(path);
    }

    return files;
}

void SceneFolderMonitor::monitorLoop()
{
    while (true)
    {
        {
            std::unique_lock<std::mutex> lock(mMutex);

            if (mStopCondition.wait_for(lock, mPollInterval, [this]()
                    {
                        return mStopRequested;
                    }))
            {
                return;
            }
        }

        const auto new_files = scanFolder();

        std::lock_guard<std::mutex> lock(mMutex);

        if (new_files != mFiles)
        {
            mFiles = new_files;
            mHasChanges = true;
        }
    }
}
}