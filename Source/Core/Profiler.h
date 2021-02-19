#pragma once

using Clock = std::chrono::high_resolution_clock;
using TimeStamp = Clock::time_point; //std::chrono::time_point<Clock, std::chrono::duration<float, std::milli>>;
using Duration = Clock::duration; //std::chrono::duration<float, std::milli>;
using StatisticLabel = char const*;

struct StatisticDataBase
{
    std::thread::id thread;
    TimeStamp startTimestamp;
    Duration duration;
};

struct GenericStatisticData : StatisticDataBase
{
    StatisticLabel label;
};

struct GameSystemStatisticData : StatisticDataBase
{
    enum class EType
    {
        System,
        ThreadOverhead
    };

    StatisticLabel label;
    EType type;
};

class Profiler;

template<class T>
class ScopedStatistic
{
public:
    ScopedStatistic()
    {
        B2D_STATIC_ASSERT_TYPE(StatisticDataBase, T);

        m_data.thread = std::this_thread::get_id();
        m_data.startTimestamp = Clock::now();
    }

    virtual ~ScopedStatistic()
    {
        m_data.duration = Clock::now() - m_data.startTimestamp;
        Profiler::AddStatistic<T>(m_data);
    }

protected:
    T m_data;
};

class ScopedGenericStatistic : public ScopedStatistic<GenericStatisticData>
{
public:
    ScopedGenericStatistic(StatisticLabel label);
};

class ScopedGameSystemStatistic : public ScopedStatistic<GameSystemStatisticData>
{
public:
    ScopedGameSystemStatistic(GameSystemStatisticData::EType type, StatisticLabel label);
};

#define PROFILE_SCOPE() _NODISCARD ScopedGenericStatistic _remitdepocs(__FUNCSIG__);

#define PROFILE_GAME_SYSTEM_BEGIN_FRAME() Profiler::OnGameSystemBeginFrame();
#define PROFILE_GAME_SYSTEM(label) ScopedGameSystemStatistic _remitdepocs(GameSystemStatisticData::EType::System, label);
#define PROFILE_GAME_SYSTEM_THREAD_OVERHEAD(label) ScopedGameSystemStatistic _remitdepocs(GameSystemStatisticData::EType::ThreadOverhead, label);
#define PROFILE_GAME_SYSTEM_END_FRAME() Profiler::OnGameSystemEndFrame();

using GenericStatisticDataList = std::vector<GenericStatisticData>;
using GameSystemStatisticDataList = std::vector<GameSystemStatisticData>;

class Profiler final
{
private:
    Profiler();

public:
    template<class T>
    FORCEINLINE static void AddStatistic(T const& data);

    template<>
    FORCEINLINE static void AddStatistic<GenericStatisticData>(GenericStatisticData const& data)
    {
        ms_mutex.lock();
        ms_genericStatistics.emplace_back(data);
        ms_mutex.unlock();
    }

    template<>
    FORCEINLINE static void AddStatistic<GameSystemStatisticData>(GameSystemStatisticData const& data)
    {
        ms_mutex.lock();
        ms_gameSystemStatistics.emplace_back(data);
        ms_mutex.unlock();
    }

    static GenericStatisticDataList const& GetGenericStatistics() { return ms_genericStatistics; }
    static uint64 GetGameSystemStatistics(GameSystemStatisticDataList const*& dataList, TimeStamp& start, TimeStamp& end) {
        start = ms_gameStart; end = ms_gameEnd; dataList = &ms_gameSystemStatistics; return ms_frameId;
    }

    static void OnGameSystemBeginFrame();
    static void OnGameSystemEndFrame();

private:
    static GenericStatisticDataList ms_genericStatistics;
    static GameSystemStatisticDataList ms_gameSystemStatistics;
    static std::mutex ms_mutex;
    static uint64 ms_frameId;
    static TimeStamp ms_gameStart;
    static TimeStamp ms_gameEnd;
};
