#include "TimeStamp.h"
USESTD;
USECRPT;
namespace liao::Util
{
    void TimeStamp::setTime(const TimePoint& time)
    {
        WriteLock lock(m_mutex);
        m_timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            time.time_since_epoch()
        ).count();
    }
    void TimeStamp::initializeString()
    {
        if (m_literal.capacity() < TIMESTAMP_LENGTH)
            m_literal.reserve(TIMESTAMP_LENGTH);
    }
    void TimeStamp::convertToString(const TimePoint& time)
    {
        initializeString();
        if (m_timestamp > 0)
           m_literal = format("{:%Y-%m-%d %H:%M:%S}", time);
    }
    TimeStamp::TimeStamp()
        : m_timestamp(0)
    {
        initializeString();
    }
    TimeStamp::TimeStamp(const TimePoint now)
    {
        reset(now);
    }
    TimeStamp::TimeStamp(const TimeStamp& obj)
        :m_timestamp(obj.m_timestamp),m_literal(obj.m_literal)
    {}
    TimeStamp::TimeStamp(TimeStamp&& obj) noexcept
        :m_literal(std::move(obj.m_literal)),m_timestamp(obj.m_timestamp)
    {}
    const std::string& TimeStamp::getString() const
    {
        return m_literal;
    }
    std::string&& TimeStamp::moveString()
    {
        return std::move(m_literal);
    }
    long long TimeStamp::getTimestamp() const
    {
        return m_timestamp;
    }
    void TimeStamp::reset(const TimePoint now)
    {
        setTime(now);
        convertToString(now);
    }
    void TimeStamp::clear()
    {
        m_literal.clear();
        m_timestamp = 0;
    }
    bool TimeStamp::isEmpty() const
    {
        return m_literal.empty()|| m_timestamp==0;
    }
    bool TimeStamp::compare(const TimeStamp& obj) const
    {
        return distance(obj)==0;
    }
    bool TimeStamp::isEarlier(const TimeStamp& obj) const
    {
        return distance(obj)<0;
    }
    bool TimeStamp::isLater(const TimeStamp& obj) const
    {
        return distance(obj)>0;
    }
    long long TimeStamp::distance(const TimeStamp& obj) const
    {
        return m_timestamp - obj.m_timestamp;
    }
}